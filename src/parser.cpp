#include "parser.h"
#include <clang-c/Index.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

namespace codemap {

namespace fs = std::filesystem;

struct ParserContext {
    FunctionGraph* graph;
    std::string current_file;
    std::unordered_map<std::string, int> function_indices;
    std::unordered_set<std::string> processed_functions;
};

static std::string getCursorSpelling(CXCursor cursor) {
    CXString spelling = clang_getCursorSpelling(cursor);
    std::string result = clang_getCString(spelling);
    clang_disposeString(spelling);
    return result;
}

static std::string getCursorKindSpelling(CXCursor cursor) {
    CXString kindSpelling = clang_getCursorKindSpelling(clang_getCursorKind(cursor));
    std::string result = clang_getCString(kindSpelling);
    clang_disposeString(kindSpelling);
    return result;
}

static std::string getFileName(CXCursor cursor) {
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned line, column;
    clang_getExpansionLocation(location, &file, &line, &column, nullptr);
    
    if (file) {
        CXString filename = clang_getFileName(file);
        std::string result = clang_getCString(filename);
        clang_disposeString(filename);
        return result;
    }
    return "";
}

static unsigned getLineNumber(CXCursor cursor) {
    CXSourceLocation location = clang_getCursorLocation(cursor);
    unsigned line;
    clang_getExpansionLocation(location, nullptr, &line, nullptr, nullptr);
    return line;
}

static bool isFunctionImplementation(CXCursor cursor) {
    CXCursorKind kind = clang_getCursorKind(cursor);
    return kind == CXCursor_FunctionDecl || kind == CXCursor_CXXMethod;
}

static bool isStubFunction(CXCursor cursor) {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXToken* tokens = nullptr;
    unsigned numTokens = 0;
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
    
    clang_tokenize(tu, range, &tokens, &numTokens);
    
    bool hasBody = false;
    bool isStub = false;
    
    for (unsigned i = 0; i < numTokens; ++i) {
        CXString tokenSpelling = clang_getTokenSpelling(tu, tokens[i]);
        std::string tokenStr = clang_getCString(tokenSpelling);
        clang_disposeString(tokenSpelling);
        
        if (tokenStr == "{") {
            hasBody = true;
        }
        
        if (hasBody && (tokenStr.find("TODO") != std::string::npos ||
                       tokenStr.find("NotImplemented") != std::string::npos ||
                       tokenStr.find("UNIMPLEMENTED") != std::string::npos ||
                       tokenStr.find("throw") != std::string::npos)) {
            isStub = true;
            break;
        }
    }
    
    if (tokens) {
        clang_disposeTokens(tu, tokens, numTokens);
    }
    
    return isStub;
}

static CXChildVisitResult findFunctionCallsVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    ParserContext* context = static_cast<ParserContext*>(client_data);
    
    CXCursorKind kind = clang_getCursorKind(cursor);
    
    if (kind == CXCursor_CallExpr || kind == CXCursor_MemberRefExpr) {
        CXCursor referenced = clang_getCursorReferenced(cursor);
        if (!clang_Cursor_isNull(referenced)) {
            std::string calleeName = getCursorSpelling(referenced);
            
            if (!calleeName.empty()) {
                std::string parentName = getCursorSpelling(parent);
                while (parentName.empty() && !clang_Cursor_isNull(parent)) {
                    parent = clang_getCursorSemanticParent(parent);
                    if (isFunctionImplementation(parent)) {
                        parentName = getCursorSpelling(parent);
                        break;
                    }
                }
                
                if (!parentName.empty() && parentName != calleeName) {
                    auto callerIt = context->function_indices.find(parentName);
                    auto calleeIt = context->function_indices.find(calleeName);
                    
                    if (callerIt == context->function_indices.end()) {
                        FunctionNode callerNode(parentName, context->current_file, getLineNumber(parent));
                        int callerIndex = context->graph->addNode(callerNode);
                        context->function_indices[parentName] = callerIndex;
                        callerIt = context->function_indices.find(parentName);
                    }
                    
                    if (calleeIt == context->function_indices.end()) {
                        std::string calleeFile = getFileName(referenced);
                        if (calleeFile.empty()) {
                            calleeFile = context->current_file;
                        }
                        
                        FunctionNode calleeNode(calleeName, calleeFile, getLineNumber(referenced));
                        
                        if (clang_Cursor_isNull(referenced) || clang_getCursorKind(referenced) == CXCursor_InvalidFile) {
                            calleeNode.isMissing = true;
                        }
                        
                        if (calleeFile.find("/usr/") == 0 || calleeFile.find("/lib/") == 0 || 
                            calleeFile.empty() || calleeFile.find("include/c++") != std::string::npos) {
                            calleeNode.isExternal = true;
                        }
                        
                        int calleeIndex = context->graph->addNode(calleeNode);
                        context->function_indices[calleeName] = calleeIndex;
                        calleeIt = context->function_indices.find(calleeName);
                    }
                    
                    context->graph->addEdge(callerIt->second, calleeIt->second);
                }
            }
        }
    }
    
    return CXChildVisit_Recurse;
}

static CXChildVisitResult extractFunctionsVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    ParserContext* context = static_cast<ParserContext*>(client_data);
    
    if (isFunctionImplementation(cursor)) {
        if (!clang_isCursorDefinition(cursor)) {
            return CXChildVisit_Continue;
        }
        
        std::string functionName = getCursorSpelling(cursor);
        std::string fileName = getFileName(cursor);
        
        if (functionName.empty() || fileName.empty()) {
            return CXChildVisit_Continue;
        }
        
        if (fileName != context->current_file) {
            return CXChildVisit_Continue;
        }
        
        if (context->processed_functions.find(functionName) != context->processed_functions.end()) {
            return CXChildVisit_Continue;
        }
        
        context->processed_functions.insert(functionName);
        
        FunctionNode node(functionName, fileName, getLineNumber(cursor));
        node.isStub = isStubFunction(cursor);
        
        int nodeIndex = context->graph->addNode(node);
        context->function_indices[functionName] = nodeIndex;
        
        clang_visitChildren(cursor, findFunctionCallsVisitor, client_data);
    }
    
    return CXChildVisit_Recurse;
}

CppParser::CppParser() {
}

std::unique_ptr<FunctionGraph> CppParser::parseFile(const std::string& file_path) {
    auto graph = std::make_unique<FunctionGraph>();
    
    if (!fs::exists(file_path)) {
        std::cerr << "File not found: " << file_path << std::endl;
        return graph;
    }
    
    CXIndex index = clang_createIndex(0, 0);
    
    const char* args[] = {
        "-std=c++17",
        "-I/usr/include",
        "-I/usr/local/include"
    };
    
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        file_path.c_str(),
        args, 3,
        nullptr, 0,
        CXTranslationUnit_None
    );
    
    if (unit == nullptr) {
        std::cerr << "Unable to parse translation unit: " << file_path << std::endl;
        clang_disposeIndex(index);
        return graph;
    }
    
    ParserContext context;
    context.graph = graph.get();
    context.current_file = fs::absolute(file_path).string();
    
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, extractFunctionsVisitor, &context);
    
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    
    return graph;
}

std::unique_ptr<FunctionGraph> CppParser::parseProject(const std::string& project_path) {
    auto graph = std::make_unique<FunctionGraph>();
    
    if (!fs::exists(project_path) || !fs::is_directory(project_path)) {
        std::cerr << "Project directory not found: " << project_path << std::endl;
        return graph;
    }
    
    ParserContext context;
    context.graph = graph.get();
    
    std::vector<std::string> sourceFiles = findSourceFiles(project_path);
    
    for (const auto& file : sourceFiles) {
        context.current_file = fs::absolute(file).string();
        context.processed_functions.clear();
        
        CXIndex index = clang_createIndex(0, 0);
        
        std::vector<std::string> includePaths;
        includePaths.push_back("-std=c++17");
        includePaths.push_back("-I" + project_path + "/include");
        includePaths.push_back("-I" + project_path);
        includePaths.push_back("-I/usr/include");
        includePaths.push_back("-I/usr/local/include");
        
        std::vector<const char*> args;
        for (const auto& path : includePaths) {
            args.push_back(path.c_str());
        }
        
        CXTranslationUnit unit = clang_parseTranslationUnit(
            index,
            file.c_str(),
            args.data(), args.size(),
            nullptr, 0,
            CXTranslationUnit_None
        );
        
        if (unit != nullptr) {
            CXCursor cursor = clang_getTranslationUnitCursor(unit);
            clang_visitChildren(cursor, extractFunctionsVisitor, &context);
            clang_disposeTranslationUnit(unit);
        }
        
        clang_disposeIndex(index);
    }
    
    return graph;
}

std::vector<std::string> CppParser::getSupportedExtensions() const {
    return {".cpp", ".cc", ".cxx", ".c++", ".h", ".hpp", ".hxx", ".h++"};
}

void CppParser::parseFileContent(const std::string& content, const std::string& file_path, FunctionGraph& graph) {
}

void CppParser::extractFunctions(const std::string& content, const std::string& file_path, FunctionGraph& graph) {
}

void CppParser::extractFunctionCalls(const std::string& content, FunctionGraph& graph) {
}

std::vector<std::string> CppParser::findSourceFiles(const std::string& directory_path) const {
    std::vector<std::string> sourceFiles;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory_path)) {
            if (entry.is_regular_file() && isSourceFile(entry.path().string())) {
                sourceFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning directory: " << e.what() << std::endl;
    }
    
    return sourceFiles;
}

bool CppParser::isSourceFile(const std::string& file_path) const {
    auto extensions = getSupportedExtensions();
    for (const auto& ext : extensions) {
        if (file_path.size() >= ext.size() && 
            file_path.substr(file_path.size() - ext.size()) == ext) {
            return true;
        }
    }
    return false;
}

std::string CppParser::readFileContent(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace codemap