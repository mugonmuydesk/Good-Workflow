// PROTECTED TEST: Do not edit except with explicit approval
// This test file verifies the contracts defined in codemap_types.h
// Tests are immutable once marked as protected to ensure contract compliance.

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../include/codemap_types.h"

using namespace codemap;

// Test fixture class for FunctionGraph tests
class FunctionGraphTestFixture {
public:
    FunctionGraph graph;
    
    void setUp() {
        graph = FunctionGraph(); // Reset graph for each test
    }
    
    void addSampleNodes() {
        graph.nodes.push_back(FunctionNode("main", "main.cpp", 1));
        graph.nodes.push_back(FunctionNode("helper", "utils.cpp", 10));
        graph.nodes.push_back(FunctionNode("process", "core.cpp", 25));
        graph.nodes.push_back(FunctionNode("cleanup", "cleanup.cpp", 5));
    }
};

// =============================================================================
// FunctionNode Tests
// =============================================================================

void test_FunctionNode_default_constructor() {
    std::cout << "Testing FunctionNode default constructor... ";
    
    FunctionNode node;
    assert(node.name.empty());
    assert(node.file.empty());
    assert(node.line == 0);
    assert(node.isStub == false);
    assert(node.isMissing == false);
    assert(node.isExternal == false);
    
    std::cout << "PASSED\n";
}

void test_FunctionNode_parametric_constructor() {
    std::cout << "Testing FunctionNode parametric constructor... ";
    
    FunctionNode node("testFunction", "/path/to/file.cpp", 42);
    assert(node.name == "testFunction");
    assert(node.file == "/path/to/file.cpp");
    assert(node.line == 42);
    assert(node.isStub == false);
    assert(node.isMissing == false);
    assert(node.isExternal == false);
    
    std::cout << "PASSED\n";
}

void test_FunctionNode_edge_cases() {
    std::cout << "Testing FunctionNode edge cases... ";
    
    // Empty strings
    FunctionNode node1("", "", 0);
    assert(node1.name.empty());
    assert(node1.file.empty());
    assert(node1.line == 0);
    
    // Negative line number
    FunctionNode node2("func", "file.cpp", -5);
    assert(node2.line == -5); // Should accept negative values
    
    // Very long strings
    std::string longName(1000, 'a');
    std::string longPath(2000, 'b');
    FunctionNode node3(longName, longPath, 999999);
    assert(node3.name == longName);
    assert(node3.file == longPath);
    assert(node3.line == 999999);
    
    // Special characters in names and paths
    FunctionNode node4("operator++", "path with spaces/file-name.cpp", 10);
    assert(node4.name == "operator++");
    assert(node4.file == "path with spaces/file-name.cpp");
    
    std::cout << "PASSED\n";
}

void test_FunctionNode_flag_modification() {
    std::cout << "Testing FunctionNode flag modification... ";
    
    FunctionNode node("test", "test.cpp", 1);
    
    // Test isStub flag
    node.isStub = true;
    assert(node.isStub == true);
    node.isStub = false;
    assert(node.isStub == false);
    
    // Test isMissing flag
    node.isMissing = true;
    assert(node.isMissing == true);
    node.isMissing = false;
    assert(node.isMissing == false);
    
    // Test isExternal flag
    node.isExternal = true;
    assert(node.isExternal == true);
    node.isExternal = false;
    assert(node.isExternal == false);
    
    // Test multiple flags simultaneously
    node.isStub = true;
    node.isMissing = true;
    node.isExternal = true;
    assert(node.isStub == true);
    assert(node.isMissing == true);
    assert(node.isExternal == true);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// FunctionGraph::findNodeIndex Tests
// =============================================================================

void test_FunctionGraph_findNodeIndex_basic() {
    std::cout << "Testing FunctionGraph::findNodeIndex basic functionality... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes();
    
    assert(fixture.graph.findNodeIndex("main") == 0);
    assert(fixture.graph.findNodeIndex("helper") == 1);
    assert(fixture.graph.findNodeIndex("process") == 2);
    assert(fixture.graph.findNodeIndex("cleanup") == 3);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_findNodeIndex_not_found() {
    std::cout << "Testing FunctionGraph::findNodeIndex not found cases... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes();
    
    assert(fixture.graph.findNodeIndex("nonexistent") == -1);
    assert(fixture.graph.findNodeIndex("") == -1);
    assert(fixture.graph.findNodeIndex("Main") == -1); // Case sensitive
    assert(fixture.graph.findNodeIndex("main ") == -1); // Trailing space
    assert(fixture.graph.findNodeIndex(" main") == -1); // Leading space
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_findNodeIndex_empty_graph() {
    std::cout << "Testing FunctionGraph::findNodeIndex on empty graph... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    assert(fixture.graph.findNodeIndex("anything") == -1);
    assert(fixture.graph.findNodeIndex("") == -1);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_findNodeIndex_duplicates() {
    std::cout << "Testing FunctionGraph::findNodeIndex with duplicate names... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Add nodes with same name but different files
    fixture.graph.nodes.push_back(FunctionNode("duplicate", "file1.cpp", 1));
    fixture.graph.nodes.push_back(FunctionNode("duplicate", "file2.cpp", 2));
    fixture.graph.nodes.push_back(FunctionNode("unique", "file3.cpp", 3));
    
    // Should return the first occurrence
    assert(fixture.graph.findNodeIndex("duplicate") == 0);
    assert(fixture.graph.findNodeIndex("unique") == 2);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_findNodeIndex_special_characters() {
    std::cout << "Testing FunctionGraph::findNodeIndex with special characters... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    fixture.graph.nodes.push_back(FunctionNode("operator++", "test.cpp", 1));
    fixture.graph.nodes.push_back(FunctionNode("std::vector::push_back", "test.cpp", 2));
    fixture.graph.nodes.push_back(FunctionNode("function_with_underscores", "test.cpp", 3));
    fixture.graph.nodes.push_back(FunctionNode("123numericStart", "test.cpp", 4));
    fixture.graph.nodes.push_back(FunctionNode("", "test.cpp", 5)); // Empty name
    
    assert(fixture.graph.findNodeIndex("operator++") == 0);
    assert(fixture.graph.findNodeIndex("std::vector::push_back") == 1);
    assert(fixture.graph.findNodeIndex("function_with_underscores") == 2);
    assert(fixture.graph.findNodeIndex("123numericStart") == 3);
    assert(fixture.graph.findNodeIndex("") == 4);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// FunctionGraph::addNode Tests
// =============================================================================

void test_FunctionGraph_addNode_basic() {
    std::cout << "Testing FunctionGraph::addNode basic functionality... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    assert(fixture.graph.nodes.size() == 0);
    
    int idx1 = fixture.graph.addNode(FunctionNode("func1", "file1.cpp", 10));
    assert(idx1 == 0);
    assert(fixture.graph.nodes.size() == 1);
    assert(fixture.graph.nodes[0].name == "func1");
    
    int idx2 = fixture.graph.addNode(FunctionNode("func2", "file2.cpp", 20));
    assert(idx2 == 1);
    assert(fixture.graph.nodes.size() == 2);
    assert(fixture.graph.nodes[1].name == "func2");
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addNode_duplicate_detection() {
    std::cout << "Testing FunctionGraph::addNode duplicate detection... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    FunctionNode node1("func1", "file1.cpp", 10);
    FunctionNode node1_duplicate("func1", "file2.cpp", 20); // Different file, same name
    FunctionNode node2("func2", "file1.cpp", 30);
    
    int idx1 = fixture.graph.addNode(node1);
    assert(idx1 == 0);
    assert(fixture.graph.nodes.size() == 1);
    
    // Adding duplicate should return existing index, not add new node
    int idx1_dup = fixture.graph.addNode(node1_duplicate);
    assert(idx1_dup == 0); // Same index as original
    assert(fixture.graph.nodes.size() == 1); // Size unchanged
    assert(fixture.graph.nodes[0].file == "file1.cpp"); // Original data unchanged
    
    int idx2 = fixture.graph.addNode(node2);
    assert(idx2 == 1);
    assert(fixture.graph.nodes.size() == 2);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addNode_edge_cases() {
    std::cout << "Testing FunctionGraph::addNode edge cases... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Add node with empty name
    int idx1 = fixture.graph.addNode(FunctionNode("", "file.cpp", 1));
    assert(idx1 == 0);
    assert(fixture.graph.nodes.size() == 1);
    
    // Add another node with empty name (should be duplicate)
    int idx2 = fixture.graph.addNode(FunctionNode("", "other.cpp", 2));
    assert(idx2 == 0); // Same index
    assert(fixture.graph.nodes.size() == 1); // No new node added
    
    // Add node with all flags set
    FunctionNode flaggedNode("flagged", "file.cpp", 10);
    flaggedNode.isStub = true;
    flaggedNode.isMissing = true;
    flaggedNode.isExternal = true;
    
    int idx3 = fixture.graph.addNode(flaggedNode);
    assert(idx3 == 1);
    assert(fixture.graph.nodes[1].isStub == true);
    assert(fixture.graph.nodes[1].isMissing == true);
    assert(fixture.graph.nodes[1].isExternal == true);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addNode_large_scale() {
    std::cout << "Testing FunctionGraph::addNode large scale... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Add many nodes
    const int NUM_NODES = 1000;
    std::vector<int> indices;
    
    for (int i = 0; i < NUM_NODES; ++i) {
        std::string name = "func" + std::to_string(i);
        std::string file = "file" + std::to_string(i) + ".cpp";
        int idx = fixture.graph.addNode(FunctionNode(name, file, i + 1));
        indices.push_back(idx);
        assert(idx == i);
    }
    
    assert(fixture.graph.nodes.size() == NUM_NODES);
    
    // Verify all nodes are accessible
    for (int i = 0; i < NUM_NODES; ++i) {
        std::string expectedName = "func" + std::to_string(i);
        assert(fixture.graph.findNodeIndex(expectedName) == i);
    }
    
    // Try to add duplicates - should not increase size
    for (int i = 0; i < 10; ++i) {
        std::string name = "func" + std::to_string(i);
        int idx = fixture.graph.addNode(FunctionNode(name, "different.cpp", 999));
        assert(idx == i); // Should return original index
    }
    assert(fixture.graph.nodes.size() == NUM_NODES); // Size unchanged
    
    std::cout << "PASSED\n";
}

// =============================================================================
// FunctionGraph::addEdge Tests
// =============================================================================

void test_FunctionGraph_addEdge_basic() {
    std::cout << "Testing FunctionGraph::addEdge basic functionality... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes(); // 4 nodes: indices 0, 1, 2, 3
    
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(0, 1); // main -> helper
    assert(fixture.graph.edges.size() == 1);
    assert(fixture.graph.edges[0].first == 0);
    assert(fixture.graph.edges[0].second == 1);
    
    fixture.graph.addEdge(1, 2); // helper -> process
    assert(fixture.graph.edges.size() == 2);
    assert(fixture.graph.edges[1].first == 1);
    assert(fixture.graph.edges[1].second == 2);
    
    fixture.graph.addEdge(0, 2); // main -> process
    assert(fixture.graph.edges.size() == 3);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addEdge_duplicate_prevention() {
    std::cout << "Testing FunctionGraph::addEdge duplicate prevention... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes();
    
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.edges.size() == 1);
    
    // Try to add the same edge again
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.edges.size() == 1); // Should not increase
    
    // Add different edge
    fixture.graph.addEdge(1, 0); // Reverse direction - should be allowed
    assert(fixture.graph.edges.size() == 2);
    
    // Try to add reverse again
    fixture.graph.addEdge(1, 0);
    assert(fixture.graph.edges.size() == 2); // Should not increase
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addEdge_invalid_indices() {
    std::cout << "Testing FunctionGraph::addEdge with invalid indices... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes(); // 4 nodes: indices 0-3
    
    // Test negative indices
    fixture.graph.addEdge(-1, 0);
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(0, -1);
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(-1, -1);
    assert(fixture.graph.edges.size() == 0);
    
    // Test out-of-bounds indices
    fixture.graph.addEdge(4, 0); // Only have indices 0-3
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(0, 10);
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(100, 200);
    assert(fixture.graph.edges.size() == 0);
    
    // Add valid edge to confirm function still works
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.edges.size() == 1);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addEdge_self_loops() {
    std::cout << "Testing FunctionGraph::addEdge self loops... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes();
    
    // Self-loops should be allowed
    fixture.graph.addEdge(0, 0);
    assert(fixture.graph.edges.size() == 1);
    assert(fixture.graph.edges[0].first == 0);
    assert(fixture.graph.edges[0].second == 0);
    
    // Duplicate self-loop should be prevented
    fixture.graph.addEdge(0, 0);
    assert(fixture.graph.edges.size() == 1);
    
    // Multiple different self-loops
    fixture.graph.addEdge(1, 1);
    fixture.graph.addEdge(2, 2);
    assert(fixture.graph.edges.size() == 3);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_addEdge_empty_graph() {
    std::cout << "Testing FunctionGraph::addEdge on empty graph... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp(); // No nodes added
    
    // Any edge on empty graph should fail
    fixture.graph.addEdge(0, 0);
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.edges.size() == 0);
    
    fixture.graph.addEdge(-1, 0);
    assert(fixture.graph.edges.size() == 0);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// FunctionGraph::isValid Tests
// =============================================================================

void test_FunctionGraph_isValid_basic() {
    std::cout << "Testing FunctionGraph::isValid basic functionality... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Empty graph should be valid
    assert(fixture.graph.isValid() == true);
    
    // Graph with only nodes should be valid
    fixture.addSampleNodes();
    assert(fixture.graph.isValid() == true);
    
    // Graph with valid edges should be valid
    fixture.graph.addEdge(0, 1);
    fixture.graph.addEdge(1, 2);
    fixture.graph.addEdge(2, 3);
    assert(fixture.graph.isValid() == true);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_isValid_invalid_edges() {
    std::cout << "Testing FunctionGraph::isValid with invalid edges... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes(); // 4 nodes: indices 0-3
    
    // Add valid edges first
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.isValid() == true);
    
    // Manually add invalid edge (bypassing addEdge validation)
    fixture.graph.edges.push_back(std::make_pair(0, 5)); // Index 5 doesn't exist
    assert(fixture.graph.isValid() == false);
    
    // Clear edges and try another invalid case
    fixture.graph.edges.clear();
    fixture.graph.edges.push_back(std::make_pair(-1, 0)); // Negative index
    assert(fixture.graph.isValid() == false);
    
    // Another invalid case
    fixture.graph.edges.clear();
    fixture.graph.edges.push_back(std::make_pair(10, 20)); // Both indices out of bounds
    assert(fixture.graph.isValid() == false);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_isValid_duplicate_edges() {
    std::cout << "Testing FunctionGraph::isValid with duplicate edges... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    fixture.addSampleNodes();
    
    // Add edges using normal method (no duplicates should be created)
    fixture.graph.addEdge(0, 1);
    fixture.graph.addEdge(1, 2);
    fixture.graph.addEdge(0, 1); // Duplicate attempt - should be ignored
    assert(fixture.graph.isValid() == true);
    
    // Manually add duplicate edge (bypassing addEdge validation)
    fixture.graph.edges.push_back(std::make_pair(0, 1)); // Duplicate of existing edge
    assert(fixture.graph.isValid() == false);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_isValid_complex_scenarios() {
    std::cout << "Testing FunctionGraph::isValid complex scenarios... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Large graph with many valid edges
    const int NUM_NODES = 100;
    for (int i = 0; i < NUM_NODES; ++i) {
        std::string name = "func" + std::to_string(i);
        fixture.graph.nodes.push_back(FunctionNode(name, "file.cpp", i));
    }
    
    // Create chain of calls: 0->1->2->...->99
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        fixture.graph.addEdge(i, i + 1);
    }
    
    // Add some cycles and self-loops
    fixture.graph.addEdge(99, 0); // Cycle back to start
    fixture.graph.addEdge(50, 50); // Self-loop
    fixture.graph.addEdge(0, 25); // Skip ahead
    
    assert(fixture.graph.isValid() == true);
    
    // Now make it invalid by manually adding bad edge
    fixture.graph.edges.push_back(std::make_pair(0, NUM_NODES)); // Out of bounds
    assert(fixture.graph.isValid() == false);
    
    std::cout << "PASSED\n";
}

void test_FunctionGraph_isValid_edge_cases() {
    std::cout << "Testing FunctionGraph::isValid edge cases... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Single node graph
    fixture.graph.nodes.push_back(FunctionNode("single", "file.cpp", 1));
    assert(fixture.graph.isValid() == true);
    
    // Single node with self-loop
    fixture.graph.addEdge(0, 0);
    assert(fixture.graph.isValid() == true);
    
    // Graph becomes invalid after removing nodes but keeping edges
    fixture.graph.nodes.push_back(FunctionNode("second", "file.cpp", 2));
    fixture.graph.addEdge(0, 1);
    assert(fixture.graph.isValid() == true);
    
    // Remove a node but keep edge referencing it (simulate corruption)
    fixture.graph.nodes.pop_back(); // Now only node 0 exists, but edge 0->1 remains
    assert(fixture.graph.isValid() == false);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// Integration Tests
// =============================================================================

void test_FunctionGraph_integration() {
    std::cout << "Testing FunctionGraph integration scenarios... ";
    
    FunctionGraphTestFixture fixture;
    fixture.setUp();
    
    // Build a realistic function call graph
    int mainIdx = fixture.graph.addNode(FunctionNode("main", "main.cpp", 1));
    int initIdx = fixture.graph.addNode(FunctionNode("initialize", "init.cpp", 10));
    int processIdx = fixture.graph.addNode(FunctionNode("processData", "core.cpp", 50));
    int validateIdx = fixture.graph.addNode(FunctionNode("validate", "utils.cpp", 20));
    int cleanupIdx = fixture.graph.addNode(FunctionNode("cleanup", "cleanup.cpp", 5));
    
    // Create call relationships
    fixture.graph.addEdge(mainIdx, initIdx);      // main calls initialize
    fixture.graph.addEdge(mainIdx, processIdx);   // main calls processData
    fixture.graph.addEdge(processIdx, validateIdx); // processData calls validate
    fixture.graph.addEdge(mainIdx, cleanupIdx);   // main calls cleanup
    fixture.graph.addEdge(processIdx, processIdx); // processData calls itself (recursion)
    
    assert(fixture.graph.nodes.size() == 5);
    assert(fixture.graph.edges.size() == 5);
    assert(fixture.graph.isValid() == true);
    
    // Test finding all nodes
    assert(fixture.graph.findNodeIndex("main") == mainIdx);
    assert(fixture.graph.findNodeIndex("initialize") == initIdx);
    assert(fixture.graph.findNodeIndex("processData") == processIdx);
    assert(fixture.graph.findNodeIndex("validate") == validateIdx);
    assert(fixture.graph.findNodeIndex("cleanup") == cleanupIdx);
    
    // Test adding duplicate nodes (should return existing indices)
    int dupMainIdx = fixture.graph.addNode(FunctionNode("main", "different.cpp", 999));
    assert(dupMainIdx == mainIdx);
    assert(fixture.graph.nodes.size() == 5); // No new nodes added
    
    // Add external function
    FunctionNode external("printf", "stdio.h", 0);
    external.isExternal = true;
    int extIdx = fixture.graph.addNode(external);
    fixture.graph.addEdge(mainIdx, extIdx);
    
    assert(fixture.graph.nodes.size() == 6);
    assert(fixture.graph.nodes[extIdx].isExternal == true);
    assert(fixture.graph.isValid() == true);
    
    std::cout << "PASSED\n";
}

// =============================================================================
// Main Test Runner
// =============================================================================

int main() {
    std::cout << "Running Comprehensive CodeMap Types Tests\n";
    std::cout << "==========================================\n\n";
    
    // FunctionNode Tests
    std::cout << "--- FunctionNode Tests ---\n";
    test_FunctionNode_default_constructor();
    test_FunctionNode_parametric_constructor();
    test_FunctionNode_edge_cases();
    test_FunctionNode_flag_modification();
    
    // FunctionGraph::findNodeIndex Tests
    std::cout << "\n--- FunctionGraph::findNodeIndex Tests ---\n";
    test_FunctionGraph_findNodeIndex_basic();
    test_FunctionGraph_findNodeIndex_not_found();
    test_FunctionGraph_findNodeIndex_empty_graph();
    test_FunctionGraph_findNodeIndex_duplicates();
    test_FunctionGraph_findNodeIndex_special_characters();
    
    // FunctionGraph::addNode Tests
    std::cout << "\n--- FunctionGraph::addNode Tests ---\n";
    test_FunctionGraph_addNode_basic();
    test_FunctionGraph_addNode_duplicate_detection();
    test_FunctionGraph_addNode_edge_cases();
    test_FunctionGraph_addNode_large_scale();
    
    // FunctionGraph::addEdge Tests
    std::cout << "\n--- FunctionGraph::addEdge Tests ---\n";
    test_FunctionGraph_addEdge_basic();
    test_FunctionGraph_addEdge_duplicate_prevention();
    test_FunctionGraph_addEdge_invalid_indices();
    test_FunctionGraph_addEdge_self_loops();
    test_FunctionGraph_addEdge_empty_graph();
    
    // FunctionGraph::isValid Tests
    std::cout << "\n--- FunctionGraph::isValid Tests ---\n";
    test_FunctionGraph_isValid_basic();
    test_FunctionGraph_isValid_invalid_edges();
    test_FunctionGraph_isValid_duplicate_edges();
    test_FunctionGraph_isValid_complex_scenarios();
    test_FunctionGraph_isValid_edge_cases();
    
    // Integration Tests
    std::cout << "\n--- Integration Tests ---\n";
    test_FunctionGraph_integration();
    
    std::cout << "\n==========================================\n";
    std::cout << "All CodeMap Types tests PASSED!\n";
    std::cout << "Total test functions: 25\n";
    
    return 0;
}