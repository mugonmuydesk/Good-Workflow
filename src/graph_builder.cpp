#include "graph_builder.h"
#include "json_exporter.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <cctype>

namespace codemap {

GraphBuilder::GraphBuilder() : parser_(std::make_unique<CppParser>()) {}

GraphBuilder::GraphBuilder(std::unique_ptr<IParser> parser) 
    : parser_(std::move(parser)) {}

std::unique_ptr<FunctionGraph> GraphBuilder::buildFromProject(const std::string& project_path) {
    if (!parser_) {
        return nullptr;
    }
    
    // Use the parser to parse the entire project
    auto graph = parser_->parseProject(project_path);
    if (!graph) {
        return nullptr;
    }
    
    // Post-processing: resolve relationships and mark special functions
    resolveCallRelationships(*graph);
    markMissingFunctions(*graph);
    markStubFunctions(*graph);
    
    return graph;
}

std::string GraphBuilder::toJSON(const FunctionGraph& graph) const {
    return JsonExporter::graphToJSON(graph);
}

std::unique_ptr<FunctionGraph> GraphBuilder::loadFromJSON(const std::string& json_string) const {
    // JsonExporter::jsonToGraph returns empty graph for invalid JSON
    // Check if JSON is valid first
    if (!JsonExporter::isValidGraphJSON(json_string)) {
        return nullptr;
    }
    
    try {
        auto graph = std::make_unique<FunctionGraph>(JsonExporter::jsonToGraph(json_string));
        return graph;
    } catch (const std::exception& e) {
        // Return nullptr if JSON parsing fails
        return nullptr;
    }
}

bool GraphBuilder::saveToFile(const FunctionGraph& graph, const std::string& file_path) const {
    try {
        std::string json = toJSON(graph);
        return writeFileContent(file_path, json);
    } catch (const std::exception& e) {
        return false;
    }
}

std::unique_ptr<FunctionGraph> GraphBuilder::loadFromFile(const std::string& file_path) const {
    try {
        std::string content = readFileContent(file_path);
        if (content.empty()) {
            return nullptr;
        }
        return loadFromJSON(content);
    } catch (const std::exception& e) {
        return nullptr;
    }
}

void GraphBuilder::setParser(std::unique_ptr<IParser> parser) {
    parser_ = std::move(parser);
}

IParser* GraphBuilder::getParser() const {
    return parser_.get();
}

void GraphBuilder::resolveCallRelationships(FunctionGraph& graph) const {
    // The parser already creates edges between nodes
    // This method ensures edges are valid and consistent
    
    // Validate that all edge indices are within bounds
    int nodeCount = static_cast<int>(graph.nodes.size());
    auto it = graph.edges.begin();
    while (it != graph.edges.end()) {
        if (it->first >= nodeCount || it->second >= nodeCount ||
            it->first < 0 || it->second < 0) {
            // Remove invalid edge
            it = graph.edges.erase(it);
        } else {
            ++it;
        }
    }
}

void GraphBuilder::markMissingFunctions(FunctionGraph& graph) const {
    // GraphBuilder is responsible for resolving missing functions
    // This is a key part of the contract - we analyze the graph to find
    // functions that are called but not defined
    
    // First, collect all defined function names
    std::unordered_set<std::string> definedFunctions;
    std::unordered_map<std::string, int> nameToIndex;
    
    for (int i = 0; i < static_cast<int>(graph.nodes.size()); ++i) {
        const auto& node = graph.nodes[i];
        nameToIndex[node.name] = i;
        
        // A function is defined if it has a valid file and line
        if (!node.file.empty() && node.line >= 0) {
            definedFunctions.insert(node.name);
        }
    }
    
    // Now look through edges to find calls to undefined functions
    std::unordered_set<std::string> calledFunctions;
    for (const auto& edge : graph.edges) {
        if (edge.second < static_cast<int>(graph.nodes.size())) {
            calledFunctions.insert(graph.nodes[edge.second].name);
        }
    }
    
    // Find missing functions (called but not defined)
    std::unordered_set<std::string> missingFunctions;
    for (const auto& calledFunc : calledFunctions) {
        if (definedFunctions.find(calledFunc) == definedFunctions.end()) {
            missingFunctions.insert(calledFunc);
        }
    }
    
    // Mark existing nodes as missing if they're undefined
    for (auto& node : graph.nodes) {
        if (node.file.empty() || node.line < 0) {
            node.isMissing = true;
            // Assume missing functions are external unless we know otherwise
            if (!node.isStub) {
                node.isExternal = true;
            }
        }
    }
    
    // Add new nodes for any missing functions we discovered
    for (const auto& missingFunc : missingFunctions) {
        // Check if node already exists
        if (nameToIndex.find(missingFunc) == nameToIndex.end()) {
            FunctionNode missingNode;
            missingNode.name = missingFunc;
            missingNode.file = "";
            missingNode.line = -1;
            missingNode.isMissing = true;
            missingNode.isExternal = true;
            missingNode.isStub = false;
            
            graph.nodes.push_back(missingNode);
            nameToIndex[missingFunc] = graph.nodes.size() - 1;
        }
    }
}

void GraphBuilder::markStubFunctions(FunctionGraph& graph) const {
    for (auto& node : graph.nodes) {
        if (isStubFunction(node)) {
            node.isStub = true;
        }
    }
}

bool GraphBuilder::isStubFunction(const FunctionNode& node) const {
    // A function is considered a stub if:
    // 1. It's already marked as stub by the parser
    // 2. It has no implementation (missing body)
    // 3. Contains TODO, FIXME, or NotImplemented markers
    
    // First check if already marked
    if (node.isStub) {
        return true;
    }
    
    // Additional heuristics can be added here
    // For example, check if function name contains "stub", "todo", "dummy"
    std::string lowerName = node.name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    if (lowerName.find("stub") != std::string::npos ||
        lowerName.find("todo") != std::string::npos ||
        lowerName.find("dummy") != std::string::npos ||
        lowerName.find("not_implemented") != std::string::npos ||
        lowerName.find("notimplemented") != std::string::npos) {
        return true;
    }
    
    return false;
}

std::string GraphBuilder::readFileContent(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool GraphBuilder::writeFileContent(const std::string& file_path, const std::string& content) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

} // namespace codemap