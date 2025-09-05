// PROTECTED CONTRACT: Do not edit except with explicit approval
// This file defines the graph builder interface for constructing function call graphs.
// Any modifications require explicit approval and must maintain backward compatibility.

#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include "codemap_types.h"
#include "parser.h"
#include <string>
#include <memory>

namespace codemap {

/**
 * Builds function graphs from source code projects
 * Orchestrates the parsing process and constructs the complete function call graph
 */
class GraphBuilder {
private:
    std::unique_ptr<IParser> parser_;

public:
    /**
     * Constructor - creates a GraphBuilder with default C++ parser
     */
    GraphBuilder();

    /**
     * Constructor with custom parser
     * @param parser Unique pointer to a parser implementation
     */
    explicit GraphBuilder(std::unique_ptr<IParser> parser);

    /**
     * Destructor
     */
    ~GraphBuilder() = default;

    /**
     * Build a function graph from a project directory
     * This is the main entry point for graph building
     * @param project_path Path to the project directory
     * @return Unique pointer to FunctionGraph containing all functions and call relationships
     */
    std::unique_ptr<FunctionGraph> buildFromProject(const std::string& project_path);

    /**
     * Convert a FunctionGraph to JSON string representation
     * @param graph The function graph to serialize
     * @return JSON string representation of the graph
     */
    std::string toJSON(const FunctionGraph& graph) const;

    /**
     * Load a FunctionGraph from JSON string representation
     * @param json_string JSON string containing serialized graph data
     * @return Unique pointer to FunctionGraph created from JSON, or nullptr if invalid
     */
    std::unique_ptr<FunctionGraph> loadFromJSON(const std::string& json_string) const;

    /**
     * Save a function graph to a JSON file
     * @param graph The function graph to save
     * @param file_path Path where to save the JSON file
     * @return True if successful, false on error
     */
    bool saveToFile(const FunctionGraph& graph, const std::string& file_path) const;

    /**
     * Load a function graph from a JSON file
     * @param file_path Path to the JSON file to load
     * @return Unique pointer to FunctionGraph loaded from file, or nullptr if error
     */
    std::unique_ptr<FunctionGraph> loadFromFile(const std::string& file_path) const;

    /**
     * Set a custom parser for the graph builder
     * @param parser Unique pointer to a parser implementation
     */
    void setParser(std::unique_ptr<IParser> parser);

    /**
     * Get the currently used parser
     * @return Pointer to the current parser (ownership remains with GraphBuilder)
     */
    IParser* getParser() const;

private:
    // Helper methods
    void resolveCallRelationships(FunctionGraph& graph) const;
    void markMissingFunctions(FunctionGraph& graph) const;
    void markStubFunctions(FunctionGraph& graph) const;
    bool isStubFunction(const FunctionNode& node) const;
    std::string readFileContent(const std::string& file_path) const;
    bool writeFileContent(const std::string& file_path, const std::string& content) const;
};

} // namespace codemap

#endif // GRAPH_BUILDER_H