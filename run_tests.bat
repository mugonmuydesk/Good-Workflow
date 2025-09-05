@echo off
echo =======================================
echo       CodeMap Test Suite
echo =======================================
echo.

REM Set build directory
set BUILD_DIR=build

REM Check if build directory exists
if not exist "%BUILD_DIR%" (
    echo Creating build directory...
    mkdir "%BUILD_DIR%"
)

REM Navigate to build directory
cd "%BUILD_DIR%"

REM Configure with CMake if not already done
if not exist "CMakeCache.txt" (
    echo Configuring project with CMake...
    cmake ..
    if errorlevel 1 (
        echo CMake configuration failed!
        cd ..
        exit /b 1
    )
)

REM Build the tests
echo.
echo Building tests...
cmake --build . --config Debug
if errorlevel 1 (
    echo Build failed!
    cd ..
    exit /b 1
)

echo.
echo =======================================
echo         Running Tests
echo =======================================
echo.

REM Run test_codemap_types
echo Running test_codemap_types...
echo -------------------------------
tests\Debug\test_codemap_types.exe
if errorlevel 1 (
    echo test_codemap_types FAILED!
    set TEST_FAILED=1
) else (
    echo test_codemap_types PASSED!
)

echo.

REM Run test_parser
echo Running test_parser...
echo -------------------------------
tests\Debug\test_parser.exe
if errorlevel 1 (
    echo test_parser FAILED!
    set TEST_FAILED=1
) else (
    echo test_parser PASSED!
)

echo.

REM Run test_json_exporter
echo Running test_json_exporter...
echo -------------------------------
tests\Debug\test_json_exporter.exe
if errorlevel 1 (
    echo test_json_exporter FAILED!
    set TEST_FAILED=1
) else (
    echo test_json_exporter PASSED!
)

echo.

REM Run test_graph_builder
echo Running test_graph_builder...
echo -------------------------------
tests\Debug\test_graph_builder.exe
if errorlevel 1 (
    echo test_graph_builder FAILED!
    set TEST_FAILED=1
) else (
    echo test_graph_builder PASSED!
)

echo.
echo =======================================

REM Check if any test failed
if defined TEST_FAILED (
    echo         SOME TESTS FAILED!
    echo =======================================
    cd ..
    exit /b 1
) else (
    echo         ALL TESTS PASSED!
    echo =======================================
    cd ..
    exit /b 0
)