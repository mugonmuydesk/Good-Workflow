cmake_minimum_required(VERSION 3.10)
project(YourProjectName VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create library from sources (if building a library)
# add_library(${PROJECT_NAME}_lib ${SOURCES} ${HEADERS})

# Create executable (if building an application)
# add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} ${HEADERS})

# Find and link dependencies
# Example for common libraries:
# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} Threads::Threads)

# Enable testing
enable_testing()

# Add test executables
file(GLOB TEST_SOURCES "tests/test_*.cpp")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${SOURCES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    
    # Link test dependencies if needed
    # target_link_libraries(${TEST_NAME} ${PROJECT_NAME}_lib)
endforeach()

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_SOURCES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests..."
)

# Installation rules (optional)
# install(TARGETS ${PROJECT_NAME}
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )
# install(DIRECTORY include/ DESTINATION include)

# Package configuration (optional)
# include(CPack)
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})