# Development Plan – Contract-First Workflow

## 📚 Related Documentation

- **[`README.md`](README.md)** - Project overview and methodology
- **[`FILES.md`](FILES.md)** - File structure and organization
- **[`context.md`](context.md)** - Current project state tracking
- **[`SETUP.md`](SETUP.md)** - Initial setup instructions

## Core Principles

This workflow enforces:
1. **Immutable Contracts** - Interfaces cannot change once protected
2. **Test-Driven Development** - Tests written before implementation
3. **Clear Boundaries** - Only implementation files can be modified
4. **Automated Protection** - GitHub Actions enforce the rules

## Workflow (strict)

### Working Directories
- **Project Root** → Permanent project files (pushed to GitHub)
- **Scratch/Temp** → Temporary work files (never committed)

### Step Transition Protocol
When progressing between workflow steps, always announce:
1. **Previous Step Completed** - What was just finished
2. **Current Step Starting** - What's being worked on now
3. **Next Step Preview** - What comes next

### Critical Dependency Rule
**NEVER alter the development plan because something isn't installed.** 
- STOP immediately if dependencies are missing
- Ask for installation help
- Wait for confirmation before proceeding
- The plan must be followed exactly

## The 10-Step Development Cycle

### 1. **Add/Change Contracts and Pseudocode**
- Contracts live in headers (`include/*.h`)
- Mark contract headers with:
  ```cpp
  // PROTECTED CONTRACT: Do not edit except with explicit approval
  ```
- From this point, the header is locked
- Define clear interfaces before implementation

### 2. **Create Tests for Contracts**
- Tests live in `tests/test_*.cpp`
- Write comprehensive tests against the contracts
- Mark test files with:
  ```cpp
  // PROTECTED TEST: Do not modify after marking as protected
  ```
- Tests become immutable once marked
- Provide test runner scripts (`run_tests.sh`, `run_tests.bat`)

### 3. **Add/Change Code**
- Implement in `src/*.cpp`
- Can include private helper headers (not protected)
- Never alter protected contracts or tests
- Focus on making tests pass

### 4. **Run Tests**
```bash
# Run your test suite
./run_tests.sh        # Linux/Mac
run_tests.bat         # Windows
# Or use your build system's test command
cmake --build . --target test
```

### 5. **Address Errors**
- Fix implementation files only
- Modify helpers and internals as needed
- Cannot change contracts or tests to "fix" errors
- Errors reveal implementation issues, not contract problems

### 6. **Iterate Steps 1-5**
- Repeat until all tests pass
- Each iteration improves implementation
- Contracts remain stable throughout

### 7. **Push to GitHub**
- Commit your changes
- Push to repository
- GitHub Actions will verify protection rules
- Failed protection checks block merging

### 8. **Release/Deploy** (Optional)
- Tag versions appropriately
- Build release artifacts
- Deploy according to project needs

### 9. **Clear Scratch/Temp**
- Clean temporary work directories
- Remove build artifacts not needed
- Keep repository clean

### 10. **Update Documentation**
- Update `FILES.md` with any new files
- Update `context.md` with current state
- Ensure README reflects current features
- Cross-reference documentation accurately

## GitHub Actions Contract Enforcement

### How Protection Works
1. On every push and pull request
2. Actions check for `PROTECTED CONTRACT` and `PROTECTED TEST` markers
3. If protected files are modified, the build fails
4. Merging is blocked until protection violations are resolved

### Protection Workflow File
The `.github/workflows/protect-contracts.yml` file:
- Runs on all pushes and PRs
- Checks diffs for protected file modifications
- Reports violations clearly
- Ensures architectural integrity
- Can be temporarily disabled via `ALLOW_PROTECTED_EDITS` secret

### Protection Override Mechanism
For exceptional cases where protected files must be modified:

1. **Add Repository Secret**:
   - Go to Settings → Secrets → Actions
   - Add `ALLOW_PROTECTED_EDITS` with value `true`
   
2. **Make Changes**:
   - Protection checks will be skipped
   - Warning messages will appear in Actions logs
   - All changes are still tracked in git history
   
3. **Re-enable Protection**:
   - **CRITICAL**: Remove or change the secret immediately after merging
   - Protection automatically resumes when secret ≠ `true`

⚠️ **Warning**: This override should only be used for:
- Fixing critical contract design flaws
- Major refactoring with team approval
- Emergency patches that require interface changes

## Architecture Guidelines

### Contract Design
- **Interfaces First**: Define behavior before implementation
- **Clear Boundaries**: Separate what from how
- **Minimal Surface**: Keep interfaces small and focused
- **Future-Proof**: Design for extension, not modification

### Test Strategy
- **Unit Tests**: Test individual components
- **Integration Tests**: Test component interactions
- **Contract Tests**: Verify interface compliance
- **Edge Cases**: Test boundary conditions

### Implementation Approach
- **Single Responsibility**: Each class/function does one thing
- **Dependency Injection**: Pass dependencies explicitly
- **Error Handling**: Fail gracefully with clear messages
- **Documentation**: Comment why, not what

## Working with AI Assistants

### Effective AI Collaboration
1. **Provide Context**: Share `context.md` at session start
2. **State Current Step**: Be explicit about workflow position
3. **Enforce Boundaries**: Remind AI about protected files
4. **Review Changes**: Verify AI respects protection rules

### Example AI Instructions
```
We're using contract-first development.
Current step: Step 3 (implementing code)
Protected files: include/*.h and tests/*.cpp cannot be modified
Task: Implement the service defined in include/my_service.h
Make all tests in tests/test_my_service.cpp pass
```

## Common Patterns

### Adding a New Feature
1. Design the contract (new header or extend existing)
2. Write comprehensive tests
3. Mark both as protected
4. Implement until tests pass
5. Document the feature

### Fixing a Bug
1. Write a failing test that exposes the bug
2. Mark test as protected
3. Fix implementation until test passes
4. Verify no regression in other tests

### Refactoring
1. Ensure comprehensive test coverage exists
2. Mark tests as protected if not already
3. Refactor implementation freely
4. All tests must still pass

## Troubleshooting

### Protected File Violation
- **Problem**: GitHub Actions blocking your PR
- **Solution**: Revert changes to protected files
- **Prevention**: Only modify files in `src/`

### Contract Needs Change
- **Problem**: Protected contract has a design flaw
- **Solution**: Use the protection override mechanism
- **Process**: 
  1. Document the need for change
  2. Get team approval
  3. Set `ALLOW_PROTECTED_EDITS` secret to `true`
  4. Make necessary changes
  5. Remove or change the secret immediately after

### Test Failures After Protection
- **Problem**: Tests are failing but can't be modified
- **Solution**: Fix the implementation, not the test
- **Note**: Failing protected tests indicate implementation bugs

## Best Practices

1. **Protect Early**: Lock contracts before extensive implementation
2. **Test Thoroughly**: Comprehensive tests prevent future issues
3. **Document Decisions**: Record why contracts are designed as they are
4. **Version Carefully**: Use semantic versioning for contract changes
5. **Communicate Changes**: Announce any contract modifications clearly

## Summary

This workflow ensures:
- **Stability**: Contracts don't change unexpectedly
- **Quality**: Tests drive correct implementation
- **Clarity**: Clear boundaries between interface and implementation
- **Automation**: Rules enforced without manual review
- **Scalability**: Works for small and large projects

Follow these steps strictly for reliable, maintainable software development with AI assistance.