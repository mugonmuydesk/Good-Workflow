# Project File Index

## 📚 Related Documentation

Before making any changes, read these documents in order:
1. **[`context.md`](/mnt/c/dev/CodeMap/Repo/context.md)** - MANDATORY: Lists all required reading
2. **[`README.md`](/mnt/c/dev/CodeMap/Repo/README.md)** - Project overview
3. **[`DEVELOPMENT_PLAN.md`](/mnt/c/dev/CodeMap/Repo/DEVELOPMENT_PLAN.md)** - Development workflow
4. **[`FILES.md`](/mnt/c/dev/CodeMap/Repo/FILES.md)** - This file: complete file index

## Header Files (Protected Contracts)

- `include/codemap_types.h`  
  Core data structures for function graph representation.  
  Defines: `FunctionNode`, `FunctionGraph`, `CallInfo`, `NodeStatus`.  
  Used by: All other headers and implementation files.

- `include/parser.h`  
  Parser interface and C++ parser class declaration.  
  Defines: `IParser` (interface), `CppParser` (concrete implementation).  
  Dependencies: `codemap_types.h`.  
  Used by: `graph_builder.h`, future parser implementations.

- `include/graph_builder.h`  
  Orchestrates parsing and graph construction from source projects.  
  Defines: `GraphBuilder`.  
  Dependencies: `codemap_types.h`, `parser.h`.  
  Used by: Main application (future).

- `include/json_exporter.h`  
  JSON serialization and deserialization for function graphs.  
  Defines: `JSONExporter`.  
  Dependencies: `codemap_types.h`.  
  Used by: `graph_builder.h`, frontend visualization (future).

- `include/webview_app.h` *(Created in Phase 4 - PROTECTED)*  
  WebviewApp interface for frontend visualization.  
  Defines: `WebviewApp`, `IWebviewBridge`.  
  Dependencies: `codemap_types.h`, `graph_builder.h`.  
  Used by: Main application.

## Implementation Files

- `src/codemap_types.cpp`  
  Implementation of core data structure methods and utilities.  
  Implements: `FunctionGraph` methods (addNode, addCall, findNodeByName, etc.).  
  Dependencies: `codemap_types.h`.

- `src/json_exporter.cpp`  
  JSON export/import functionality for function graphs.  
  Implements: `JSONExporter::exportGraph`, `JSONExporter::importGraph`.  
  Dependencies: `json_exporter.h`, `codemap_types.h`.

- `src/parser.cpp` *(Implemented in Phase 2)*  
  C++ parser implementation using libclang for function extraction.  
  Implements: `CppParser` methods (parseFile, parseProject, getSupportedExtensions).  
  Dependencies: `parser.h`, `codemap_types.h`, libclang-18.  
  Features: Function detection, call graph extraction, stub/missing/external detection.

- `src/graph_builder.cpp` *(Implemented in Phase 3)*  
  Graph construction and analysis from parsed source files.  
  Implements: `GraphBuilder` methods (buildFromProject, toJSON, loadFromJSON, file I/O).  
  Dependencies: `graph_builder.h`, `parser.h`, `json_exporter.h`.  
  Features: Missing function resolution, stub detection, graph validation.

- `src/webview_app_simple.cpp` *(Implemented in Phase 4)*  
  Simplified webview application implementation.  
  Implements: `WebviewApp` methods without native webview dependency.  
  Dependencies: `webview_app.h`, `json_exporter.h`, filesystem.  
  Features: Graph export to JSON, HTML visualization instructions.

- `src/main.cpp` *(Implemented in Phase 4)*  
  Main application entry point.  
  Features: Command-line interface, demo mode, project analysis, JSON loading.  
  Dependencies: `webview_app.h`, `graph_builder.h`, `json_exporter.h`.

## Test Files (Protected Tests)

- `tests/test_codemap_types.cpp`  
  Comprehensive unit tests for core data structures.  
  Tests: FunctionNode creation, FunctionGraph operations, call relationships.  
  Dependencies: `codemap_types.h`.

- `tests/test_json_exporter.cpp`  
  Tests for JSON serialization and deserialization.  
  Tests: Export format, import validation, round-trip consistency.  
  Dependencies: `json_exporter.h`, `codemap_types.h`.

- `tests/test_parser.cpp` *(Created in Phase 2 - PROTECTED)*  
  Comprehensive tests for C++ parser functionality.  
  Tests: Function detection, call extraction, file/project parsing, edge cases.  
  Dependencies: `parser.h`, `codemap_types.h`, `json_exporter.h`.  
  Test count: 12 test functions covering all parser methods.

- `tests/test_graph_builder.cpp` *(Created in Phase 3 - PROTECTED)*  
  Comprehensive tests for GraphBuilder functionality.  
  Tests: Graph construction, JSON serialization, file I/O, missing/stub detection.  
  Dependencies: `graph_builder.h`, `json_exporter.h`, `codemap_types.h`.  
  Test count: 15 test functions covering graph building and analysis.

## Build Configuration

- `CMakeLists.txt`  
  CMake build configuration for the project.  
  Defines: Build targets, test executables, compiler settings.  
  Creates: `codemap_core` library, test executables.

- `run_tests.bat`  
  Windows batch script for building and running all tests.  
  Functions: Clean build, compile tests, execute test suite.  
  Platform: Windows/cmd.exe.

- `run_tests.sh`  
  Linux/WSL shell script for building and running all tests.  
  Functions: Clean build, compile tests, execute test suite.  
  Platform: Linux/WSL/bash.

## Frontend Files (Phase 4)

- `frontend/index.html`  
  Main HTML page for the visualization interface.  
  Features: Controls toolbar, graph container, sidebar, legend.  
  Dependencies: Cytoscape.js (CDN), styles.css, app.js.

- `frontend/styles.css`  
  Dark theme styling for the visualization interface.  
  Features: Modern dark theme, responsive layout, interactive elements.  
  Components: Header controls, graph container, sidebar, legend.

- `frontend/app.js`  
  JavaScript application for interactive graph visualization.  
  Dependencies: Cytoscape.js library.  
  Features: Graph rendering, layouts, filtering, node selection, export.

## Documentation

- **[`README.md`](/mnt/c/dev/CodeMap/Repo/README.md)**  
  Main project documentation with features, build instructions, and usage.  
  Contains: Project overview, build steps, architecture, contribution guidelines.  
  References: Links to DEVELOPMENT_PLAN.md, FILES.md, context.md.  
  Audience: Users and contributors.

- **[`DEVELOPMENT_PLAN.md`](/mnt/c/dev/CodeMap/Repo/DEVELOPMENT_PLAN.md)**  
  Detailed development workflow and implementation phases.  
  Contains: Strict workflow rules, architecture design, phase breakdown.  
  References: Links to README.md, FILES.md, context.md.  
  Audience: Developers implementing features.

- **[`context.md`](/mnt/c/dev/CodeMap/Repo/context.md)** *(MANDATORY STARTING POINT)*  
  Current project state and progress tracking.  
  Contains: Required reading list, completed phases, current work, protection status.  
  References: Links to all other documentation with mandatory reading notice.  
  Purpose: Recovery context for development continuity.

- **[`FILES.md`](/mnt/c/dev/CodeMap/Repo/FILES.md)** *(This file)*  
  Complete index of all project files and their purposes.  
  Contains: File descriptions, key definitions, dependencies, cross-references.  
  References: Links to all documentation files.  
  Purpose: Quick reference for project structure.

## GitHub Actions

- `.github/workflows/protect-contracts.yml`  
  Automated CI/CD workflow for contract protection.  
  Functions: Checks for PROTECTED markers, blocks modifications.  
  Triggers: Push events, pull requests.

- `.github/workflows/build-release.yml`  
  Automated multi-platform build and release workflow.  
  Functions: Builds Windows and Linux binaries, creates GitHub releases.  
  Triggers: Version tags (v*), manual dispatch.  
  Platforms: Windows (MSVC + LLVM), Linux (GCC + libclang).

## Configuration Files

- `.gitignore`  
  Git ignore patterns for build artifacts and temporary files.  
  Excludes: build/, *.exe, *.o, temp files, IDE configs.  
  Purpose: Keep repository clean of generated files.

## Directory Structure

- `include/` - Protected contract headers (immutable interfaces)
- `src/` - Implementation files (modifiable)
- `tests/` - Protected test files (immutable tests)
- `build/` - Build output directory (not in repo)
- `docs/` - Additional documentation (future)
- `frontend/` - Web UI visualization (future Phase 4)
- `.github/` - GitHub Actions workflows

## External Dependencies

- **CMake 3.10+** - Build system
- **C++17 compiler** - Language standard
- **libclang** - C++ parsing (Phase 2)
- **Webview** - UI framework (future Phase 4)
- **Cytoscape.js/vis.js** - Graph visualization (future Phase 4)