# AI-Driven Contract-First Development Workflow

[![Protected Contracts](https://github.com/[your-username]/[your-repo]/actions/workflows/protect-contracts.yml/badge.svg)](https://github.com/[your-username]/[your-repo]/actions)

*A proven workflow template for AI-assisted software development with immutable contracts and test-driven implementation.*

## 🎯 What is This?

This repository provides a **battle-tested workflow** for developing software with AI assistance (like Claude, GPT-4, etc.) that ensures:

- **Contracts stay stable**: Once defined, interfaces cannot be accidentally modified
- **Tests drive development**: Tests are written against contracts before implementation
- **AI stays focused**: Clear boundaries prevent scope creep and maintain consistency
- **Quality is enforced**: GitHub Actions automatically protect critical code

## ✨ Key Features

### Contract-First Development
- Define interfaces and data structures first
- Mark them as `PROTECTED CONTRACT` to lock them
- AI and humans can only modify implementations, not contracts

### Test-Driven Implementation
- Write tests against contracts before implementing
- Tests become `PROTECTED TEST` once complete
- Implementation must satisfy existing tests

### GitHub Actions Protection
- Automatically blocks PRs that modify protected files
- Ensures contracts and tests remain immutable
- Maintains architectural integrity

### Clear Workflow Steps
- Structured 10-step development cycle
- Explicit handoff points between planning and coding
- Built-in documentation requirements

## 🚀 Getting Started

### Quick Setup

1. **Use this template** to create your new repository
2. Clone your new repository
3. Read the core documentation:
   - `SETUP.md` - How to configure for your project
   - `DEVELOPMENT_PLAN.md` - The development workflow
   - `context.md` - Track your project state

### First Steps

1. **Define your project** in `context.md`
2. **Create your first contract** using the example template:
   ```bash
   cp include/example_contract.h.template include/my_feature.h
   ```
3. **Mark it as protected** by adding the marker comment
4. **Write tests** using the test template
5. **Implement** in the `src/` directory
6. **Run tests** to verify your implementation

## 📁 Repository Structure

```
your-project/
├── include/                # Contract headers (PROTECTED)
│   └── *.h                # Your interfaces and data structures
├── src/                   # Implementation files (MODIFIABLE)
│   └── *.cpp             # Your actual code
├── tests/                 # Test files (PROTECTED)
│   └── test_*.cpp        # Your test suites
├── .github/               
│   └── workflows/
│       └── protect-contracts.yml  # Automated protection
├── DEVELOPMENT_PLAN.md    # The workflow process
├── context.md            # Current project state
├── FILES.md              # File index and structure
└── SETUP.md              # Setup instructions
```

## 🔒 Protection Mechanism

### How It Works

1. Add `// PROTECTED CONTRACT` to header files you want to lock
2. Add `// PROTECTED TEST` to test files you want to lock
3. GitHub Actions will automatically:
   - Check all commits and PRs
   - Block changes to protected files
   - Ensure only `src/` files can be modified

### Emergency Override

In exceptional cases, maintainers can temporarily disable protection:
- Set repository secret `ALLOW_PROTECTED_EDITS` to `true`
- Make necessary contract/test changes
- Remove the secret to re-enable protection
- See `docs/PROTECTION_OVERRIDE.md` for detailed instructions

### Example Protected Contract

```cpp
// PROTECTED CONTRACT: Do not edit except with explicit approval
#ifndef MY_INTERFACE_H
#define MY_INTERFACE_H

class IMyService {
public:
    virtual bool process(const Data& input) = 0;
    virtual Result getResult() const = 0;
};

#endif
```

## 🤖 Working with AI Assistants

This workflow is optimized for AI pair programming:

### Best Practices

1. **Share the context**: Always provide `context.md` to the AI
2. **Reference the plan**: Point to `DEVELOPMENT_PLAN.md` for workflow
3. **Be explicit about phases**: Tell the AI which step you're on
4. **Protect early**: Lock contracts before extensive implementation
5. **Test first**: Have AI write tests before implementation

### Example AI Prompt

```
I'm using the contract-first development workflow. 
Current context is in context.md.
We're at Step 3 (implementing code).
The contracts in include/ are protected and cannot be changed.
Please implement the functions defined in include/my_service.h
```

## 📋 The 10-Step Workflow

1. **Add/change contracts** → Define interfaces
2. **Create tests** → Write tests against contracts  
3. **Add/change code** → Implement functionality
4. **Run tests** → Verify implementation
5. **Address errors** → Fix implementation issues
6. **Iterate** → Repeat until all tests pass
7. **Push to GitHub** → Share your changes
8. **Update documentation** → Keep docs current
9. **Clear scratch** → Clean temporary files
10. **Update context** → Record project state

See `DEVELOPMENT_PLAN.md` for detailed workflow rules.

## 🎯 When to Use This Workflow

Perfect for:
- New projects with AI assistance
- Complex systems needing stable interfaces
- Team projects requiring clear boundaries
- Educational projects teaching good practices

Not ideal for:
- Rapid prototypes where everything changes
- Single-file scripts
- Projects without testing requirements

## 📖 Documentation

- `SETUP.md` - How to configure for your project
- `DEVELOPMENT_PLAN.md` - Detailed workflow explanation
- `FILES.md` - Understanding the file structure
- `context.md` - Template for tracking progress

## 🤝 Contributing

This workflow template is open for improvements! 

To contribute:
1. Fork the repository
2. Create a feature branch
3. Make your improvements
4. Ensure documentation is updated
5. Submit a pull request

## 📄 License

[MIT License](LICENSE) - Use freely in your projects

## 🙏 Acknowledgments

This workflow was developed through extensive real-world usage in production projects, proving its effectiveness for AI-assisted development.

---

*Ready to start? Check out `SETUP.md` for configuration instructions!*