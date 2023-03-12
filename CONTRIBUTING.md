# Contributing
Contributions are very appreciated! This document should cover most things, but if you have any questions feel free to head to the discussion page on the GitHub.
## Project structure
CornedBeefHash uses a CI/CD pipeline with trunk-based development. This basically means that the main branch (essentially just corned_beef.hpp) should be ready to be taken and used by end users at any time.
## Prerequisites
At least version 3.19 of CMake (ideally the latest release) and the latest Doxygen are required.
## Style
Classes, functions, and enums use pascal case; variables use camel case; and static constants use all caps with underscores in between words. The indentation style is Allman. When in doubt, look at the rest of the code or ask in the discussion page. Using braces when the contents of a for loop or if statement are a single line are optional.
## Guidelines
Everything should be compliant to the C++20 standard, so no undefined behaviour or platform/compiler specific stuff. Try to have specific names for commits/issues/PRs/etc. For example, put "Fixed bug which x y z" instead of "Bugfix". In terms of commuinity guidelines, the general rule is DBAD. We're all here to collaborate and help eachother, so feel free to ask questions or for help!
## Getting started
1. Find or create an issue describing what you wish to change. This goes for new features as well as changing pre-existing code. If you're adding a new feature and it's a good idea to talk about it in the discussion page first before creating an issue. Also, if you're creating a new issue, make sure it is limited to one discrete topic.
2. Claim the issue as your own in the comments (though feel free to ask for help).
3. Fork the repository, then clone it onto your local machine. Make sure you can compile it and pass the test suite before changing anything.
4. Make your proposed changes. Make sure to add test cases and documentation. If you fix a bug, add a test case that checks for it in case the same bug reappears.
5. Use CMake's ALL build in Release config to build the tests and documentation.
6. Run the test suite and make sure it passes locally.
7. Commit all your changes to the main branch, ideally only using a single commit.
8. Push your local changes to your remote fork.
9. Create a PR to merge your fork to the original repo which references the issue from step 1.
10. Now just wait for review! If any changes are needed a reviewer will leave a comment explaining what.

