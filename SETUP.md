# Setup Guide - Contract-First Development Workflow

This guide will help you configure this template for your specific project.

## 🚀 Quick Start

### 1. Use This Template

If viewing on GitHub:
- Click "Use this template" button
- Create a new repository
- Clone your new repository

If using locally:
```bash
# Clone this template
git clone https://github.com/mugonmuydesk/Good-Workflow.git YourProjectName
cd YourProjectName

# Remove the template's git history
rm -rf .git

# Initialize your own repository
git init
git add .
git commit -m "Initial commit from contract-first template"
```

### 2. Configure Your Project

#### Update Project Name
Replace placeholder names in these files:
- `CMakeLists.txt` - Change `YourProjectName` to your actual project name
- `README.md` - Update badge URLs with your GitHub username/repo

#### Update Documentation
1. Edit `context.md`:
   - Fill in your project name and description
   - Define initial objectives
   - Set up phase structure

2. Update `README.md`:
   - Replace `[your-username]/[your-repo]` with your GitHub details
   - Customize the project description

### 3. Create Your First Contract

```bash
# Copy the template
cp include/example_contract.h.template include/my_first_feature.h

# Edit the file to define your interface
# Add the protection marker when ready:
# // PROTECTED CONTRACT: Do not edit except with explicit approval
```

### 4. Write Your First Test

```bash
# Copy the test template
cp tests/test_example.cpp.template tests/test_my_first_feature.cpp

# Write tests for your contract
# Add the protection marker when complete:
# // PROTECTED TEST: Do not modify after marking as protected
```

### 5. Implement Your Code

```bash
# Copy the implementation template
cp src/example_implementation.cpp.template src/my_first_feature.cpp

# Implement the contract
# This file remains modifiable
```

## 📦 Language-Specific Setup

### For C++ Projects (Default)

The template comes configured for C++. Just:
1. Ensure CMake 3.10+ is installed
2. Ensure a C++17 compiler is available
3. Build with:
```bash
mkdir build && cd build
cmake ..
make
```

### For Python Projects

1. Remove C++ specific files:
```bash
rm -rf CMakeLists.txt run_tests.bat run_tests.sh
rm -rf include/*.template src/*.template tests/*.template
```

2. Create Python structure:
```bash
# Create Python package structure
mkdir -p my_package/contracts
mkdir -p my_package/implementations
mkdir -p tests

# Create __init__.py files
touch my_package/__init__.py
touch my_package/contracts/__init__.py
touch my_package/implementations/__init__.py
```

3. Create Python contract example:
```python
# my_package/contracts/example_contract.py
# PROTECTED CONTRACT: Do not edit except with explicit approval

from abc import ABC, abstractmethod

class IExampleService(ABC):
    @abstractmethod
    def process(self, data: dict) -> bool:
        """Process the provided data."""
        pass
    
    @abstractmethod
    def get_results(self) -> list:
        """Return processed results."""
        pass
```

4. Update test runner:
```bash
# Create run_tests.py
cat > run_tests.py << 'EOF'
#!/usr/bin/env python3
import unittest
import sys

if __name__ == '__main__':
    loader = unittest.TestLoader()
    suite = loader.discover('tests', pattern='test_*.py')
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    sys.exit(0 if result.wasSuccessful() else 1)
EOF

chmod +x run_tests.py
```

### For Other Languages

1. Remove C++ specific files
2. Create appropriate directory structure
3. Adapt the contract/test/implementation pattern
4. Update build configuration
5. Ensure GitHub Actions still work

## 🔧 Build System Configuration

### CMake (C++)
Already configured in `CMakeLists.txt`. Customize:
- Project name
- Dependencies
- Compiler flags
- Installation rules

### Make
Create a `Makefile`:
```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include
SOURCES = $(wildcard src/*.cpp)
TESTS = $(wildcard tests/test_*.cpp)

all: build

build:
    $(CXX) $(CXXFLAGS) $(SOURCES) -o myapp

test:
    @for test in $(TESTS); do \
        $(CXX) $(CXXFLAGS) $$test $(SOURCES) -o test_binary && \
        ./test_binary || exit 1; \
    done

clean:
    rm -f myapp test_binary
```

### Other Build Systems
- **Gradle/Maven**: For Java projects
- **Cargo**: For Rust projects
- **npm/yarn**: For JavaScript/TypeScript
- **Poetry/setuptools**: For Python

## 🤖 GitHub Actions Setup

The template includes `protect-contracts.yml` which enforces the workflow rules.

### Customize Protection Rules

Edit `.github/workflows/protect-contracts.yml` if you need to:
- Change protected directories
- Modify protection markers
- Add additional checks

### Add Build Automation

Create `.github/workflows/build.yml`:
```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make
      - name: Test
        run: |
          cd build
          ctest --output-on-failure
```

## 📝 Documentation Setup

### Required Documentation
Ensure these files are kept updated:
- `context.md` - Current project state
- `README.md` - User-facing documentation
- `FILES.md` - File structure reference
- `DEVELOPMENT_PLAN.md` - Workflow guide

### Optional Documentation
Consider adding:
- `docs/API.md` - API documentation
- `docs/DESIGN.md` - Design decisions
- `docs/CONTRIBUTING.md` - Contribution guidelines
- `CHANGELOG.md` - Version history

## 🎯 Best Practices

### Starting a New Feature
1. Design the contract first
2. Write comprehensive tests
3. Mark both as protected
4. Implement until tests pass
5. Update documentation

### Working with AI
When using AI assistants:
```
I'm starting a new project using the contract-first workflow.
Here's my context.md: [paste content]
I'm at Step 1: defining contracts.
Please help me design the interface for [feature].
```

### Team Collaboration
1. Share this SETUP.md with team members
2. Ensure everyone understands protection rules
3. Use pull requests for all changes
4. Let GitHub Actions enforce workflow

## 🚨 Common Issues

### "Protected file modified" error
- Only modify files in `src/`
- Contracts and tests are immutable once protected
- If change is essential, remove protection temporarily with team approval

### Tests not found
- Ensure test files match pattern `test_*.cpp` (or your language's convention)
- Check test runner script has correct paths
- Verify CMakeLists.txt includes test configuration

### Build failures
- Check all dependencies are installed
- Verify compiler/interpreter version
- Ensure build system is properly configured

## 📚 Additional Resources

- [Contract-First Development](https://en.wikipedia.org/wiki/Design_by_contract)
- [Test-Driven Development](https://en.wikipedia.org/wiki/Test-driven_development)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)

## 🎉 Ready to Go!

You're now set up with a robust, AI-friendly development workflow that:
- Protects your contracts and tests
- Enforces quality through automation
- Provides clear boundaries for development
- Scales from small to large projects

Start by defining your first contract in `include/` and follow the workflow in `DEVELOPMENT_PLAN.md`!

---

*Questions? Check the README.md or create an issue in the template repository.*