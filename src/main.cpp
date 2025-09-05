#include <iostream>
#include <string>
#include <filesystem>
#include "webview_app.h"
#include "graph_builder.h"
#include "json_exporter.h"

using namespace CodeMap;

void printUsage(const char* programName) {
    std::cout << "CodeMap - Visual Call Graph Generator\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <project-directory>    Analyze a C++ project\n";
    std::cout << "  " << programName << " -j <json-file>        Load a pre-computed graph\n";
    std::cout << "  " << programName << " -h, --help            Show this help message\n";
    std::cout << "  " << programName << " --demo                Run with demo data\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " /path/to/project      Scan and visualize project\n";
    std::cout << "  " << programName << " -j graph.json         Load existing graph from JSON\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string arg1(argv[1]);
    
    // Help
    if (arg1 == "-h" || arg1 == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    // Configure webview app
    WebviewApp::Config config;
    config.windowTitle = "CodeMap - Visual Call Graph";
    config.windowWidth = 1400;
    config.windowHeight = 900;
    config.debug = false;
    
    // Determine HTML path relative to executable
    std::filesystem::path exePath = std::filesystem::path(argv[0]).parent_path();
    config.htmlPath = (exePath / "frontend" / "index.html").string();
    
    // Create and initialize app
    WebviewApp app(config);
    
    std::cout << "Initializing CodeMap visualization...\n";
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize webview application\n";
        return 1;
    }
    
    // Handle different modes
    if (arg1 == "--demo") {
        // Demo mode - create sample graph
        std::cout << "Running in demo mode with sample data...\n";
        
        FunctionGraph demoGraph;
        
        // Add sample nodes
        FunctionNode main("main", "src/main.cpp", 10);
        FunctionNode parser("parseFile", "src/parser.cpp", 25);
        FunctionNode builder("buildGraph", "src/graph_builder.cpp", 40);
        FunctionNode exporter("exportJSON", "src/json_exporter.cpp", 15);
        FunctionNode stub("TODO_implement", "src/utils.cpp", 5);
        stub.isStub = true;
        FunctionNode missing("missingFunction", "", 0);
        missing.isMissing = true;
        FunctionNode external("std::cout", "", 0);
        external.isExternal = true;
        
        demoGraph.addNode(main);      // 0
        demoGraph.addNode(parser);    // 1
        demoGraph.addNode(builder);   // 2
        demoGraph.addNode(exporter);  // 3
        demoGraph.addNode(stub);      // 4
        demoGraph.addNode(missing);   // 5
        demoGraph.addNode(external);  // 6
        
        // Add sample edges using indices
        demoGraph.addEdge(0, 1);  // main -> parseFile
        demoGraph.addEdge(0, 2);  // main -> buildGraph  
        demoGraph.addEdge(1, 4);  // parseFile -> TODO_implement
        demoGraph.addEdge(2, 3);  // buildGraph -> exportJSON
        demoGraph.addEdge(2, 5);  // buildGraph -> missingFunction
        demoGraph.addEdge(3, 6);  // exportJSON -> std::cout
        
        app.setGraph(demoGraph);
        
    } else if (arg1 == "-j" && argc >= 3) {
        // Load from JSON file
        std::string jsonFile(argv[2]);
        std::cout << "Loading graph from: " << jsonFile << "\n";
        
        if (!app.loadGraphFromJSON(jsonFile)) {
            std::cerr << "Failed to load graph from JSON file\n";
            return 1;
        }
        
    } else {
        // Analyze project directory
        std::string projectPath(argv[1]);
        std::cout << "Analyzing project: " << projectPath << "\n";
        
        if (!std::filesystem::exists(projectPath)) {
            std::cerr << "Error: Project path does not exist: " << projectPath << "\n";
            return 1;
        }
        
        if (!app.loadProject(projectPath)) {
            std::cerr << "Failed to analyze project\n";
            return 1;
        }
    }
    
    std::cout << "Starting CodeMap visualization...\n";
    std::cout << "Close the window to exit.\n";
    
    // Run the application
    return app.run();
}