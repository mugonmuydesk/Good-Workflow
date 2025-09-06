# Protection Override Guide

## ⚠️ WARNING: Use With Extreme Caution

This guide explains how to temporarily disable contract and test protection. This should only be used in exceptional circumstances with explicit team approval.

## When to Use the Override

The protection override should **ONLY** be used for:

1. **Critical Contract Fixes**: When a protected contract has a fundamental design flaw that blocks development
2. **Major Refactoring**: During approved architectural changes that require interface modifications
3. **Emergency Patches**: When production issues require immediate interface changes
4. **Test Corrections**: When protected tests have errors that prevent valid implementations from passing

## When NOT to Use the Override

Never use the override for:

- Convenience or to "work around" the protection
- Making implementation easier by changing contracts
- Fixing failing tests by modifying them instead of the implementation
- Any changes without team consensus

## How to Enable the Override

### Step 1: Document the Need

Before enabling the override, document:
- Why the change is necessary
- What specific files need modification
- The expected impact on existing implementations
- Team members who approved the change

### Step 2: Add the Repository Secret

1. Navigate to your repository on GitHub
2. Go to **Settings** → **Secrets and variables** → **Actions**
3. Click **New repository secret**
4. Add the following:
   - **Name**: `ALLOW_PROTECTED_EDITS`
   - **Value**: `true`
5. Click **Add secret**

### Step 3: Verify Override is Active

1. Make a small change to trigger GitHub Actions
2. Check the Actions tab
3. Look for the warning message:
   ```
   ⚠️ WARNING: Contract protection is temporarily DISABLED
   ```

### Step 4: Make Your Changes

With protection disabled:
1. Modify the necessary protected files
2. Ensure all changes are well-documented in commit messages
3. Create a pull request with detailed explanation
4. Get additional review from team members

### Step 5: Re-enable Protection (CRITICAL!)

**Immediately after merging your changes:**

1. Go back to **Settings** → **Secrets and variables** → **Actions**
2. Find `ALLOW_PROTECTED_EDITS`
3. Either:
   - **Delete** the secret (recommended)
   - **Update** its value to anything other than `true`
4. Verify protection is re-enabled by checking the next GitHub Action run

## Best Practices

### Before Disabling Protection

- [ ] Get explicit approval from at least 2 team members
- [ ] Document the reason in the pull request description
- [ ] Create an issue tracking the contract change
- [ ] Consider if the change can be avoided through better implementation

### While Protection is Disabled

- [ ] Only modify the specific files discussed
- [ ] Keep the override period as short as possible
- [ ] Notify the team that protection is temporarily disabled
- [ ] Monitor all pull requests extra carefully

### After Re-enabling Protection

- [ ] Confirm protection is working (make a test PR if needed)
- [ ] Update all documentation affected by the changes
- [ ] Notify team members about the interface changes
- [ ] Update any dependent code that needs adjustment

## Security Considerations

- Repository secrets are only accessible to repository administrators
- All uses of the override are logged in GitHub Actions history
- The override only affects the protection checks, not other CI/CD steps
- Git history still tracks all changes for audit purposes

## Troubleshooting

### Override Not Working

If setting the secret doesn't disable protection:
1. Ensure the secret name is exactly `ALLOW_PROTECTED_EDITS`
2. Ensure the value is exactly `true` (lowercase, no quotes)
3. Check you have admin permissions on the repository
4. Try re-running the GitHub Action

### Can't Re-enable Protection

If protection doesn't resume after removing the secret:
1. Verify the secret is actually deleted/changed
2. Trigger a new GitHub Action run
3. Check for typos in the secret name
4. Ensure no duplicate secrets exist

### Accidental Changes While Disabled

If unwanted changes were made while protection was disabled:
1. Immediately re-enable protection
2. Revert the unwanted changes
3. Review the GitHub Actions logs to understand what happened
4. Consider adding additional review requirements

## Example Scenario

### Fixing a Contract Design Flaw

```markdown
## Issue #123: UserService Interface Missing Critical Method

The IUserService interface is missing a method for batch operations,
causing performance issues in production.

### Approval
- @teammate1: Approved (2024-01-15)
- @teammate2: Approved (2024-01-15)

### Changes Required
- Add `batchProcess()` method to IUserService interface
- Update all implementations
- Add corresponding tests

### Override Period
- Enabled: 2024-01-15 14:00 UTC
- Disabled: 2024-01-15 15:30 UTC
```

## Alternative Approaches

Before using the override, consider:

1. **Adapter Pattern**: Create an adapter that provides the new interface while maintaining the old one
2. **Extension Interface**: Create a new interface that extends the protected one
3. **Versioning**: Create a v2 of the interface while maintaining v1 for compatibility
4. **Implementation-Level Fix**: Solve the problem in implementation without changing contracts

## Audit Log Template

When using the override, add an entry to your project's CHANGELOG or audit log:

```markdown
### Protection Override Log

**Date**: YYYY-MM-DD
**Duration**: HH:MM
**Authorized By**: @username1, @username2
**Files Modified**: 
- include/example.h
- tests/test_example.cpp
**Reason**: Brief description
**PR**: #XXX
```

---

⚠️ **Remember**: The protection mechanism exists to maintain architectural integrity and prevent accidental breaking changes. Every override should be treated as an exceptional event requiring careful consideration and documentation.