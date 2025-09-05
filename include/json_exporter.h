// PROTECTED CONTRACT: Do not edit except with explicit approval
// This file defines the JSON export/import interface for function graphs.
// Any modifications require explicit approval and must maintain backward compatibility.

#ifndef JSON_EXPORTER_H
#define JSON_EXPORTER_H

#include "codemap_types.h"
#include <string>
#include <memory>

namespace codemap {

/**
 * Handles JSON serialization and deserialization of FunctionGraph
 * Provides static methods for converting graphs to/from JSON format
 */
class JsonExporter {
public:
    /**
     * Convert FunctionGraph to JSON string representation
     * @param graph The function graph to serialize
     * @return JSON string representing the graph structure
     */
    static std::string graphToJSON(const FunctionGraph& graph);
    
    /**
     * Convert JSON string to FunctionGraph object
     * @param jsonString JSON string containing serialized graph data
     * @return FunctionGraph object created from JSON data
     * @throws std::runtime_error if JSON is invalid or malformed
     */
    static FunctionGraph jsonToGraph(const std::string& jsonString);
    
    /**
     * Validate that a JSON string contains valid graph data
     * @param jsonString JSON string to validate
     * @return True if the JSON contains a valid graph structure, false otherwise
     */
    static bool isValidGraphJSON(const std::string& jsonString);
    
    /**
     * Helper function to escape special characters in JSON strings
     * @param str String to escape for JSON format
     * @return Escaped string safe for use in JSON
     */
    static std::string escapeJSON(const std::string& str);

private:
    // Helper methods for JSON processing
    static std::string nodeToJSON(const FunctionNode& node);
    static FunctionNode jsonToNode(const std::string& nodeJson);
    static std::string edgeToJSON(const std::pair<int, int>& edge);
    static std::pair<int, int> jsonToEdge(const std::string& edgeJson);
    static bool validateNodeStructure(const std::string& nodeJson);
    static bool validateEdgeStructure(const std::string& edgeJson);
};

} // namespace codemap

#endif // JSON_EXPORTER_H