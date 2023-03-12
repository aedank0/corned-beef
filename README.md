# CornedBeefHash
A problem with using STL's built in hashing faculties is that the standard makes no guarantees about its performance or consistency. It's up to the implementation how they wish to trade off security and speed, and different implementations can output completely different values. Hashing the same data can even have a different result in seperate runs of the same application. CornedBeefHash was created to alleviate those concerns by providing a fast, simple, consistent, cross-platform implementation that can act as a stand-in for std::hash, as well as a few helper functions to make life easier.

### Requirements
Using CornedBeefHash requires C++20 and the standard library. The CMake and source files are only there for testing and automation on the development end, they aren't necessary if you just want to use the header.
If you wish to work on the project you'll need some additional things, see [`CONTRIBUTING.md`](https://github.com/aedank0/corned-beef/blob/e946ea2e2e76ddd9a21b961203cf37a1293acff2/CONTRIBUTING.md) for more.

### Usage
Simply copy corned_beef.hpp into your project and include where needed! It's recommended to use file from the main branch as it's the most up to date with features and fixes, but there's also the releases if you want an older version (for some reason). See below for a small code example.

### Documentation
Documentation is available in the repo for download and offline usage, as well as online [here](https://aedank0.github.io/corned-beef/docs/html/).

### Contribution
Contributions are welcomed!
Please read [`CONTRIBUTING.md`](https://github.com/aedank0/corned-beef/blob/e946ea2e2e76ddd9a21b961203cf37a1293acff2/CONTRIBUTING.md) for details.

## Example
```
#include <unordered_set>
#include <iostream>

#include "corned_beef.hpp"

namespace cb = corned_beef;

int main(int argc, const char** argv)
{
    std::cout << "Hash of -12 equals: " << HashThis(-12) << std::endl;

    std::unordered_set<int, cb::Hash<int>> set;
    set.emplace(2);
    set.emplace(-1);
    set.emplace(7);

    return !set.contains(-1);
}
```
