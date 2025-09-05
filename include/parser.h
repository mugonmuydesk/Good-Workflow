// PROTECTED CONTRACT: Do not edit except with explicit approval
// This file defines the parser interface for extracting function information.
// Any modifications require explicit approval and must maintain backward compatibility.

#ifndef PARSER_H
#define PARSER_H

#include "codemap_types.h"
#include <string>
#include <vector>
#include <memory>

namespace codemap {

/**
 * Interface for language parsers
 * Defines the contract that all language parsers must implement
 */
class IParser {
public:
    virtual ~IParser() = default;

    /**
     * Parse a single file and extract function information
     * @param file_path Path to the file to parse
     * @return Unique pointer to FunctionGraph containing functions from the file
     */
    virtual std::unique_ptr<FunctionGraph> parseFile(const std::string& file_path) = 0;

    /**
     * Parse an entire project directory and extract function information
     * @param project_path Path to the project directory
     * @return Unique pointer to FunctionGraph containing all functions from the project
     */
    virtual std::unique_ptr<FunctionGraph> parseProject(const std::string& project_path) = 0;

    /**
     * Get list of file extensions supported by this parser
     * @return Vector of supported file extensions (e.g., {".cpp", ".h", ".hpp"})
     */
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
};

/**
 * C++ language parser implementation
 * Implements IParser interface for parsing C++ source files
 */
class CppParser : public IParser {
public:
    /**
     * Constructor - initializes the C++ parser
     */
    CppParser();

    /**
     * Destructor
     */
    virtual ~CppParser() = default;

    /**
     * Parse a C++ file and extract function information
     * @param file_path Path to the C++ file to parse
     * @return Unique pointer to FunctionGraph containing functions from the file
     */
    std::unique_ptr<FunctionGraph> parseFile(const std::string& file_path) override;

    /**
     * Parse an entire C++ project directory and extract function information
     * Recursively scans directory for C++ files and parses all found files
     * @param project_path Path to the project directory
     * @return Unique pointer to FunctionGraph containing all functions from the project
     */
    std::unique_ptr<FunctionGraph> parseProject(const std::string& project_path) override;

    /**
     * Get list of file extensions supported by the C++ parser
     * @return Vector of C++ file extensions
     */
    std::vector<std::string> getSupportedExtensions() const override;

private:
    // Helper methods for parsing implementation
    void parseFileContent(const std::string& content, const std::string& file_path, FunctionGraph& graph);
    void extractFunctions(const std::string& content, const std::string& file_path, FunctionGraph& graph);
    void extractFunctionCalls(const std::string& content, FunctionGraph& graph);
    std::vector<std::string> findSourceFiles(const std::string& directory_path) const;
    bool isSourceFile(const std::string& file_path) const;
    std::string readFileContent(const std::string& file_path) const;
};

} // namespace codemap

#endif // PARSER_H