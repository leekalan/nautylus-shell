# Git Practice

## Issue Practice

### Issue Naming

Short but descriptive name for the issue:

- `Test and implement ls`

### Issue Structure

Issue

- Brief description of what the problem or missing functionality is

Plan

- Skeleton for what the related branch will aim to achieve

Notes

- Any additional notes (dependencies, remaining errors, etc.)

Pull Request

- Link to the related pull request (once created)

### Example Issue

Title: `Test and implement ls`

---

```
Issue: The shell does not currently support the `ls` command

Plan:
- Create tests for the `ls` command
- Implement the `ls` command

Notes:
- Relies on the `find` command [Test and implement find](https://github.com/) <-- link to `find` Issue
- Resolves the `touch` dependency [Test and implement touch](https://github.com/) <-- link to `touch` Issue
- Can crash if the file is a shortcut

Pull Request:
- [test-and-implement-ls](https://github.com/)
```

## Branch Practice

### Branch Naming

Seperate the type of branch and the name by a `/`.

Use descriptive, lowercase, hyphen-separated names:

- `feature/test-and-implement-ls`

The branch types that are used are:

- `feature` - for new features
- `bugfix` - for bug fixes
- `refactor` - for code refactoring
- `meta` - for more complicated changes involving the whole project

### Branch Usage

Always branch off `master` and delete branches after merging.

Keep `master` stable, only merge tested and reviewed features.

In the case of circular dependencies, clearly describe which tests rely on unimplemented features within the github-issue.

### Feature Branches

Feature branches should adhere to the following workflow:

- Document the missing functionality and plan within the corresponding github-issue
- Commit any required directories, files, functions and types to the related header files
- Commit the feature tests
- Commit the feature implementation
- Create a PR and document a review of the feature and refactor if any problems appear

### Bugfix Branches

Bugfix branches should adhere to the following workflow:

- Document the bug within the corresponding github-issue
- Commit the tests which reproduce the bug (if not possible explain why in the documentation file)
- Commit the bugfix
- Create a PR and document a review of the bugfix and refactor if any problems appear

### Refactor Branches

Refactor branches should adhere to the following workflow:

- Document the reasons for the refactor within the corresponding github-issue
- Commit the tests which test for the refactor (if not possible explain why in the documentation file)
- Commit the refactor
- Create a PR document a review of the changes and refactor if any problems appear

## Commit Practice

### Commit Naming

Use descriptive, lowercase names:

- `add tests for ls`

### Commit Usage

Never commit changes to `master` (expect for setting up the project of course!).

Commits should be short and simple, any complexity in implementation should be captured within the issue, not the commit).

## Pull Request Practice

### Naming

A concise summary:

- `Test and implement ls`

### Description

`Issue: [issue-name](link)`

`Resolves: list the issues of any circular dependencies that have been resolved`

`Review: <short review of the changes>`
