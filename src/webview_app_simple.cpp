#include "webview_app.h"
#include "json_exporter.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace CodeMap {

using namespace codemap;

// Simplified implementation for now - will use basic file I/O
class WebviewApp::Impl {
public:
    Config config;
    std::unique_ptr<GraphBuilder> graphBuilder;
    FunctionGraph currentGraph;
    std::function<void(const std::string&)> messageCallback;
    
    Impl(const Config& cfg) : config(cfg) {
        graphBuilder = std::make_unique<GraphBuilder>();
    }
    
    ~Impl() = default;
    
    bool initialize() {
        // For Phase 4, we'll create a simple HTML file viewer
        std::cout << "WebviewApp initialized (simplified mode)\n";
        return true;
    }
    
    void sendGraphToFrontend() {
        // Export graph to JSON file for frontend to load
        codemap::JsonExporter exporter;
        std::string graphJSON = codemap::JsonExporter::graphToJSON(currentGraph);
        
        // Save to temp file
        std::string tempFile = "/tmp/codemap_graph.json";
        std::ofstream out(tempFile);
        if (out.is_open()) {
            out << graphJSON;
            out.close();
            std::cout << "Graph data saved to: " << tempFile << "\n";
        }
    }
};

// WebviewApp public interface implementation
WebviewApp::WebviewApp() : pImpl(std::make_unique<Impl>(Config{})) {}

WebviewApp::WebviewApp(const Config& config) : pImpl(std::make_unique<Impl>(config)) {}

WebviewApp::~WebviewApp() = default;

bool WebviewApp::initialize() {
    return pImpl->initialize();
}

bool WebviewApp::loadProject(const std::string& projectPath) {
    try {
        std::cout << "Scanning project: " << projectPath << "\n";
        
        // Build graph from project
        auto graphPtr = pImpl->graphBuilder->buildFromProject(projectPath);
        pImpl->currentGraph = *graphPtr;
        
        // Save to file for frontend
        pImpl->sendGraphToFrontend();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load project: " << e.what() << "\n";
        return false;
    }
}

bool WebviewApp::loadGraphFromJSON(const std::string& jsonPath) {
    try {
        // Load graph from file
        auto graphPtr = pImpl->graphBuilder->loadFromFile(jsonPath);
        pImpl->currentGraph = *graphPtr;
        
        // Save for frontend
        pImpl->sendGraphToFrontend();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load graph: " << e.what() << "\n";
        return false;
    }
}

void WebviewApp::setGraph(const FunctionGraph& graph) {
    pImpl->currentGraph = graph;
    pImpl->sendGraphToFrontend();
}

int WebviewApp::run() {
    // For now, just export the HTML file and inform user
    std::filesystem::path htmlPath = std::filesystem::current_path() / "frontend" / "index.html";
    
    if (!std::filesystem::exists(htmlPath)) {
        htmlPath = std::filesystem::path(pImpl->config.htmlPath);
    }
    
    std::cout << "\n===========================================\n";
    std::cout << "CodeMap Visualization Ready!\n";
    std::cout << "===========================================\n\n";
    std::cout << "Graph data exported to: /tmp/codemap_graph.json\n";
    std::cout << "Open the following file in your browser:\n";
    std::cout << "  file://" << std::filesystem::absolute(htmlPath) << "\n\n";
    std::cout << "The frontend will load the demo data automatically.\n";
    std::cout << "To load your project data, modify app.js to load from /tmp/codemap_graph.json\n";
    
    return 0;
}

void WebviewApp::sendMessage(const std::string& message) {
    std::cout << "Message to frontend: " << message << "\n";
}

void WebviewApp::onMessage(std::function<void(const std::string&)> callback) {
    pImpl->messageCallback = callback;
}

bool WebviewApp::exportGraph(const std::string& outputPath) const {
    try {
        return pImpl->graphBuilder->saveToFile(pImpl->currentGraph, outputPath);
    } catch (const std::exception& e) {
        std::cerr << "Failed to export graph: " << e.what() << "\n";
        return false;
    }
}

const WebviewApp::Config& WebviewApp::getConfig() const {
    return pImpl->config;
}

} // namespace CodeMap