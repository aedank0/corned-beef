# Contributing
Contributions are very appreciated! This document should cover most things, but if you have any questions feel free to head to the discussion page on the GitHub.
## Project structure
CornedBeefHash uses a CI/CD pipeline with trunk-based development with short-lived feature branches. This basically means that the main branch (essentially just corned_beef.hpp) should be ready for usage by end users at any time. To this end, development is done on short-lived feature branches which are promptly merged into the main branch and deleted.
## Prerequisites
At least version 3.19 of CMake and the latest Doxygen are required.
## Style
Classes, functions, and enums use pascal case; variables use camel case; and static constants use all caps with underscores. The indentation style is Allman. When in doubt, look at the rest of the code or ask in the discussion page.
## Getting started
After installing the prerequisites and cloning the repository to your machine, the development process is as follows:
1. Find or create an issue describing what you wish to change. This goes for new features as well as changing pre-existing code. If you're adding a new feature and are newer it's a good idea to talk about it in the discussion page first before creating an issue. Also, if you're creating a new issue, make sure it is limited to one discreet topic.
2. Claim the issue as your own in the comments (though feel free to ask for help).
3. In your local copy of the repo, create a new branch with the name topic/your-topic-here (e.g. topic/cstr-hash). The name should be concise, clear, and match the name of the issue.
4. Make your proposed changes. Make sure to add test cases and documentation. If you fix a bug, add a test case that checks for it so the same bug doesn't reappear.
5. Use CMake's ALL build in Release config to build the tests and documentation.
6. Run the test suite and make sure it passes locally.
7. Commit all your changes to the topic branch you created earlier, ideally only using a single commit.
8. Create a PR to merge your new branch which references the issue from step 1.
9. Now just wait for review! If any changes are needed a reviewer will leave a comment explaining what.

