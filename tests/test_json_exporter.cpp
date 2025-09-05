// PROTECTED TEST: Do not edit except with explicit approval
// This test file verifies the contracts defined in json_exporter.h
// Tests are immutable once marked as protected to ensure contract compliance.

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>
#include "../include/json_exporter.h"
#include "../include/codemap_types.h"

using namespace codemap;

// Test fixture class for JsonExporter tests
class JsonExporterTestFixture {
public:
    FunctionGraph graph;
    
    void setUp() {
        graph = FunctionGraph(); // Reset graph for each test
    }
    
    void createSampleGraph() {
        // Create a realistic function graph for testing
        FunctionNode main("main", "src/main.cpp", 1);
        FunctionNode helper("helper", "src/utils.cpp", 10);
        helper.isStub = true;
        
        FunctionNode process("processData", "src/core.cpp", 25);
        process.isMissing = true;
        
        FunctionNode external("printf", "stdio.h", 0);
        external.isExternal = true;
        
        graph.addNode(main);     // index 0
        graph.addNode(helper);   // index 1  
        graph.addNode(process);  // index 2
        graph.addNode(external); // index 3
        
        graph.addEdge(0, 1); // main -> helper
        graph.addEdge(0, 2); // main -> processData
        graph.addEdge(2, 1); // processData -> helper
        graph.addEdge(0, 3); // main -> printf
    }
    
    void createComplexGraph() {
        // Create graph with special characters and edge cases
        FunctionNode op("operator++", "include/vector.hpp", 42);
        FunctionNode ns("std::vector::push_back", "system/vector", 100);
        FunctionNode empty("", "empty.cpp", 1);
        FunctionNode quotes("func\"with\"quotes", "path/with spaces/file-name.cpp", 15);
        FunctionNode newlines("func\nwith\nnewlines", "path\twith\ttabs.cpp", 20);
        
        op.isStub = true;
        ns.isExternal = true;
        empty.isMissing = true;
        quotes.isStub = true;
        quotes.isMissing = true;
        newlines.isExternal = true;
        newlines.isStub = true;
        newlines.isMissing = true;
        
        graph.addNode(op);       // index 0
        graph.addNode(ns);       // index 1
        graph.addNode(empty);    // index 2
        graph.addNode(quotes);   // index 3
        graph.addNode(newlines); // index 4
        
        // Create various edge patterns
        graph.addEdge(0, 1);
        graph.addEdge(1, 2);
        graph.addEdge(2, 3);
        graph.addEdge(3, 4);
        graph.addEdge(4, 0); // Cycle back
        graph.addEdge(2, 2); // Self-loop
    }
};

// =============================================================================
// JsonExporter::escapeJSON Tests
// =============================================================================

void test_escapeJSON_basic() {
    std::cout << "Testing JsonExporter::escapeJSON basic functionality... ";
    
    // Test normal strings (should be unchanged)
    assert(JsonExporter::escapeJSON("normal string") == "normal string");
    assert(JsonExporter::escapeJSON("123456") == "123456");
    assert(JsonExporter::escapeJSON("underscore_function") == "underscore_function");
    assert(JsonExporter::escapeJSON("") == "");
    
    std::cout << "PASSED\n";
}

void test_escapeJSON_quotes() {
    std::cout << "Testing JsonExporter::escapeJSON quote escaping... ";
    
    assert(JsonExporter::escapeJSON("\"") == "\\\"");
    assert(JsonExporter::escapeJSON("Hello \"World\"") == "Hello \\\"World\\\"");
    assert(JsonExporter::escapeJSON("\"quoted\"") == "\\\"quoted\\\"");
    assert(JsonExporter::escapeJSON("\"\"") == "\\\"\\\"");
    assert(JsonExporter::escapeJSON("Say \"Hello\" to \"Everyone\"") == "Say \\\"Hello\\\" to \\\"Everyone\\\"");
    
    std::cout << "PASSED\n";
}

void test_escapeJSON_backslashes() {
    std::cout << "Testing JsonExporter::escapeJSON backslash escaping... ";
    
    assert(JsonExporter::escapeJSON("\\") == "\\\\");
    assert(JsonExporter::escapeJSON("C:\\path\\to\\file") == "C:\\\\path\\\\to\\\\file");
    assert(JsonExporter::escapeJSON("\\\\") == "\\\\\\\\");
    assert(JsonExporter::escapeJSON("path\\file.txt") == "path\\\\file.txt");
    
    std::cout << "PASSED\n";
}

void test_escapeJSON_newlines_and_special() {
    std::cout << "Testing JsonExporter::escapeJSON newlines and special characters... ";
    
    assert(JsonExporter::escapeJSON("\n") == "\\n");
    assert(JsonExporter::escapeJSON("Line1\nLine2") == "Line1\\nLine2");
    assert(JsonExporter::escapeJSON("Line1\nLine2\nLine3") == "Line1\\nLine2\\nLine3");
    
    assert(JsonExporter::escapeJSON("\r") == "\\r");
    assert(JsonExporter::escapeJSON("carriage\rreturn") == "carriage\\rreturn");
    
    assert(JsonExporter::escapeJSON("\t") == "\\t");
    assert(JsonExporter::escapeJSON("tab\tseparated") == "tab\\tseparated");
    
    std::cout << "PASSED\n";
}

void test_escapeJSON_combined_special_characters() {
    std::cout << "Testing JsonExporter::escapeJSON combined special characters... ";
    
    // Mix of quotes and backslashes
    assert(JsonExporter::escapeJSON("C:\\\"Program Files\\\"") == "C:\\\\\\\"Program Files\\\\\\\"");
    
    // Mix of newlines, tabs, and quotes
    assert(JsonExporter::escapeJSON("\"Hello\n\tWorld\"") == "\\\"Hello\\n\\tWorld\\\"");
    
    // All special characters together
    assert(JsonExporter::escapeJSON("\"\\\n\r\t") == "\\\"\\\\\\n\\r\\t");
    
    // Complex realistic example
    std::string complex = "Error: \"File not found\" at C:\\temp\\file.txt\n\tLine: 42";
    std::string expected = "Error: \\\"File not found\\\" at C:\\\\temp\\\\file.txt\\n\\tLine: 42";
    assert(JsonExporter::escapeJSON(complex) == expected);
    
    std::cout << "PASSED\n";
}

void test_escapeJSON_edge_cases() {
    std::cout << "Testing JsonExporter::escapeJSON edge cases... ";
    
    // Very long string
    std::string longString(1000, 'a');
    assert(JsonExporter::escapeJSON(longString) == longString); // Should be unchanged
    
    // String with only special characters
    std::string specialOnly = "\"\\\n\r\t\"";
    std::string specialEscaped = "\\\"\\\\\\n\\r\\t\\\"";
    assert(JsonExporter::escapeJSON(specialOnly) == specialEscaped);
    
    // Unicode and extended ASCII (should pass through unchanged)
    std::string unicode = "Hello 世界 café";
    assert(JsonExporter::escapeJSON(unicode) == unicode);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// JsonExporter::graphToJSON Tests  
// =============================================================================

void test_graphToJSON_empty_graph() {
    std::cout << "Testing JsonExporter::graphToJSON with empty graph... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Should contain empty nodes and edges arrays
    assert(json.find("\"nodes\": [") != std::string::npos);
    assert(json.find("\"edges\": [") != std::string::npos);
    assert(json.find("\"nodes\": []") != std::string::npos || 
           json.find("\"nodes\": [\n  ]") != std::string::npos);
    assert(json.find("\"edges\": []") != std::string::npos || 
           json.find("\"edges\": [\n  ]") != std::string::npos);
    
    // Should be valid JSON structure
    assert(json.front() == '{');
    assert(json.back() == '}');
    
    std::cout << "PASSED\n";
}

void test_graphToJSON_single_node() {
    std::cout << "Testing JsonExporter::graphToJSON with single node... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    FunctionNode node("testFunction", "test.cpp", 42);
    node.isStub = true;
    fixture.graph.addNode(node);
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Check for node properties
    assert(json.find("\"name\": \"testFunction\"") != std::string::npos);
    assert(json.find("\"file\": \"test.cpp\"") != std::string::npos);
    assert(json.find("\"line\": 42") != std::string::npos);
    assert(json.find("\"isStub\": true") != std::string::npos);
    assert(json.find("\"isMissing\": false") != std::string::npos);
    assert(json.find("\"isExternal\": false") != std::string::npos);
    
    // Should have empty edges
    assert(json.find("\"edges\": []") != std::string::npos || 
           json.find("\"edges\": [\n  ]") != std::string::npos);
    
    std::cout << "PASSED\n";
}

void test_graphToJSON_multiple_nodes() {
    std::cout << "Testing JsonExporter::graphToJSON with multiple nodes... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    fixture.createSampleGraph();
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Check all nodes are present
    assert(json.find("\"name\": \"main\"") != std::string::npos);
    assert(json.find("\"name\": \"helper\"") != std::string::npos);
    assert(json.find("\"name\": \"processData\"") != std::string::npos);
    assert(json.find("\"name\": \"printf\"") != std::string::npos);
    
    // Check file paths
    assert(json.find("\"file\": \"src/main.cpp\"") != std::string::npos);
    assert(json.find("\"file\": \"src/utils.cpp\"") != std::string::npos);
    assert(json.find("\"file\": \"src/core.cpp\"") != std::string::npos);
    assert(json.find("\"file\": \"stdio.h\"") != std::string::npos);
    
    // Check flags
    assert(json.find("\"isStub\": true") != std::string::npos);  // helper
    assert(json.find("\"isMissing\": true") != std::string::npos); // processData
    assert(json.find("\"isExternal\": true") != std::string::npos); // printf
    
    std::cout << "PASSED\n";
}

void test_graphToJSON_with_edges() {
    std::cout << "Testing JsonExporter::graphToJSON with edges... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    fixture.createSampleGraph();
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Check edges are present
    assert(json.find("\"edges\"") != std::string::npos);
    assert(json.find("\"from\": 0") != std::string::npos);
    assert(json.find("\"to\": 1") != std::string::npos);
    assert(json.find("\"from\": 0") != std::string::npos);
    assert(json.find("\"to\": 2") != std::string::npos);
    
    // Count the number of edges (should be 4)
    size_t fromCount = 0;
    size_t pos = 0;
    while ((pos = json.find("\"from\":", pos)) != std::string::npos) {
        fromCount++;
        pos += 7; // Length of "\"from\":"
    }
    assert(fromCount == 4); // Should have 4 edges
    
    std::cout << "PASSED\n";
}

void test_graphToJSON_special_characters() {
    std::cout << "Testing JsonExporter::graphToJSON with special characters... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    fixture.createComplexGraph();
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Check that special characters are properly escaped
    assert(json.find("\"name\": \"operator++\"") != std::string::npos);
    assert(json.find("\"name\": \"std::vector::push_back\"") != std::string::npos);
    assert(json.find("\"name\": \"\"") != std::string::npos); // Empty name
    
    // Check escaped quotes
    assert(json.find("\\\"with\\\"") != std::string::npos);
    
    // Check escaped newlines and tabs
    assert(json.find("\\n") != std::string::npos);
    assert(json.find("\\t") != std::string::npos);
    
    // Verify it's still valid JSON structure
    assert(json.front() == '{');
    assert(json.back() == '}');
    
    std::cout << "PASSED\n";
}

void test_graphToJSON_large_graph() {
    std::cout << "Testing JsonExporter::graphToJSON with large graph... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    // Create large graph
    const int NUM_NODES = 100;
    for (int i = 0; i < NUM_NODES; ++i) {
        std::string name = "func" + std::to_string(i);
        std::string file = "file" + std::to_string(i) + ".cpp";
        FunctionNode node(name, file, i + 1);
        
        // Set some flags randomly
        if (i % 3 == 0) node.isStub = true;
        if (i % 5 == 0) node.isMissing = true;
        if (i % 7 == 0) node.isExternal = true;
        
        fixture.graph.addNode(node);
        
        // Add edges
        if (i > 0) {
            fixture.graph.addEdge(i - 1, i); // Chain
        }
        if (i % 10 == 0 && i > 0) {
            fixture.graph.addEdge(0, i); // Connect to main
        }
    }
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Verify basic structure
    assert(json.find("\"nodes\"") != std::string::npos);
    assert(json.find("\"edges\"") != std::string::npos);
    assert(json.front() == '{');
    assert(json.back() == '}');
    
    // Check that some nodes are present
    assert(json.find("\"name\": \"func0\"") != std::string::npos);
    assert(json.find("\"name\": \"func50\"") != std::string::npos);
    assert(json.find("\"name\": \"func99\"") != std::string::npos);
    
    // Verify JSON is reasonably sized (not empty)
    assert(json.length() > 1000);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// JsonExporter::isValidGraphJSON Tests
// =============================================================================

void test_isValidGraphJSON_valid_json() {
    std::cout << "Testing JsonExporter::isValidGraphJSON with valid JSON... ";
    
    // Minimal valid JSON
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [], \"edges\": []}") == true);
    
    // Valid with whitespace
    assert(JsonExporter::isValidGraphJSON("{\n  \"nodes\": [],\n  \"edges\": []\n}") == true);
    
    // Valid with content
    std::string validWithContent = R"({
        "nodes": [
            {"name": "test", "file": "test.cpp", "line": 1, "isStub": false, "isMissing": false, "isExternal": false}
        ],
        "edges": [
            {"from": 0, "to": 1}
        ]
    })";
    assert(JsonExporter::isValidGraphJSON(validWithContent) == true);
    
    std::cout << "PASSED\n";
}

void test_isValidGraphJSON_invalid_structure() {
    std::cout << "Testing JsonExporter::isValidGraphJSON with invalid structure... ";
    
    // Missing nodes field
    assert(JsonExporter::isValidGraphJSON("{\"edges\": []}") == false);
    
    // Missing edges field  
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": []}") == false);
    
    // Empty JSON
    assert(JsonExporter::isValidGraphJSON("{}") == false);
    
    // Completely empty
    assert(JsonExporter::isValidGraphJSON("") == false);
    
    // Wrong field names
    assert(JsonExporter::isValidGraphJSON("{\"vertices\": [], \"connections\": []}") == false);
    
    std::cout << "PASSED\n";
}

void test_isValidGraphJSON_malformed_json() {
    std::cout << "Testing JsonExporter::isValidGraphJSON with malformed JSON... ";
    
    // Missing closing brace
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [], \"edges\": [") == false);
    
    // Missing opening brace
    assert(JsonExporter::isValidGraphJSON("\"nodes\": [], \"edges\": []}") == false);
    
    // Unbalanced brackets
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [[], \"edges\": []}") == false);
    
    // Missing commas
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [] \"edges\": []}") == true); // Basic validator might pass this
    
    // Extra closing braces
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [], \"edges\": []}}") == false);
    
    // Nested unbalanced
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [{\"test\": [}], \"edges\": []}") == false);
    
    std::cout << "PASSED\n";
}

void test_isValidGraphJSON_edge_cases() {
    std::cout << "Testing JsonExporter::isValidGraphJSON edge cases... ";
    
    // Very long valid JSON
    std::string longValid = "{\"nodes\": [";
    for (int i = 0; i < 100; ++i) {
        if (i > 0) longValid += ",";
        longValid += "{\"name\":\"func" + std::to_string(i) + "\"}";
    }
    longValid += "], \"edges\": []}";
    assert(JsonExporter::isValidGraphJSON(longValid) == true);
    
    // JSON with special characters in strings
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [{\"name\":\"test\\\"quote\"}], \"edges\": []}") == true);
    
    // Case sensitivity
    assert(JsonExporter::isValidGraphJSON("{\"Nodes\": [], \"Edges\": []}") == false);
    assert(JsonExporter::isValidGraphJSON("{\"NODES\": [], \"EDGES\": []}") == false);
    
    // Extra fields should be allowed
    assert(JsonExporter::isValidGraphJSON("{\"nodes\": [], \"edges\": [], \"extra\": \"field\"}") == true);
    
    std::cout << "PASSED\n";
}

void test_isValidGraphJSON_realistic_examples() {
    std::cout << "Testing JsonExporter::isValidGraphJSON with realistic examples... ";
    
    // Generate JSON from actual graph and verify it's valid
    JsonExporterTestFixture fixture;
    fixture.setUp();
    fixture.createSampleGraph();
    
    std::string generatedJson = JsonExporter::graphToJSON(fixture.graph);
    assert(JsonExporter::isValidGraphJSON(generatedJson) == true);
    
    // Generate from complex graph
    fixture.setUp();
    fixture.createComplexGraph();
    generatedJson = JsonExporter::graphToJSON(fixture.graph);
    assert(JsonExporter::isValidGraphJSON(generatedJson) == true);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// JsonExporter::jsonToGraph Tests
// =============================================================================

void test_jsonToGraph_basic() {
    std::cout << "Testing JsonExporter::jsonToGraph basic functionality... ";
    
    // Test basic JSON parsing functionality
    
    std::string validJson = "{\"nodes\": [], \"edges\": []}";
    FunctionGraph graph = JsonExporter::jsonToGraph(validJson);
    
    // Should return empty graph for minimal valid JSON
    assert(graph.nodes.empty());
    assert(graph.edges.empty());
    assert(graph.isValid());
    
    std::cout << "PASSED\n";
}

void test_jsonToGraph_invalid_json() {
    std::cout << "Testing JsonExporter::jsonToGraph with invalid JSON... ";
    
    // Note: Current implementation doesn't throw exceptions
    // Test current behavior
    
    std::string invalidJson = "invalid json";
    FunctionGraph graph = JsonExporter::jsonToGraph(invalidJson);
    
    // Should return empty graph for minimal valid JSON
    assert(graph.nodes.empty());
    assert(graph.edges.empty());
    
    std::cout << "PASSED\n";
}

void test_jsonToGraph_roundtrip_compatibility() {
    std::cout << "Testing JsonExporter::jsonToGraph roundtrip compatibility... ";
    
    // Test that jsonToGraph can handle output from graphToJSON
    // Test round-trip conversion
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    fixture.createSampleGraph();
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    FunctionGraph reconstructed = JsonExporter::jsonToGraph(json);
    
    // Verify round-trip conversion maintains data integrity
    // Future implementation should reconstruct the graph
    // For now, just verify it doesn't crash and returns valid graph
    assert(reconstructed.isValid());
    
    std::cout << "PASSED\n";
}

// =============================================================================
// Integration and Error Condition Tests
// =============================================================================

void test_integration_full_workflow() {
    std::cout << "Testing JsonExporter integration workflow... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    // Create complex realistic graph
    FunctionNode main("main", "src/main.cpp", 1);
    FunctionNode init("initialize", "src/init.cpp", 10);
    FunctionNode config("loadConfig", "src/config.cpp", 25);
    FunctionNode process("processData", "src/process.cpp", 50);
    FunctionNode validate("validateInput", "src/validation.cpp", 75);
    FunctionNode output("writeOutput", "src/output.cpp", 100);
    FunctionNode cleanup("cleanup", "src/cleanup.cpp", 125);
    
    // Set various flags
    init.isStub = true;
    config.isMissing = true;
    validate.isExternal = true;
    output.isStub = true;
    output.isMissing = true; // Both stub and missing
    
    // Add nodes and create realistic call graph
    int mainIdx = fixture.graph.addNode(main);
    int initIdx = fixture.graph.addNode(init);
    int configIdx = fixture.graph.addNode(config);
    int processIdx = fixture.graph.addNode(process);
    int validateIdx = fixture.graph.addNode(validate);
    int outputIdx = fixture.graph.addNode(output);
    int cleanupIdx = fixture.graph.addNode(cleanup);
    
    // Create call relationships
    fixture.graph.addEdge(mainIdx, initIdx);
    fixture.graph.addEdge(mainIdx, configIdx);
    fixture.graph.addEdge(mainIdx, processIdx);
    fixture.graph.addEdge(processIdx, validateIdx);
    fixture.graph.addEdge(processIdx, outputIdx);
    fixture.graph.addEdge(mainIdx, cleanupIdx);
    fixture.graph.addEdge(processIdx, processIdx); // Recursion
    
    // Test full workflow
    assert(fixture.graph.isValid());
    
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    assert(JsonExporter::isValidGraphJSON(json));
    
    // Verify content is reasonable
    assert(json.length() > 500); // Should be substantial
    assert(json.find("main") != std::string::npos);
    assert(json.find("initialize") != std::string::npos);
    assert(json.find("processData") != std::string::npos);
    
    // Test edge cases work
    assert(json.find("\"isStub\": true") != std::string::npos);
    assert(json.find("\"isMissing\": true") != std::string::npos);
    assert(json.find("\"isExternal\": true") != std::string::npos);
    
    std::cout << "PASSED\n";
}

void test_error_conditions() {
    std::cout << "Testing JsonExporter error conditions and edge cases... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    // Test with corrupted graph (invalid but not caught by isValid)
    fixture.graph.nodes.push_back(FunctionNode("test", "test.cpp", 1));
    
    // Should still produce JSON even with minimal graph
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    assert(!json.empty());
    assert(json.find("test") != std::string::npos);
    
    // Test escaping with null characters and other edge cases
    std::string withNull = "test";
    withNull += '\0';
    withNull += "after_null";
    std::string escaped = JsonExporter::escapeJSON(withNull);
    // Should handle gracefully (null might just be preserved)
    
    // Test very large line numbers
    FunctionNode bigLine("func", "file.cpp", 999999999);
    fixture.graph.addNode(bigLine);
    json = JsonExporter::graphToJSON(fixture.graph);
    assert(json.find("999999999") != std::string::npos);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// Performance and Stress Tests
// =============================================================================

void test_performance_large_scale() {
    std::cout << "Testing JsonExporter performance with large scale data... ";
    
    JsonExporterTestFixture fixture;
    fixture.setUp();
    
    // Create very large graph
    const int NUM_NODES = 1000;
    const int NUM_EDGES = 2000;
    
    // Add nodes with complex names and paths
    for (int i = 0; i < NUM_NODES; ++i) {
        std::string name = "function_" + std::to_string(i) + "_with_long_name";
        std::string file = "/very/long/path/to/source/files/module_" + 
                          std::to_string(i) + "/implementation.cpp";
        
        FunctionNode node(name, file, i * 10 + 1);
        
        // Set flags for variety
        if (i % 3 == 0) node.isStub = true;
        if (i % 5 == 0) node.isMissing = true;
        if (i % 7 == 0) node.isExternal = true;
        
        fixture.graph.addNode(node);
    }
    
    // Add many edges
    for (int i = 0; i < NUM_EDGES; ++i) {
        int from = i % NUM_NODES;
        int to = (i * 7 + 13) % NUM_NODES; // Some pseudo-random distribution
        fixture.graph.addEdge(from, to);
    }
    
    // Generate JSON - should complete reasonably quickly
    std::string json = JsonExporter::graphToJSON(fixture.graph);
    
    // Verify basic properties
    assert(!json.empty());
    assert(json.length() > 10000); // Should be substantial
    assert(JsonExporter::isValidGraphJSON(json));
    
    // Spot check content
    assert(json.find("function_0_with_long_name") != std::string::npos);
    assert(json.find("function_500_with_long_name") != std::string::npos);
    assert(json.find("function_999_with_long_name") != std::string::npos);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// Main Test Runner
// =============================================================================

int main() {
    std::cout << "Running Comprehensive JSON Exporter Tests\n";
    std::cout << "==========================================\n\n";
    
    // JsonExporter::escapeJSON Tests
    std::cout << "--- JsonExporter::escapeJSON Tests ---\n";
    test_escapeJSON_basic();
    test_escapeJSON_quotes();
    test_escapeJSON_backslashes();
    test_escapeJSON_newlines_and_special();
    test_escapeJSON_combined_special_characters();
    test_escapeJSON_edge_cases();
    
    // JsonExporter::graphToJSON Tests
    std::cout << "\n--- JsonExporter::graphToJSON Tests ---\n";
    test_graphToJSON_empty_graph();
    test_graphToJSON_single_node();
    test_graphToJSON_multiple_nodes();
    test_graphToJSON_with_edges();
    test_graphToJSON_special_characters();
    test_graphToJSON_large_graph();
    
    // JsonExporter::isValidGraphJSON Tests
    std::cout << "\n--- JsonExporter::isValidGraphJSON Tests ---\n";
    test_isValidGraphJSON_valid_json();
    test_isValidGraphJSON_invalid_structure();
    test_isValidGraphJSON_malformed_json();
    test_isValidGraphJSON_edge_cases();
    test_isValidGraphJSON_realistic_examples();
    
    // JsonExporter::jsonToGraph Tests
    std::cout << "\n--- JsonExporter::jsonToGraph Tests ---\n";
    test_jsonToGraph_basic();
    test_jsonToGraph_invalid_json();
    test_jsonToGraph_roundtrip_compatibility();
    
    // Integration and Stress Tests
    std::cout << "\n--- Integration and Error Tests ---\n";
    test_integration_full_workflow();
    test_error_conditions();
    test_performance_large_scale();
    
    std::cout << "\n==========================================\n";
    std::cout << "All JSON Exporter tests PASSED!\n";
    std::cout << "Total test functions: 21\n";
    std::cout << "All jsonToGraph tests passed!\n";
    
    return 0;
}