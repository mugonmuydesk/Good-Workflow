// PROTECTED CONTRACT: Do not edit except with explicit approval

#pragma once

#include <string>
#include <memory>
#include <functional>
#include "codemap_types.h"
#include "graph_builder.h"

namespace CodeMap {

using namespace codemap;  // Use codemap namespace types

/**
 * WebviewApp - Main application class for the CodeMap visualization frontend.
 * 
 * This class manages the webview window and communication between the C++ backend
 * and JavaScript frontend. It provides the main entry point for launching the
 * interactive graph visualization.
 */
class WebviewApp {
public:
    /**
     * Configuration for the webview application
     */
    struct Config {
        std::string windowTitle = "CodeMap - Visual Call Graph";
        int windowWidth = 1200;
        int windowHeight = 800;
        bool debug = false;
        std::string htmlPath = "frontend/index.html";
    };

    /**
     * Create a new WebviewApp instance
     */
    WebviewApp();
    
    /**
     * Create a new WebviewApp instance with custom configuration
     */
    explicit WebviewApp(const Config& config);
    
    /**
     * Destructor
     */
    ~WebviewApp();

    /**
     * Initialize the webview application
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * Load a project and display its call graph
     * @param projectPath Path to the C++ project directory
     * @return true if project loaded successfully, false otherwise
     */
    bool loadProject(const std::string& projectPath);

    /**
     * Load a pre-computed graph from JSON file
     * @param jsonPath Path to the JSON file containing the graph data
     * @return true if graph loaded successfully, false otherwise
     */
    bool loadGraphFromJSON(const std::string& jsonPath);

    /**
     * Set the graph data directly
     * @param graph The FunctionGraph to display
     */
    void setGraph(const FunctionGraph& graph);

    /**
     * Run the application main loop
     * This function blocks until the webview window is closed
     * @return 0 on success, error code on failure
     */
    int run();

    /**
     * Send a message to the JavaScript frontend
     * @param message JSON message to send to frontend
     */
    void sendMessage(const std::string& message);

    /**
     * Register a callback for messages from JavaScript
     * @param callback Function to call when message received from frontend
     */
    void onMessage(std::function<void(const std::string&)> callback);

    /**
     * Export the current graph to a JSON file
     * @param outputPath Path where the JSON file should be saved
     * @return true if export successful, false otherwise
     */
    bool exportGraph(const std::string& outputPath) const;

    /**
     * Get the current configuration
     */
    const Config& getConfig() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Disable copy
    WebviewApp(const WebviewApp&) = delete;
    WebviewApp& operator=(const WebviewApp&) = delete;
};

/**
 * IWebviewBridge - Interface for communication between C++ and JavaScript
 * 
 * This interface defines the protocol for bidirectional communication
 * between the C++ backend and JavaScript frontend.
 */
class IWebviewBridge {
public:
    virtual ~IWebviewBridge() = default;

    /**
     * Send graph data to the frontend
     * @param graphJSON JSON representation of the function graph
     */
    virtual void sendGraphData(const std::string& graphJSON) = 0;

    /**
     * Handle a command from the frontend
     * @param command Command string from JavaScript
     * @param payload JSON payload with command parameters
     * @return JSON response to send back to frontend
     */
    virtual std::string handleCommand(const std::string& command, const std::string& payload) = 0;

    /**
     * Notify frontend of an error
     * @param errorMessage Human-readable error message
     */
    virtual void notifyError(const std::string& errorMessage) = 0;

    /**
     * Notify frontend of progress during long operations
     * @param message Progress message
     * @param percentage Progress percentage (0-100)
     */
    virtual void notifyProgress(const std::string& message, int percentage) = 0;
};

} // namespace CodeMap