# Project File Index

## 📚 Related Documentation

Before making any changes, read these documents in order:
1. **[`context.md`](context.md)** - Current project state and requirements
2. **[`README.md`](README.md)** - Project overview and methodology
3. **[`DEVELOPMENT_PLAN.md`](DEVELOPMENT_PLAN.md)** - Development workflow
4. **[`FILES.md`](FILES.md)** - This file: complete file structure

## Project Structure

### Header Files (Protected Contracts)
Location: `include/`

Protected headers contain the immutable contracts that define your project's interfaces. Once marked with `// PROTECTED CONTRACT`, these files cannot be modified.

**Example Structure:**
- `include/[feature].h` - Interface definitions
- `include/[data_types].h` - Core data structures
- `include/[service].h` - Service interfaces

**Template Available:**
- `include/example_contract.h.template` - Example showing the pattern

### Implementation Files (Modifiable)
Location: `src/`

Implementation files contain the actual code that fulfills the contracts. These are the only files that can be modified after protection is enabled.

**Example Structure:**
- `src/[feature].cpp` - Feature implementation
- `src/[service].cpp` - Service implementation
- `src/[helpers].cpp` - Utility functions

**Template Available:**
- `src/example_implementation.cpp.template` - Example implementation

### Test Files (Protected Tests)
Location: `tests/`

Test files verify that implementations correctly fulfill the contracts. Once marked with `// PROTECTED TEST`, these files ensure implementation quality.

**Example Structure:**
- `tests/test_[feature].cpp` - Feature tests
- `tests/test_[service].cpp` - Service tests
- `tests/test_integration.cpp` - Integration tests

**Template Available:**
- `tests/test_example.cpp.template` - Example test structure

### Build Configuration
Location: Project root

- `CMakeLists.txt` - CMake build configuration
- `run_tests.sh` - Linux/Mac test runner script
- `run_tests.bat` - Windows test runner script
- `Makefile` (optional) - Alternative build system

### GitHub Actions
Location: `.github/workflows/`

- `protect-contracts.yml` - Enforces protection rules
- `build.yml` (optional) - Automated builds
- `test.yml` (optional) - Automated testing

### Documentation
Location: Project root

- **[`README.md`](README.md)**  
  Main project documentation  
  Purpose: Overview, features, usage instructions

- **[`DEVELOPMENT_PLAN.md`](DEVELOPMENT_PLAN.md)**  
  Workflow and methodology guide  
  Purpose: Step-by-step development process

- **[`context.md`](context.md)**  
  Project state tracking  
  Purpose: Current status, objectives, progress

- **[`FILES.md`](FILES.md)** (This file)  
  File structure documentation  
  Purpose: Understanding project organization

- **[`SETUP.md`](SETUP.md)**  
  Initial configuration guide  
  Purpose: Getting started with the template

## Directory Layout

```
your-project/
├── include/                      # Protected contract headers
│   ├── example_contract.h.template  # Template to copy
│   └── *.h                      # Your interface files
│
├── src/                         # Modifiable implementations
│   ├── example_implementation.cpp.template  # Template
│   └── *.cpp                    # Your implementation files
│
├── tests/                       # Protected test suites
│   ├── test_example.cpp.template   # Template
│   └── test_*.cpp               # Your test files
│
├── build/                       # Build output (git-ignored)
│   └── [build artifacts]
│
├── .github/
│   └── workflows/
│       ├── protect-contracts.yml   # Contract protection
│       └── [other workflows]
│
├── docs/                        # Additional documentation (optional)
│   └── [design docs, API docs, etc.]
│
├── scripts/                     # Utility scripts (optional)
│   └── [helper scripts]
│
├── CMakeLists.txt              # Build configuration
├── run_tests.sh                # Test runner (Linux/Mac)
├── run_tests.bat               # Test runner (Windows)
├── .gitignore                  # Git ignore patterns
├── README.md                   # Project overview
├── DEVELOPMENT_PLAN.md         # Workflow guide
├── FILES.md                    # This file
├── context.md                  # Project state
└── SETUP.md                    # Setup instructions
```

## File Naming Conventions

### Headers (include/)
- `i[name].h` - Interfaces (e.g., `iservice.h`)
- `[name]_types.h` - Type definitions
- `[name].h` - Class declarations

### Source (src/)
- `[name].cpp` - Implementation files
- `[name]_impl.cpp` - Alternative naming
- `[name]_helpers.cpp` - Utility functions

### Tests (tests/)
- `test_[name].cpp` - Unit tests
- `test_integration_[name].cpp` - Integration tests
- `benchmark_[name].cpp` - Performance tests

## Protection Markers

### Contract Protection
```cpp
// PROTECTED CONTRACT: Do not edit except with explicit approval
```

### Test Protection
```cpp
// PROTECTED TEST: Do not modify after marking as protected
```

## Working with Templates

### Creating a New Contract
```bash
cp include/example_contract.h.template include/my_feature.h
# Edit my_feature.h to define your interface
# Add protection marker when ready
```

### Creating a New Test
```bash
cp tests/test_example.cpp.template tests/test_my_feature.cpp
# Edit test_my_feature.cpp to test your contract
# Add protection marker when complete
```

### Creating Implementation
```bash
cp src/example_implementation.cpp.template src/my_feature.cpp
# Edit my_feature.cpp to implement the contract
# This file remains modifiable
```

## Dependencies

Document your project's dependencies here:

### Build Dependencies
- CMake 3.10+ (or your build system)
- C++17 compiler (or your language/version)
- [Other build requirements]

### Runtime Dependencies
- [Library 1] - Purpose
- [Library 2] - Purpose

### Development Dependencies
- Testing framework (if not built-in)
- Documentation generator (optional)
- Code formatter (optional)

## Notes

- Only files in `src/` can be modified after protection
- Protected files require explicit approval to change
- Templates are provided as `.template` files
- Build output should be git-ignored
- Keep documentation up-to-date with changes