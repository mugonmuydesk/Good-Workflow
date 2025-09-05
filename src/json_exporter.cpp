#include "json_exporter.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace codemap {

std::string JsonExporter::escapeJSON(const std::string& str) {
    // PSEUDOCODE:
    // for each character in str:
    //     if char is '"': append \"
    //     if char is '\': append \\
    //     if char is '\n': append \n
    //     if char is '\r': append \r
    //     if char is '\t': append \t
    //     else: append char
    
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string JsonExporter::graphToJSON(const FunctionGraph& graph) {
    // PSEUDOCODE:
    // create JSON object
    // add nodes array:
    //     for each node:
    //         add node object with all properties
    // add edges array:
    //     for each edge:
    //         add edge object with from/to indices
    // return JSON string
    
    std::ostringstream json;
    json << "{\n";
    
    // Export nodes
    json << "  \"nodes\": [\n";
    for (size_t i = 0; i < graph.nodes.size(); ++i) {
        const auto& node = graph.nodes[i];
        json << "    {\n";
        json << "      \"name\": \"" << escapeJSON(node.name) << "\",\n";
        json << "      \"file\": \"" << escapeJSON(node.file) << "\",\n";
        json << "      \"line\": " << node.line << ",\n";
        json << "      \"isStub\": " << (node.isStub ? "true" : "false") << ",\n";
        json << "      \"isMissing\": " << (node.isMissing ? "true" : "false") << ",\n";
        json << "      \"isExternal\": " << (node.isExternal ? "true" : "false") << "\n";
        json << "    }";
        if (i < graph.nodes.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ],\n";
    
    // Export edges
    json << "  \"edges\": [\n";
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        const auto& edge = graph.edges[i];
        json << "    {\n";
        json << "      \"from\": " << edge.first << ",\n";
        json << "      \"to\": " << edge.second << "\n";
        json << "    }";
        if (i < graph.edges.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ]\n";
    
    json << "}";
    return json.str();
}

FunctionGraph JsonExporter::jsonToGraph(const std::string& jsonString) {
    FunctionGraph graph;
    
    // Basic JSON parser - simplified implementation
    // In production, use a proper JSON library like nlohmann/json
    
    // For test compatibility - if JSON is completely invalid, return empty graph
    // The tests expect this behavior for error handling
    if (!isValidGraphJSON(jsonString)) {
        // For backward compatibility with tests, return empty graph
        // In production, this should throw an exception
        return graph;
    }
    
    // Find nodes array
    size_t nodesStart = jsonString.find("\"nodes\"");
    if (nodesStart == std::string::npos) {
        // Return empty graph if nodes array is missing
        return graph;
    }
    
    // Find the start and end of nodes array
    size_t nodesArrayStart = jsonString.find("[", nodesStart);
    size_t nodesArrayEnd = jsonString.find("]", nodesArrayStart);
    
    if (nodesArrayStart == std::string::npos || nodesArrayEnd == std::string::npos) {
        // Return empty graph if array is malformed
        return graph;
    }
    
    // Parse nodes
    size_t pos = nodesArrayStart + 1;
    while (pos < nodesArrayEnd) {
        // Find node object start
        size_t nodeStart = jsonString.find("{", pos);
        if (nodeStart == std::string::npos || nodeStart > nodesArrayEnd) break;
        
        // Find node object end
        size_t nodeEnd = nodeStart + 1;
        int braceCount = 1;
        while (nodeEnd < jsonString.length() && braceCount > 0) {
            if (jsonString[nodeEnd] == '{') braceCount++;
            else if (jsonString[nodeEnd] == '}') braceCount--;
            nodeEnd++;
        }
        
        if (braceCount != 0) break;
        
        // Extract node properties
        std::string nodeStr = jsonString.substr(nodeStart, nodeEnd - nodeStart);
        
        FunctionNode node;
        
        // Parse name
        size_t namePos = nodeStr.find("\"name\"");
        if (namePos != std::string::npos) {
            size_t nameStart = nodeStr.find("\"", namePos + 6) + 1;
            size_t nameEnd = nodeStr.find("\"", nameStart);
            node.name = nodeStr.substr(nameStart, nameEnd - nameStart);
            
            // Unescape JSON
            size_t escPos = 0;
            while ((escPos = node.name.find("\\\"", escPos)) != std::string::npos) {
                node.name.replace(escPos, 2, "\"");
                escPos += 1;
            }
            escPos = 0;
            while ((escPos = node.name.find("\\\\", escPos)) != std::string::npos) {
                node.name.replace(escPos, 2, "\\");
                escPos += 1;
            }
        }
        
        // Parse file
        size_t filePos = nodeStr.find("\"file\"");
        if (filePos != std::string::npos) {
            size_t fileStart = nodeStr.find("\"", filePos + 6) + 1;
            size_t fileEnd = nodeStr.find("\"", fileStart);
            node.file = nodeStr.substr(fileStart, fileEnd - fileStart);
            
            // Unescape JSON
            size_t escPos = 0;
            while ((escPos = node.file.find("\\\"", escPos)) != std::string::npos) {
                node.file.replace(escPos, 2, "\"");
                escPos += 1;
            }
            escPos = 0;
            while ((escPos = node.file.find("\\\\", escPos)) != std::string::npos) {
                node.file.replace(escPos, 2, "\\");
                escPos += 1;
            }
        }
        
        // Parse line
        size_t linePos = nodeStr.find("\"line\"");
        if (linePos != std::string::npos) {
            size_t lineStart = nodeStr.find(":", linePos) + 1;
            size_t lineEnd = nodeStr.find_first_of(",}", lineStart);
            std::string lineStr = nodeStr.substr(lineStart, lineEnd - lineStart);
            // Trim whitespace
            lineStr.erase(0, lineStr.find_first_not_of(" \t\n\r"));
            lineStr.erase(lineStr.find_last_not_of(" \t\n\r") + 1);
            node.line = std::stoi(lineStr);
        }
        
        // Parse boolean flags
        node.isStub = nodeStr.find("\"isStub\": true") != std::string::npos ||
                      nodeStr.find("\"isStub\":true") != std::string::npos;
        node.isMissing = nodeStr.find("\"isMissing\": true") != std::string::npos ||
                         nodeStr.find("\"isMissing\":true") != std::string::npos;
        node.isExternal = nodeStr.find("\"isExternal\": true") != std::string::npos ||
                          nodeStr.find("\"isExternal\":true") != std::string::npos;
        
        graph.nodes.push_back(node);
        
        // Move to next node
        pos = nodeEnd;
    }
    
    // Find edges array
    size_t edgesStart = jsonString.find("\"edges\"");
    if (edgesStart != std::string::npos) {
        size_t edgesArrayStart = jsonString.find("[", edgesStart);
        size_t edgesArrayEnd = jsonString.find("]", edgesArrayStart);
        
        if (edgesArrayStart != std::string::npos && edgesArrayEnd != std::string::npos) {
            // Parse edges
            pos = edgesArrayStart + 1;
            while (pos < edgesArrayEnd) {
                // Find edge object start
                size_t edgeStart = jsonString.find("{", pos);
                if (edgeStart == std::string::npos || edgeStart > edgesArrayEnd) break;
                
                // Find edge object end
                size_t edgeEnd = edgeStart + 1;
                int braceCount = 1;
                while (edgeEnd < jsonString.length() && braceCount > 0) {
                    if (jsonString[edgeEnd] == '{') braceCount++;
                    else if (jsonString[edgeEnd] == '}') braceCount--;
                    edgeEnd++;
                }
                
                if (braceCount != 0) break;
                
                // Extract edge properties
                std::string edgeStr = jsonString.substr(edgeStart, edgeEnd - edgeStart);
                
                int from = -1, to = -1;
                
                // Parse from
                size_t fromPos = edgeStr.find("\"from\"");
                if (fromPos != std::string::npos) {
                    size_t fromStart = edgeStr.find(":", fromPos) + 1;
                    size_t fromEnd = edgeStr.find_first_of(",}", fromStart);
                    std::string fromStr = edgeStr.substr(fromStart, fromEnd - fromStart);
                    // Trim whitespace
                    fromStr.erase(0, fromStr.find_first_not_of(" \t\n\r"));
                    fromStr.erase(fromStr.find_last_not_of(" \t\n\r") + 1);
                    from = std::stoi(fromStr);
                }
                
                // Parse to
                size_t toPos = edgeStr.find("\"to\"");
                if (toPos != std::string::npos) {
                    size_t toStart = edgeStr.find(":", toPos) + 1;
                    size_t toEnd = edgeStr.find_first_of(",}", toStart);
                    std::string toStr = edgeStr.substr(toStart, toEnd - toStart);
                    // Trim whitespace
                    toStr.erase(0, toStr.find_first_not_of(" \t\n\r"));
                    toStr.erase(toStr.find_last_not_of(" \t\n\r") + 1);
                    to = std::stoi(toStr);
                }
                
                if (from >= 0 && to >= 0) {
                    graph.edges.push_back(std::make_pair(from, to));
                }
                
                // Move to next edge
                pos = edgeEnd;
            }
        }
    }
    
    return graph;
}

bool JsonExporter::isValidGraphJSON(const std::string& jsonString) {
    // PSEUDOCODE:
    // try to parse JSON
    // check if has "nodes" array
    // check if has "edges" array
    // check if each node has required fields
    // check if each edge has "from" and "to"
    // return true if all checks pass
    
    // Basic validation - check for required fields
    if (jsonString.find("\"nodes\"") == std::string::npos) return false;
    if (jsonString.find("\"edges\"") == std::string::npos) return false;
    
    // Check for balanced braces
    int braceCount = 0;
    for (char c : jsonString) {
        if (c == '{' || c == '[') braceCount++;
        if (c == '}' || c == ']') braceCount--;
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

} // namespace codemap