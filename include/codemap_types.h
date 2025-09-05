// PROTECTED CONTRACT: Do not edit except with explicit approval
// This file defines the core data structures for the CodeMap project.
// Any modifications require explicit approval and must maintain backward compatibility.

#ifndef CODEMAP_TYPES_H
#define CODEMAP_TYPES_H

#include <string>
#include <vector>
#include <utility>

namespace codemap {

/**
 * Represents a function node in the call graph
 * Contains all metadata about a function in the codebase
 */
struct FunctionNode {
    std::string name;        // Function name
    std::string file;        // File path where function is defined
    int line;               // Line number where function starts
    bool isStub;            // True if function contains TODO/NotImplemented/pass
    bool isMissing;         // True if function is called but not defined
    bool isExternal;        // True if function is from external library
    
    /**
     * Default constructor - initializes all fields to safe defaults
     */
    FunctionNode() : line(0), isStub(false), isMissing(false), isExternal(false) {}
    
    /**
     * Constructor with basic function information
     * @param n Function name
     * @param f File path where function is defined
     * @param l Line number where function starts
     */
    FunctionNode(const std::string& n, const std::string& f, int l)
        : name(n), file(f), line(l), 
          isStub(false), isMissing(false), isExternal(false) {}
};

/**
 * Represents the complete function call graph
 * Contains all functions and their call relationships
 */
struct FunctionGraph {
    std::vector<FunctionNode> nodes;           // All functions in the project
    std::vector<std::pair<int, int>> edges;    // Caller → Callee (node indices)
    
    /**
     * Find node index by function name
     * @param functionName Name of the function to find
     * @return Index of the function node, or -1 if not found
     */
    int findNodeIndex(const std::string& functionName) const;
    
    /**
     * Add a new node to the graph
     * @param node Function node to add
     * @return Index of the newly added node
     */
    int addNode(const FunctionNode& node);
    
    /**
     * Add an edge between two nodes (caller -> callee relationship)
     * @param callerIndex Index of the calling function
     * @param calleeIndex Index of the called function
     */
    void addEdge(int callerIndex, int calleeIndex);
    
    /**
     * Validate graph integrity
     * Checks that all edge indices are valid and within bounds
     * @return True if graph is valid, false otherwise
     */
    bool isValid() const;
};

} // namespace codemap

#endif // CODEMAP_TYPES_H