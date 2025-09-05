// PROTECTED TEST: Do not edit except with explicit approval
// This file contains comprehensive tests for the GraphBuilder class.
// Tests cover graph construction, JSON export/import, and file operations.

#include "graph_builder.h"
#include "json_exporter.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <set>

using namespace codemap;
namespace fs = std::filesystem;

// Test helper functions
void createTestFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
}

void createTestProject() {
    // Create a test project structure
    fs::create_directories("test_project/src");
    fs::create_directories("test_project/include");
    
    // Create header file with function declarations
    createTestFile("test_project/include/math.h", R"(
#ifndef MATH_H
#define MATH_H

int add(int a, int b);
int multiply(int a, int b);
double divide(double a, double b);

#endif
)");
    
    // Create implementation file with function definitions
    createTestFile("test_project/src/math.cpp", R"(
#include "../include/math.h"

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return add(a, 0) + add(b, 0);  // Calls add
}

double divide(double a, double b) {
    if (b == 0) return 0;
    return a / b;
}
)");
    
    // Create main file with function calls
    createTestFile("test_project/src/main.cpp", R"(
#include "../include/math.h"
#include <iostream>

void calculate() {
    int sum = add(5, 3);
    int product = multiply(4, 6);
    double quotient = divide(10.0, 2.0);
    std::cout << sum << " " << product << " " << quotient << std::endl;
}

int main() {
    calculate();
    return 0;
}
)");
}

void cleanupTestProject() {
    fs::remove_all("test_project");
    fs::remove("test_graph.json");
}

// Test 1: Constructor creates GraphBuilder with default parser
void test_default_constructor() {
    std::cout << "Test 1: Default constructor... ";
    GraphBuilder builder;
    if (builder.getParser() != nullptr) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Parser is null" << std::endl;
    }
}

// Test 2: Constructor with custom parser
void test_custom_parser_constructor() {
    std::cout << "Test 2: Custom parser constructor... ";
    auto parser = std::make_unique<CppParser>();
    IParser* parserPtr = parser.get();
    GraphBuilder builder(std::move(parser));
    if (builder.getParser() == parserPtr) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Parser not set correctly" << std::endl;
    }
}

// Test 3: Build graph from project directory
void test_build_from_project() {
    std::cout << "Test 3: Build from project... ";
    createTestProject();
    
    GraphBuilder builder;
    auto graph = builder.buildFromProject("test_project");
    
    if (graph && graph->nodes.size() >= 4) {  // Should have at least main, calculate, add, multiply, divide
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Graph not built correctly" << std::endl;
        if (graph) {
            std::cout << "  Found " << graph->nodes.size() << " nodes" << std::endl;
        }
    }
    
    cleanupTestProject();
}

// Test 4: Build from non-existent project
void test_build_from_invalid_project() {
    std::cout << "Test 4: Build from non-existent project... ";
    GraphBuilder builder;
    auto graph = builder.buildFromProject("non_existent_project");
    
    if (!graph || graph->nodes.empty()) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Should return null or empty graph for invalid path" << std::endl;
    }
}

// Test 5: Convert graph to JSON
void test_to_json() {
    std::cout << "Test 5: Convert to JSON... ";
    GraphBuilder builder;
    
    FunctionGraph graph;
    FunctionNode node1("func1", "file1.cpp", 10);
    FunctionNode node2("func2", "file2.cpp", 20);
    graph.nodes.push_back(node1);
    graph.nodes.push_back(node2);
    graph.edges.push_back({0, 1});
    
    std::string json = builder.toJSON(graph);
    
    if (!json.empty() && json.find("func1") != std::string::npos) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: JSON conversion failed" << std::endl;
    }
}

// Test 6: Load graph from JSON
void test_load_from_json() {
    std::cout << "Test 6: Load from JSON... ";
    GraphBuilder builder;
    
    std::string json = R"({
        "nodes": [
            {"name": "test_func", "file": "test.cpp", "line": 5, "isStub": false, "isMissing": false, "isExternal": false}
        ],
        "edges": []
    })";
    
    auto graph = builder.loadFromJSON(json);
    
    if (graph && graph->nodes.size() == 1 && graph->nodes[0].name == "test_func") {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Failed to load from JSON" << std::endl;
    }
}

// Test 7: Load from invalid JSON
void test_load_from_invalid_json() {
    std::cout << "Test 7: Load from invalid JSON... ";
    GraphBuilder builder;
    
    std::string invalidJson = "{ invalid json }";
    auto graph = builder.loadFromJSON(invalidJson);
    
    if (!graph) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Should return nullptr for invalid JSON" << std::endl;
    }
}

// Test 8: Save graph to file
void test_save_to_file() {
    std::cout << "Test 8: Save to file... ";
    GraphBuilder builder;
    
    FunctionGraph graph;
    FunctionNode node("save_test", "file.cpp", 1);
    graph.nodes.push_back(node);
    
    bool saved = builder.saveToFile(graph, "test_graph.json");
    
    if (saved && fs::exists("test_graph.json")) {
        std::cout << "PASS" << std::endl;
        fs::remove("test_graph.json");
    } else {
        std::cout << "FAIL: Failed to save to file" << std::endl;
    }
}

// Test 9: Load graph from file
void test_load_from_file() {
    std::cout << "Test 9: Load from file... ";
    GraphBuilder builder;
    
    // First create a file
    FunctionGraph originalGraph;
    FunctionNode node("load_test", "file.cpp", 2);
    originalGraph.nodes.push_back(node);
    builder.saveToFile(originalGraph, "test_graph.json");
    
    // Now load it back
    auto loadedGraph = builder.loadFromFile("test_graph.json");
    
    if (loadedGraph && loadedGraph->nodes.size() == 1 && 
        loadedGraph->nodes[0].name == "load_test") {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Failed to load from file" << std::endl;
    }
    
    fs::remove("test_graph.json");
}

// Test 10: Load from non-existent file
void test_load_from_nonexistent_file() {
    std::cout << "Test 10: Load from non-existent file... ";
    GraphBuilder builder;
    
    auto graph = builder.loadFromFile("nonexistent.json");
    
    if (!graph) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Should return nullptr for non-existent file" << std::endl;
    }
}

// Test 11: Set custom parser
void test_set_parser() {
    std::cout << "Test 11: Set custom parser... ";
    GraphBuilder builder;
    
    auto newParser = std::make_unique<CppParser>();
    IParser* parserPtr = newParser.get();
    builder.setParser(std::move(newParser));
    
    if (builder.getParser() == parserPtr) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Parser not set correctly" << std::endl;
    }
}

// Test 12: Round-trip JSON serialization
void test_json_round_trip() {
    std::cout << "Test 12: JSON round-trip... ";
    GraphBuilder builder;
    
    // Create a complex graph
    FunctionGraph originalGraph;
    FunctionNode node1("func1", "file1.cpp", 10);
    FunctionNode node2("func2", "file2.cpp", 20);
    node2.isStub = true;  // stub
    FunctionNode node3("func3", "file3.cpp", 30);
    node3.isMissing = true;  // missing
    originalGraph.nodes.push_back(node1);
    originalGraph.nodes.push_back(node2);
    originalGraph.nodes.push_back(node3);
    originalGraph.edges.push_back({0, 1});
    originalGraph.edges.push_back({1, 2});
    
    // Convert to JSON and back
    std::string json = builder.toJSON(originalGraph);
    auto loadedGraph = builder.loadFromJSON(json);
    
    if (loadedGraph && 
        loadedGraph->nodes.size() == originalGraph.nodes.size() &&
        loadedGraph->edges.size() == originalGraph.edges.size() &&
        loadedGraph->nodes[1].isStub == true &&
        loadedGraph->nodes[2].isMissing == true) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Round-trip failed" << std::endl;
    }
}

// Test 13: Build graph with missing functions
void test_missing_functions() {
    std::cout << "Test 13: Missing functions detection... ";
    
    // Create test project with missing function
    fs::create_directories("test_missing/src");
    createTestFile("test_missing/src/main.cpp", R"(
void existing_function() {
    missing_function();  // This function is not defined
}

int main() {
    existing_function();
    undefined_function();  // Another missing function
    return 0;
}
)");
    
    GraphBuilder builder;
    auto graph = builder.buildFromProject("test_missing");
    
    bool hasMissing = false;
    if (graph) {
        for (const auto& node : graph->nodes) {
            if (node.isMissing) {
                hasMissing = true;
                break;
            }
        }
    }
    
    if (hasMissing) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Missing functions not detected" << std::endl;
    }
    
    fs::remove_all("test_missing");
}

// Test 14: Build graph with stub functions
void test_stub_functions() {
    std::cout << "Test 14: Stub functions detection... ";
    
    // Create test project with stub function
    fs::create_directories("test_stub/src");
    createTestFile("test_stub/src/stubs.cpp", R"(
int implemented_function() {
    return 42;
}

void stub_function() {
    // TODO: implement this
}

int not_implemented_yet() {
    // NotImplemented
    return 0;
}
)");
    
    GraphBuilder builder;
    auto graph = builder.buildFromProject("test_stub");
    
    int stubCount = 0;
    if (graph) {
        for (const auto& node : graph->nodes) {
            if (node.isStub) {
                stubCount++;
            }
        }
    }
    
    if (stubCount >= 1) {  // Should detect at least one stub
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Stub functions not detected" << std::endl;
    }
    
    fs::remove_all("test_stub");
}

// Test 15: Complex project with multiple files and dependencies
void test_complex_project() {
    std::cout << "Test 15: Complex project analysis... ";
    
    // Create a more complex test project
    fs::create_directories("test_complex/src");
    fs::create_directories("test_complex/include");
    fs::create_directories("test_complex/lib");
    
    createTestFile("test_complex/include/utils.h", R"(
#ifndef UTILS_H
#define UTILS_H
void print_message(const char* msg);
int calculate(int x, int y);
#endif
)");
    
    createTestFile("test_complex/src/utils.cpp", R"(
#include "../include/utils.h"
#include <iostream>

void print_message(const char* msg) {
    std::cout << msg << std::endl;
}

int calculate(int x, int y) {
    return x + y;
}
)");
    
    createTestFile("test_complex/src/app.cpp", R"(
#include "../include/utils.h"

void process_data() {
    print_message("Processing...");
    int result = calculate(10, 20);
    print_message("Done");
}

void run_app() {
    process_data();
    print_message("App finished");
}
)");
    
    createTestFile("test_complex/src/main.cpp", R"(
void run_app();

int main() {
    run_app();
    return 0;
}
)");
    
    GraphBuilder builder;
    auto graph = builder.buildFromProject("test_complex");
    
    bool hasEdges = false;
    bool hasMultipleFiles = false;
    
    if (graph) {
        hasEdges = !graph->edges.empty();
        
        // Check if nodes come from different files
        std::set<std::string> files;
        for (const auto& node : graph->nodes) {
            if (!node.file.empty()) {
                files.insert(node.file);
            }
        }
        hasMultipleFiles = files.size() > 1;
    }
    
    if (hasEdges && hasMultipleFiles) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL: Complex project not analyzed correctly" << std::endl;
    }
    
    fs::remove_all("test_complex");
}

int main() {
    std::cout << "=== GraphBuilder Test Suite ===" << std::endl;
    
    test_default_constructor();
    test_custom_parser_constructor();
    test_build_from_project();
    test_build_from_invalid_project();
    test_to_json();
    test_load_from_json();
    test_load_from_invalid_json();
    test_save_to_file();
    test_load_from_file();
    test_load_from_nonexistent_file();
    test_set_parser();
    test_json_round_trip();
    test_missing_functions();
    test_stub_functions();
    test_complex_project();
    
    std::cout << "=== Test Suite Complete ===" << std::endl;
    
    // Clean up any remaining test files
    cleanupTestProject();
    
    return 0;
}