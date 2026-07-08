# Contributing to ignorelib

## Issues

Even without extensive programming knowledge, this is a way for you to help.
  - Trying to help people with issues.
  - Trying to reproduce potential bugs.

Please close duplicate issues.

## Reviewing and Testing Pull Requests

At this stage, we should be able to handle all pull requests that arise.

### Testing Pull Requests

Pick an open pull request, and build it and see if it works.

Be as thorough as possible with testing, specifically testing things related to
the pull request, and looking for even minor problems.

### Reviewing Pull Requests

If you have the relevant programming experience, feel free to look through the
code.
  - Look for potential edge cases that may arise in conditional statements.
  - Check to see if the formatting of the code makes sense in the context of the
    rest of the program.
    - I understand that this is vague. Essentially, this is largely about the
      larger format of the structure, i.e., how it is divided up into
      classes,functions, and such, rather than concern about specific syntax.
  - Ensure comments are necessary, specific, and relevant.
    - Comments (in main) should be appropriately written.
    - Self explanatory code should not have comments.
  - Watch out for performance issues that arise out of code changes.

## Writing Pull Requests

When creating pull requests, please keep in mind the following:

  - Make sure pull requests either fix an issue or add a new feature that is
    useful, or make some other improvement.
  - Keep pull requests topical and related. If something is unrelated, it should
    be a separate pull request.
  - For best practice, keep commits contained to relatively small changes.
    - This makes it easier to regress if issues later arise from code.
    - Ideally, every commit should work.
  - Turn on allow edits from maintainers, so we can make any final changes if
    necessary.

### On Pre-Commit:

Just install it via your package manager and run

```pre-commit install```

The github actions *should* catch any cases where it did not run, and then it
should be a relatively easy fix. You'll also need the clang-format and other
miscellaneous tools. Thank you!

As far as I know, there is no way to enforce the hooks in the .git directory.

## AI Contribution

The usage of LLMs is not specifically prohibited or recommended. However, it is
the responsibility of the programmer to:
  - Remove errant comments
  - Ensure the code functions as intended
  - Otherwise clean up the output

While LLMs are a sometimes useful tool, they can not fully understand an entire
program at this stage, and therefore code they produce will frequently require
changes in order to work with the rest of the existing code base.
