#include "codemap_types.h"
#include <algorithm>

namespace codemap {

int FunctionGraph::findNodeIndex(const std::string& functionName) const {
    // PSEUDOCODE:
    // for i = 0 to nodes.size()-1:
    //     if nodes[i].name == functionName:
    //         return i
    // return -1
    
    auto it = std::find_if(nodes.begin(), nodes.end(),
        [&functionName](const FunctionNode& node) {
            return node.name == functionName;
        });
    
    if (it != nodes.end()) {
        return static_cast<int>(std::distance(nodes.begin(), it));
    }
    return -1;
}

int FunctionGraph::addNode(const FunctionNode& node) {
    // PSEUDOCODE:
    // check if node already exists
    // if exists, return existing index
    // else add node and return new index
    
    int existingIndex = findNodeIndex(node.name);
    if (existingIndex != -1) {
        return existingIndex;
    }
    
    nodes.push_back(node);
    return static_cast<int>(nodes.size() - 1);
}

void FunctionGraph::addEdge(int callerIndex, int calleeIndex) {
    // PSEUDOCODE:
    // validate indices are within bounds
    // check if edge already exists
    // if not exists, add edge
    
    if (callerIndex < 0 || callerIndex >= static_cast<int>(nodes.size()) ||
        calleeIndex < 0 || calleeIndex >= static_cast<int>(nodes.size())) {
        return;
    }
    
    auto edge = std::make_pair(callerIndex, calleeIndex);
    if (std::find(edges.begin(), edges.end(), edge) == edges.end()) {
        edges.push_back(edge);
    }
}

bool FunctionGraph::isValid() const {
    // PSEUDOCODE:
    // check all edge indices are valid
    // check no duplicate edges
    // return true if all checks pass
    
    for (const auto& edge : edges) {
        if (edge.first < 0 || edge.first >= static_cast<int>(nodes.size()) ||
            edge.second < 0 || edge.second >= static_cast<int>(nodes.size())) {
            return false;
        }
    }
    
    // Check for duplicate edges
    auto edgesCopy = edges;
    std::sort(edgesCopy.begin(), edgesCopy.end());
    auto last = std::unique(edgesCopy.begin(), edgesCopy.end());
    
    return last == edgesCopy.end();
}

} // namespace codemap