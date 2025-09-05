// CodeMap Frontend Application
// Main JavaScript file for the interactive call graph visualization

// Global variables
let cy = null;  // Cytoscape instance
let graphData = null;  // Current graph data
let originalGraphData = null;  // Original unfiltered graph data
let moduleGroups = new Map();  // Map of module names to their nodes
let collapsedModules = new Set();  // Set of collapsed module names

// Initialize the application when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    initializeCytoscape();
    setupEventListeners();
    setupWebviewBridge();
    
    // Load demo data if available (for testing)
    loadDemoData();
});

/**
 * Initialize Cytoscape graph visualization
 */
function initializeCytoscape() {
    cy = cytoscape({
        container: document.getElementById('cytoscape'),
        
        style: [
            // Node styles
            {
                selector: 'node',
                style: {
                    'label': 'data(label)',
                    'text-valign': 'center',
                    'text-halign': 'center',
                    'background-color': '#4ec9b0',
                    'color': '#fff',
                    'text-outline-width': 2,
                    'text-outline-color': '#1e1e1e',
                    'font-size': '12px',
                    'width': 'label',
                    'height': 'label',
                    'padding': '10px',
                    'shape': 'round-rectangle',
                    'border-width': 2,
                    'border-color': '#3e3e42'
                }
            },
            
            // Edge styles
            {
                selector: 'edge',
                style: {
                    'width': 2,
                    'line-color': '#606060',
                    'target-arrow-color': '#606060',
                    'target-arrow-shape': 'triangle',
                    'curve-style': 'bezier',
                    'arrow-scale': 1.2,
                    'label': 'data(label)',
                    'font-size': '10px',
                    'text-background-color': '#1e1e1e',
                    'text-background-opacity': 0.8,
                    'text-background-padding': '2px',
                    'text-background-shape': 'roundrectangle',
                    'color': '#808080',
                    'text-outline-width': 0,
                    'edge-text-rotation': 'autorotate'
                }
            },
            
            // Module group styles
            {
                selector: '.module-group',
                style: {
                    'background-color': '#2d2d30',
                    'background-opacity': 0.3,
                    'border-width': 2,
                    'border-color': '#606060',
                    'border-style': 'dashed',
                    'shape': 'roundrectangle',
                    'padding': '20px',
                    'text-valign': 'top',
                    'text-halign': 'center',
                    'font-weight': 'bold'
                }
            },
            
            // Node status styles
            {
                selector: '.implemented',
                style: {
                    'background-color': '#4ec9b0',
                    'border-color': '#3a9b8c'
                }
            },
            {
                selector: '.stub',
                style: {
                    'background-color': '#dcdcaa',
                    'border-color': '#b8b888'
                }
            },
            {
                selector: '.missing',
                style: {
                    'background-color': '#f48771',
                    'border-color': '#d06050'
                }
            },
            {
                selector: '.external',
                style: {
                    'background-color': '#808080',
                    'border-color': '#606060'
                }
            },
            
            // Highlighted styles
            {
                selector: '.highlighted',
                style: {
                    'border-width': 4,
                    'border-color': '#4ec9b0',
                    'z-index': 999
                }
            },
            {
                selector: '.dimmed',
                style: {
                    'opacity': 0.3
                }
            },
            {
                selector: 'edge.highlighted',
                style: {
                    'width': 3,
                    'line-color': '#4ec9b0',
                    'target-arrow-color': '#4ec9b0',
                    'z-index': 999
                }
            },
            
            // Search highlighting styles
            {
                selector: '.search-match',
                style: {
                    'border-width': 4,
                    'border-color': '#ff9800',
                    'background-color': '#ffb74d',
                    'z-index': 1000,
                    'overlay-padding': 6,
                    'overlay-color': '#ff9800',
                    'overlay-opacity': 0.2
                }
            },
            {
                selector: '.search-related',
                style: {
                    'opacity': 0.8,
                    'z-index': 500
                }
            },
            {
                selector: '.search-dimmed',
                style: {
                    'opacity': 0.2
                }
            },
            {
                selector: 'edge.search-match',
                style: {
                    'width': 3,
                    'line-color': '#ff9800',
                    'target-arrow-color': '#ff9800',
                    'z-index': 1000
                }
            },
            {
                selector: 'edge.search-related',
                style: {
                    'width': 2.5,
                    'line-color': '#ffa726',
                    'target-arrow-color': '#ffa726',
                    'z-index': 500
                }
            }
        ],
        
        layout: {
            name: 'dagre',
            rankDir: 'TB',
            nodeDimensionsIncludeLabels: true,
            animate: false,
            fit: true,
            padding: 50
        },
        
        wheelSensitivity: 0.2,
        minZoom: 0.1,
        maxZoom: 5
    });
    
    // Node click handler
    cy.on('tap', 'node', function(evt) {
        const node = evt.target;
        if (!node.data('isModule')) {
            showNodeDetails(node);
            highlightConnections(node);
        }
    });
    
    // Double-click handler for module groups
    cy.on('dbltap', '.module-group', function(evt) {
        const moduleNode = evt.target;
        const moduleName = moduleNode.id().replace('module-', '');
        toggleModule(moduleName);
    });
    
    // Node hover handlers for enhanced tooltips
    cy.on('mouseover', 'node', function(evt) {
        const node = evt.target;
        if (!node.data('isModule')) {
            showEnhancedTooltip(node, evt);
        }
    });
    
    cy.on('mouseout', 'node', function(evt) {
        hideEnhancedTooltip();
    });
    
    // Background click handler
    cy.on('tap', function(evt) {
        if (evt.target === cy) {
            hideSidebar();
            clearHighlights();
        }
    });
}

/**
 * Setup event listeners for UI controls
 */
function setupEventListeners() {
    // Layout selector
    document.getElementById('layout-select').addEventListener('change', (e) => {
        applyLayout(e.target.value);
    });
    
    // Filter input
    document.getElementById('filter-input').addEventListener('input', (e) => {
        filterGraph(e.target.value);
    });
    
    // View controls
    document.getElementById('fit-button').addEventListener('click', () => {
        cy.fit();
    });
    
    document.getElementById('reset-button').addEventListener('click', () => {
        resetView();
    });
    
    // Export controls
    document.getElementById('export-png').addEventListener('click', () => {
        exportPNG();
    });
    
    document.getElementById('export-json').addEventListener('click', () => {
        exportJSON();
    });
    
    // Sidebar close button
    document.getElementById('close-sidebar').addEventListener('click', () => {
        hideSidebar();
        clearHighlights();
    });
    
    // Settings panel controls
    document.getElementById('settings-button').addEventListener('click', () => {
        showSettings();
    });
    
    document.getElementById('close-settings').addEventListener('click', () => {
        hideSettings();
    });
    
    document.getElementById('apply-settings').addEventListener('click', () => {
        applySettings();
    });
    
    document.getElementById('reset-settings').addEventListener('click', () => {
        resetSettings();
    });
    
    // Settings input handlers
    setupSettingsInputs();
}

/**
 * Setup communication bridge with C++ webview
 */
function setupWebviewBridge() {
    // Listen for messages from C++
    if (window.cppBridge) {
        window.cppBridge.onMessage = (message) => {
            handleCppMessage(message);
        };
    }
}

/**
 * Handle messages from C++ backend
 */
function handleCppMessage(message) {
    try {
        const data = JSON.parse(message);
        
        switch (data.type) {
            case 'graph':
                loadGraphData(data.payload);
                break;
            case 'error':
                showError(data.message);
                break;
            case 'progress':
                showProgress(data.message, data.percentage);
                break;
            default:
                console.warn('Unknown message type:', data.type);
        }
    } catch (error) {
        console.error('Error handling C++ message:', error);
    }
}

/**
 * Send message to C++ backend
 */
function sendToCpp(type, payload) {
    if (window.cppBridge && window.cppBridge.postMessage) {
        window.cppBridge.postMessage(JSON.stringify({ type, payload }));
    }
}

/**
 * Load graph data from JSON
 */
function loadGraphData(data) {
    showLoading();
    
    try {
        graphData = data;
        originalGraphData = JSON.parse(JSON.stringify(data));  // Deep copy
        
        // Convert to Cytoscape format
        const elements = convertToCytoscapeFormat(data);
        
        // Update graph
        cy.elements().remove();
        cy.add(elements);
        
        // Apply layout
        applyLayout('dagre');
        
        // Update stats
        updateStats();
        
        hideLoading();
    } catch (error) {
        console.error('Error loading graph data:', error);
        showError('Failed to load graph data: ' + error.message);
    }
}

/**
 * Convert graph data to Cytoscape format
 */
function convertToCytoscapeFormat(data) {
    const elements = [];
    moduleGroups.clear();
    
    // Group nodes by file/module
    data.nodes.forEach((node, index) => {
        const moduleName = getModuleName(node.file);
        if (!moduleGroups.has(moduleName)) {
            moduleGroups.set(moduleName, []);
        }
        moduleGroups.get(moduleName).push(index);
    });
    
    // Add parent nodes for modules with multiple functions
    moduleGroups.forEach((nodeIndices, moduleName) => {
        if (nodeIndices.length > 1 && moduleName !== 'Unknown') {
            elements.push({
                group: 'nodes',
                data: {
                    id: 'module-' + moduleName,
                    label: moduleName + ` (${nodeIndices.length})`,
                    isModule: true,
                    collapsed: collapsedModules.has(moduleName)
                },
                classes: 'module-group'
            });
        }
    });
    
    // Add nodes
    data.nodes.forEach((node, index) => {
        const status = getNodeStatus(node);
        const moduleName = getModuleName(node.file);
        const isInGroup = moduleGroups.get(moduleName)?.length > 1 && moduleName !== 'Unknown';
        const isCollapsed = collapsedModules.has(moduleName);
        
        elements.push({
            group: 'nodes',
            data: {
                id: 'node-' + index,
                label: node.name,
                name: node.name,
                file: node.file,
                line: node.line,
                status: status,
                isStub: node.isStub,
                isMissing: node.isMissing,
                isExternal: node.isExternal,
                parent: isInGroup && !isCollapsed ? 'module-' + moduleName : null
            },
            classes: status + (isCollapsed ? ' hidden' : '')
        });
    });
    
    // Add edges
    data.edges.forEach((edge, index) => {
        // Create edge label with call location if available
        let edgeLabel = '';
        if (edge.line) {
            edgeLabel = `line ${edge.line}`;
        } else if (data.nodes[edge.from]) {
            // If no specific line info, use the source node's line
            edgeLabel = `line ${data.nodes[edge.from].line || '?'}`;
        }
        
        elements.push({
            group: 'edges',
            data: {
                id: 'edge-' + index,
                source: 'node-' + edge.from,
                target: 'node-' + edge.to,
                label: edgeLabel
            }
        });
    });
    
    return elements;
}

/**
 * Get node status based on flags
 */
function getNodeStatus(node) {
    if (node.isMissing) return 'missing';
    if (node.isExternal) return 'external';
    if (node.isStub) return 'stub';
    return 'implemented';
}

/**
 * Extract module name from file path
 */
function getModuleName(filePath) {
    if (!filePath) return 'Unknown';
    // Get just the filename without path
    const parts = filePath.split(/[/\\]/);
    const filename = parts[parts.length - 1];
    // Remove extension
    return filename.replace(/\.[^.]+$/, '');
}

/**
 * Toggle module collapse/expand
 */
function toggleModule(moduleName) {
    if (collapsedModules.has(moduleName)) {
        collapsedModules.delete(moduleName);
    } else {
        collapsedModules.add(moduleName);
    }
    
    // Reload the graph with updated collapse state
    if (graphData) {
        loadGraphData(graphData);
    }
}

/**
 * Apply layout to graph
 */
function applyLayout(layoutName) {
    const layouts = {
        'dagre': {
            name: 'dagre',
            rankDir: 'TB',
            nodeDimensionsIncludeLabels: true,
            animate: true,
            animationDuration: 500,
            fit: true,
            padding: 50
        },
        'breadthfirst': {
            name: 'breadthfirst',
            directed: true,
            animate: true,
            animationDuration: 500,
            fit: true,
            padding: 50
        },
        'circle': {
            name: 'circle',
            animate: true,
            animationDuration: 500,
            fit: true,
            padding: 50
        },
        'concentric': {
            name: 'concentric',
            concentric: function(node) {
                return node.degree();
            },
            levelWidth: function() {
                return 2;
            },
            animate: true,
            animationDuration: 500,
            fit: true,
            padding: 50
        },
        'grid': {
            name: 'grid',
            animate: true,
            animationDuration: 500,
            fit: true,
            padding: 50
        }
    };
    
    const layout = cy.layout(layouts[layoutName] || layouts['dagre']);
    layout.run();
}

/**
 * Filter graph based on search query
 */
function filterGraph(query) {
    // Clear previous search highlights
    cy.elements().removeClass('search-match search-related search-dimmed');
    
    if (!query) {
        // Show all elements normally
        cy.elements().removeClass('hidden');
        cy.elements().style('display', 'element');
    } else {
        const lowerQuery = query.toLowerCase();
        let matchFound = false;
        
        // First, dim all elements
        cy.elements().addClass('search-dimmed');
        
        // Find and highlight matching nodes
        cy.nodes().forEach(node => {
            if (node.data('isModule')) return;  // Skip module groups
            
            const name = (node.data('name') || '').toLowerCase();
            const file = (node.data('file') || '').toLowerCase();
            
            if (name.includes(lowerQuery) || file.includes(lowerQuery)) {
                matchFound = true;
                
                // Highlight the matching node
                node.removeClass('search-dimmed').addClass('search-match');
                
                // Highlight connected edges and nodes
                node.connectedEdges().removeClass('search-dimmed').addClass('search-related');
                node.connectedEdges().connectedNodes().removeClass('search-dimmed').addClass('search-related');
            }
        });
        
        // If no matches found, show a message
        if (!matchFound && query.length > 0) {
            showSearchMessage(`No functions matching "${query}"`);
        } else {
            hideSearchMessage();
        }
    }
    
    updateStats();
}

/**
 * Show search message
 */
function showSearchMessage(message) {
    let searchMsg = document.getElementById('search-message');
    if (!searchMsg) {
        searchMsg = document.createElement('div');
        searchMsg.id = 'search-message';
        searchMsg.className = 'search-message';
        document.querySelector('#controls').appendChild(searchMsg);
    }
    searchMsg.textContent = message;
    searchMsg.style.display = 'block';
}

/**
 * Hide search message
 */
function hideSearchMessage() {
    const searchMsg = document.getElementById('search-message');
    if (searchMsg) {
        searchMsg.style.display = 'none';
    }
}

/**
 * Show node details in sidebar
 */
function showNodeDetails(node) {
    const data = node.data();
    
    document.getElementById('detail-name').textContent = data.name;
    document.getElementById('detail-file').textContent = data.file;
    document.getElementById('detail-line').textContent = data.line;
    document.getElementById('detail-status').textContent = data.status;
    
    // Get callers and callees
    const callers = [];
    const callees = [];
    
    node.connectedEdges().forEach(edge => {
        if (edge.target().id() === node.id()) {
            callers.push(edge.source().data('name'));
        } else {
            callees.push(edge.target().data('name'));
        }
    });
    
    // Update lists
    updateList('detail-calls', callees);
    updateList('detail-callers', callers);
    
    // Show sidebar
    document.getElementById('sidebar').classList.remove('hidden');
}

/**
 * Update a detail list
 */
function updateList(elementId, items) {
    const list = document.getElementById(elementId);
    list.innerHTML = '';
    
    if (items.length === 0) {
        const li = document.createElement('li');
        li.textContent = 'None';
        li.style.color = '#808080';
        list.appendChild(li);
    } else {
        items.forEach(item => {
            const li = document.createElement('li');
            li.textContent = item;
            li.addEventListener('click', () => {
                // Find and select the node
                const targetNode = cy.nodes(`[name="${item}"]`).first();
                if (targetNode) {
                    targetNode.trigger('tap');
                }
            });
            list.appendChild(li);
        });
    }
}

/**
 * Hide sidebar
 */
function hideSidebar() {
    document.getElementById('sidebar').classList.add('hidden');
}

/**
 * Show enhanced tooltip with detailed function information
 */
function showEnhancedTooltip(node, evt) {
    const data = node.data();
    
    // Create tooltip element if it doesn't exist
    let tooltip = document.getElementById('enhanced-tooltip');
    if (!tooltip) {
        tooltip = document.createElement('div');
        tooltip.id = 'enhanced-tooltip';
        tooltip.className = 'enhanced-tooltip';
        document.body.appendChild(tooltip);
    }
    
    // Calculate connections
    let incomingCalls = 0;
    let outgoingCalls = 0;
    node.connectedEdges().forEach(edge => {
        if (edge.target().id() === node.id()) {
            incomingCalls++;
        } else {
            outgoingCalls++;
        }
    });
    
    // Build tooltip content
    let statusIcon = '';
    let statusText = '';
    let statusClass = '';
    
    switch (data.status) {
        case 'implemented':
            statusIcon = '✓';
            statusText = 'Implemented';
            statusClass = 'status-implemented';
            break;
        case 'stub':
            statusIcon = '⚠';
            statusText = 'Stub';
            statusClass = 'status-stub';
            break;
        case 'missing':
            statusIcon = '✗';
            statusText = 'Missing';
            statusClass = 'status-missing';
            break;
        case 'external':
            statusIcon = '⊙';
            statusText = 'External';
            statusClass = 'status-external';
            break;
    }
    
    tooltip.innerHTML = `
        <div class="tooltip-header">
            <span class="function-name">${data.name}</span>
            <span class="status-badge ${statusClass}">${statusIcon} ${statusText}</span>
        </div>
        <div class="tooltip-body">
            <div class="tooltip-row">
                <span class="tooltip-label">File:</span>
                <span class="tooltip-value">${data.file || 'Unknown'}</span>
            </div>
            <div class="tooltip-row">
                <span class="tooltip-label">Line:</span>
                <span class="tooltip-value">${data.line || 'N/A'}</span>
            </div>
            <div class="tooltip-separator"></div>
            <div class="tooltip-row">
                <span class="tooltip-label">Called by:</span>
                <span class="tooltip-value">${incomingCalls} function${incomingCalls !== 1 ? 's' : ''}</span>
            </div>
            <div class="tooltip-row">
                <span class="tooltip-label">Calls:</span>
                <span class="tooltip-value">${outgoingCalls} function${outgoingCalls !== 1 ? 's' : ''}</span>
            </div>
            <div class="tooltip-row">
                <span class="tooltip-label">Total connections:</span>
                <span class="tooltip-value">${incomingCalls + outgoingCalls}</span>
            </div>
        </div>
        <div class="tooltip-footer">
            Click to view details and connections
        </div>
    `;
    
    // Position tooltip near the node
    const renderedPosition = node.renderedPosition();
    const containerOffset = cy.container().getBoundingClientRect();
    
    tooltip.style.left = (containerOffset.left + renderedPosition.x + 20) + 'px';
    tooltip.style.top = (containerOffset.top + renderedPosition.y - 20) + 'px';
    tooltip.style.display = 'block';
    tooltip.style.opacity = '1';
}

/**
 * Hide enhanced tooltip
 */
function hideEnhancedTooltip() {
    const tooltip = document.getElementById('enhanced-tooltip');
    if (tooltip) {
        tooltip.style.opacity = '0';
        setTimeout(() => {
            tooltip.style.display = 'none';
        }, 200);
    }
}

/**
 * Highlight connections for a node
 */
function highlightConnections(node) {
    // Clear previous highlights
    clearHighlights();
    
    // Dim all elements
    cy.elements().addClass('dimmed');
    
    // Highlight selected node and connections
    node.removeClass('dimmed').addClass('highlighted');
    node.connectedEdges().removeClass('dimmed').addClass('highlighted');
    node.connectedEdges().connectedNodes().removeClass('dimmed');
}

/**
 * Clear all highlights
 */
function clearHighlights() {
    cy.elements().removeClass('highlighted dimmed');
}

/**
 * Reset view to initial state
 */
function resetView() {
    document.getElementById('filter-input').value = '';
    filterGraph('');
    clearHighlights();
    hideSidebar();
    cy.fit();
}

/**
 * Update statistics display
 */
function updateStats() {
    const visibleNodes = cy.nodes(':visible');
    const visibleEdges = cy.edges(':visible');
    
    document.getElementById('node-count').textContent = `${visibleNodes.length} functions`;
    document.getElementById('edge-count').textContent = `${visibleEdges.length} calls`;
}

/**
 * Export graph as PNG
 */
function exportPNG() {
    const png = cy.png({
        output: 'blob',
        bg: '#1e1e1e',
        scale: 2,
        full: true
    });
    
    const link = document.createElement('a');
    link.href = URL.createObjectURL(png);
    link.download = 'codemap-graph.png';
    link.click();
}

/**
 * Export graph as JSON
 */
function exportJSON() {
    if (graphData) {
        const json = JSON.stringify(graphData, null, 2);
        const blob = new Blob([json], { type: 'application/json' });
        
        const link = document.createElement('a');
        link.href = URL.createObjectURL(blob);
        link.download = 'codemap-graph.json';
        link.click();
    }
}

/**
 * Show loading indicator
 */
function showLoading() {
    document.getElementById('loading').classList.remove('hidden');
    document.getElementById('error').classList.add('hidden');
}

/**
 * Hide loading indicator
 */
function hideLoading() {
    document.getElementById('loading').classList.add('hidden');
}

/**
 * Show error message
 */
function showError(message) {
    document.getElementById('error-message').textContent = message;
    document.getElementById('error').classList.remove('hidden');
    document.getElementById('loading').classList.add('hidden');
}

/**
 * Show progress indicator
 */
function showProgress(message, percentage) {
    // Could enhance loading indicator with progress bar
    console.log(`Progress: ${message} (${percentage}%)`);
}

/**
 * Load demo data for testing
 */
function loadDemoData() {
    // Check if we're running in demo mode (no C++ backend)
    if (!window.cppBridge) {
        // Create some demo data
        const demoData = {
            nodes: [
                { name: 'main', file: 'main.cpp', line: 10, isStub: false, isMissing: false, isExternal: false },
                { name: 'parseFile', file: 'parser.cpp', line: 25, isStub: false, isMissing: false, isExternal: false },
                { name: 'buildGraph', file: 'graph.cpp', line: 40, isStub: false, isMissing: false, isExternal: false },
                { name: 'exportJSON', file: 'json.cpp', line: 15, isStub: false, isMissing: false, isExternal: false },
                { name: 'TODO_validate', file: 'validator.cpp', line: 5, isStub: true, isMissing: false, isExternal: false },
                { name: 'missingFunc', file: '', line: 0, isStub: false, isMissing: true, isExternal: false },
                { name: 'std::cout', file: '', line: 0, isStub: false, isMissing: false, isExternal: true }
            ],
            edges: [
                { from: 0, to: 1 },
                { from: 0, to: 2 },
                { from: 1, to: 4 },
                { from: 2, to: 3 },
                { from: 2, to: 5 },
                { from: 3, to: 6 }
            ]
        };
        
        // Load demo data after a short delay
        setTimeout(() => {
            loadGraphData(demoData);
        }, 500);
    }
}

/**
 * Settings management
 */
const settings = {
    showEdgeLabels: true,
    showTooltips: true,
    autoGroupModules: true,
    nodeSize: 100,
    edgeWidth: 2,
    fontSize: 12,
    animationEnabled: true,
    highQualityRender: true
};

/**
 * Setup settings input handlers
 */
function setupSettingsInputs() {
    // Range input handlers
    document.getElementById('node-size').addEventListener('input', (e) => {
        document.getElementById('node-size-value').textContent = e.target.value + '%';
    });
    
    document.getElementById('edge-width').addEventListener('input', (e) => {
        document.getElementById('edge-width-value').textContent = e.target.value + 'px';
    });
    
    document.getElementById('font-size').addEventListener('input', (e) => {
        document.getElementById('font-size-value').textContent = e.target.value + 'px';
    });
}

/**
 * Show settings panel
 */
function showSettings() {
    document.getElementById('settings-panel').classList.remove('hidden');
    
    // Load current settings into UI
    document.getElementById('show-edge-labels').checked = settings.showEdgeLabels;
    document.getElementById('show-tooltips').checked = settings.showTooltips;
    document.getElementById('auto-group-modules').checked = settings.autoGroupModules;
    document.getElementById('node-size').value = settings.nodeSize;
    document.getElementById('edge-width').value = settings.edgeWidth;
    document.getElementById('font-size').value = settings.fontSize;
    document.getElementById('animation-enabled').checked = settings.animationEnabled;
    document.getElementById('high-quality-render').checked = settings.highQualityRender;
    
    // Update value displays
    document.getElementById('node-size-value').textContent = settings.nodeSize + '%';
    document.getElementById('edge-width-value').textContent = settings.edgeWidth + 'px';
    document.getElementById('font-size-value').textContent = settings.fontSize + 'px';
}

/**
 * Hide settings panel
 */
function hideSettings() {
    document.getElementById('settings-panel').classList.add('hidden');
}

/**
 * Apply settings changes
 */
function applySettings() {
    // Gather settings from UI
    settings.showEdgeLabels = document.getElementById('show-edge-labels').checked;
    settings.showTooltips = document.getElementById('show-tooltips').checked;
    settings.autoGroupModules = document.getElementById('auto-group-modules').checked;
    settings.nodeSize = parseInt(document.getElementById('node-size').value);
    settings.edgeWidth = parseInt(document.getElementById('edge-width').value);
    settings.fontSize = parseInt(document.getElementById('font-size').value);
    settings.animationEnabled = document.getElementById('animation-enabled').checked;
    settings.highQualityRender = document.getElementById('high-quality-render').checked;
    
    // Apply visual changes
    const nodeSizeScale = settings.nodeSize / 100;
    cy.style()
        .selector('node')
        .style('font-size', settings.fontSize + 'px')
        .style('width', ele => ele.width() * nodeSizeScale)
        .style('height', ele => ele.height() * nodeSizeScale)
        .selector('edge')
        .style('width', settings.edgeWidth)
        .style('font-size', (settings.fontSize - 2) + 'px')
        .style('label', settings.showEdgeLabels ? 'data(label)' : '')
        .update();
    
    // Toggle tooltips
    if (!settings.showTooltips) {
        cy.off('mouseover', 'node');
        cy.off('mouseout', 'node');
    } else {
        cy.on('mouseover', 'node', function(evt) {
            const node = evt.target;
            if (!node.data('isModule')) {
                showEnhancedTooltip(node, evt);
            }
        });
        cy.on('mouseout', 'node', function(evt) {
            hideEnhancedTooltip();
        });
    }
    
    // Save settings to localStorage
    localStorage.setItem('codemapSettings', JSON.stringify(settings));
    
    hideSettings();
}

/**
 * Reset settings to defaults
 */
function resetSettings() {
    settings.showEdgeLabels = true;
    settings.showTooltips = true;
    settings.autoGroupModules = true;
    settings.nodeSize = 100;
    settings.edgeWidth = 2;
    settings.fontSize = 12;
    settings.animationEnabled = true;
    settings.highQualityRender = true;
    
    // Update UI
    showSettings();
}

// Load settings from localStorage on startup
window.addEventListener('DOMContentLoaded', () => {
    const savedSettings = localStorage.getItem('codemapSettings');
    if (savedSettings) {
        Object.assign(settings, JSON.parse(savedSettings));
    }
});