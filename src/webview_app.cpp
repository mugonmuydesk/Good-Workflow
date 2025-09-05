#include "webview_app.h"
#include "json_exporter.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

// Platform-specific includes
#include "webview.h"

namespace CodeMap {

using namespace codemap;  // Use codemap namespace types

// Implementation class for WebviewApp
class WebviewApp::Impl {
public:
    Config config;
    std::unique_ptr<GraphBuilder> graphBuilder;
    FunctionGraph currentGraph;
    webview::webview* webviewInstance;
    std::function<void(const std::string&)> messageCallback;
    
    Impl(const Config& cfg) : config(cfg), webviewInstance(nullptr) {
        graphBuilder = std::make_unique<GraphBuilder>();
    }
    
    ~Impl() {
        if (webviewInstance) {
            webviewInstance->terminate();
            delete webviewInstance;
        }
    }
    
    bool initialize() {
        try {
            // Create webview instance
            webviewInstance = new webview::webview(config.debug, nullptr);
            
            // Set window properties
            webviewInstance->set_title(config.windowTitle);
            webviewInstance->set_size(config.windowWidth, config.windowHeight, WEBVIEW_HINT_NONE);
            
            // Load HTML content
            std::string htmlContent = loadHTMLContent();
            if (htmlContent.empty()) {
                std::cerr << "Failed to load HTML content from: " << config.htmlPath << std::endl;
                return false;
            }
            
            // Navigate to HTML
            webviewInstance->navigate("data:text/html," + htmlContent);
            
            // Bind C++ functions to JavaScript
            bindJavaScriptFunctions();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize webview: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::string loadHTMLContent() {
        std::filesystem::path htmlPath = std::filesystem::current_path() / config.htmlPath;
        
        if (!std::filesystem::exists(htmlPath)) {
            // Try relative to executable
            htmlPath = std::filesystem::path(config.htmlPath);
            if (!std::filesystem::exists(htmlPath)) {
                return "";
            }
        }
        
        std::ifstream file(htmlPath);
        if (!file.is_open()) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Replace relative paths with absolute paths for CSS and JS
        std::filesystem::path frontendDir = htmlPath.parent_path();
        
        // Replace styles.css path
        size_t pos = content.find("href=\"styles.css\"");
        if (pos != std::string::npos) {
            std::string cssPath = (frontendDir / "styles.css").string();
            std::ifstream cssFile(cssPath);
            if (cssFile.is_open()) {
                std::stringstream cssBuffer;
                cssBuffer << cssFile.rdbuf();
                std::string cssContent = cssBuffer.str();
                
                // Inject CSS inline
                std::string inlineCSS = "<style>" + cssContent + "</style>";
                content.replace(pos - 6, 30, inlineCSS);  // Replace <link> tag
            }
        }
        
        // Replace app.js path
        pos = content.find("src=\"app.js\"");
        if (pos != std::string::npos) {
            std::string jsPath = (frontendDir / "app.js").string();
            std::ifstream jsFile(jsPath);
            if (jsFile.is_open()) {
                std::stringstream jsBuffer;
                jsBuffer << jsFile.rdbuf();
                std::string jsContent = jsBuffer.str();
                
                // Inject JS inline
                std::string inlineJS = "<script>" + jsContent + "</script>";
                size_t scriptStart = content.find("<script src=\"app.js\">");
                if (scriptStart != std::string::npos) {
                    size_t scriptEnd = content.find("</script>", scriptStart) + 9;
                    content.replace(scriptStart, scriptEnd - scriptStart, inlineJS);
                }
            }
        }
        
        return content;
    }
    
    void bindJavaScriptFunctions() {
        // Bind C++ bridge object to JavaScript
        webviewInstance->bind("cppBridge", [this](const std::string& message) -> std::string {
            return handleJavaScriptMessage(message);
        });
        
        // Initialize bridge in JavaScript
        webviewInstance->eval(R"(
            window.cppBridge = {
                postMessage: function(message) {
                    return cppBridge(message);
                },
                onMessage: null
            };
        )");
    }
    
    std::string handleJavaScriptMessage(const std::string& message) {
        try {
            // Parse message (simplified - should use proper JSON parser)
            if (message.find("\"type\":\"request_graph\"") != std::string::npos) {
                // Send current graph data
                sendGraphToFrontend();
                return "{\"status\":\"success\"}";
            } else if (message.find("\"type\":\"export\"") != std::string::npos) {
                // Handle export request
                return handleExportRequest(message);
            }
            
            // Call user callback if registered
            if (messageCallback) {
                messageCallback(message);
            }
            
            return "{\"status\":\"success\"}";
        } catch (const std::exception& e) {
            return "{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}";
        }
    }
    
    void sendGraphToFrontend() {
        JSONExporter exporter;
        std::string graphJSON = exporter.exportGraph(currentGraph);
        
        std::string jsCode = R"(
            if (window.cppBridge && window.cppBridge.onMessage) {
                window.cppBridge.onMessage(')" + 
                "{\"type\":\"graph\",\"payload\":" + graphJSON + "}" + 
                R"(');
            }
        )";
        
        webviewInstance->eval(jsCode);
    }
    
    std::string handleExportRequest(const std::string& message) {
        // Extract export type and path from message
        // This is simplified - should use proper JSON parsing
        
        if (message.find("\"format\":\"json\"") != std::string::npos) {
            JSONExporter exporter;
            std::string json = exporter.exportGraph(currentGraph);
            return "{\"status\":\"success\",\"data\":" + json + "}";
        }
        
        return "{\"status\":\"error\",\"message\":\"Unknown export format\"}";
    }
    
    void sendMessageToFrontend(const std::string& type, const std::string& payload) {
        std::string message = "{\"type\":\"" + type + "\",\"payload\":" + payload + "}";
        std::string jsCode = R"(
            if (window.cppBridge && window.cppBridge.onMessage) {
                window.cppBridge.onMessage(')" + message + R"(');
            }
        )";
        
        webviewInstance->eval(jsCode);
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
        // Show progress
        pImpl->sendMessageToFrontend("progress", 
            "{\"message\":\"Scanning project...\",\"percentage\":0}");
        
        // Build graph from project
        pImpl->currentGraph = pImpl->graphBuilder->buildFromProject(projectPath);
        
        // Send to frontend
        pImpl->sendGraphToFrontend();
        
        return true;
    } catch (const std::exception& e) {
        pImpl->sendMessageToFrontend("error", 
            "{\"message\":\"Failed to load project: " + std::string(e.what()) + "\"}");
        return false;
    }
}

bool WebviewApp::loadGraphFromJSON(const std::string& jsonPath) {
    try {
        // Load graph from file
        pImpl->currentGraph = pImpl->graphBuilder->loadFromFile(jsonPath);
        
        // Send to frontend
        pImpl->sendGraphToFrontend();
        
        return true;
    } catch (const std::exception& e) {
        pImpl->sendMessageToFrontend("error", 
            "{\"message\":\"Failed to load graph: " + std::string(e.what()) + "\"}");
        return false;
    }
}

void WebviewApp::setGraph(const FunctionGraph& graph) {
    pImpl->currentGraph = graph;
    pImpl->sendGraphToFrontend();
}

int WebviewApp::run() {
    if (!pImpl->webviewInstance) {
        return -1;
    }
    
    // Run webview event loop
    pImpl->webviewInstance->run();
    
    return 0;
}

void WebviewApp::sendMessage(const std::string& message) {
    if (pImpl->webviewInstance) {
        std::string jsCode = R"(
            if (window.cppBridge && window.cppBridge.onMessage) {
                window.cppBridge.onMessage(')" + message + R"(');
            }
        )";
        pImpl->webviewInstance->eval(jsCode);
    }
}

void WebviewApp::onMessage(std::function<void(const std::string&)> callback) {
    pImpl->messageCallback = callback;
}

bool WebviewApp::exportGraph(const std::string& outputPath) const {
    try {
        return pImpl->graphBuilder->saveToFile(pImpl->currentGraph, outputPath);
    } catch (const std::exception& e) {
        std::cerr << "Failed to export graph: " << e.what() << std::endl;
        return false;
    }
}

const WebviewApp::Config& WebviewApp::getConfig() const {
    return pImpl->config;
}

} // namespace CodeMap