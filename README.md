# ignorelib
C++ library for .gitignore-style file parsing and pattern matching

## Installation

You can use ignorelib in your projects through any one of the following:

#### Clone and build repository
```sh
git clone https://github.com/Romket/ignorelib.git
cd ignorelib && mkdir build && cd build
cmake .. && cmake --build .
```

#### Include through FetchContent
```cmake
include(FetchContent)

find_package(ignorelib QUIET)
if(NOT ignorelib_FOUND)
    FetchContent_Declare(
        ignorelib
        GIT_REPOSITORY https://github.com/Romket/ignorelib.git
        GIT_TAG <release version or tag>
    )
    FetchContent_MakeAvailable(ignorelib)
endif()
```

## Usage

### 1. Include the header file `ignorelib/ignorelib.h`. This file contains everything you need to fully utilize ignorelib.

### 2. Create an `IgnoreLib::IgnoreFile` object. There are a few ways to do this, such as pointing to a .gitignore-style file containing patterns, refered to as an ignore file, passing in a list of patterns, or passing a list of pre-constructed `IgnoreLib::Pattern` objects.

#### _Passing a path to an ignore file_

This is likely the most common way you will use ignorelib, as it is what most
closely mirrors git and .gitignore files, as well as easily allowing you to
accept user input for the files to ignore.

```cpp
#include <ignorelib/ignorelib.h>

void LoadFromFileExample()
{
    IgnoreLib::IgnoreFile file("path/to/ignorefile");
}
```

#### _Passing a list of patterns_

```cpp
#include <ignorelib/ignorelib.h>

void LoadFromPatternListExample()
{
    IgnoreLib::IgnoreFile file{"ignore/this.file", "!**/more[120a-q]/**/complicated?/pattern\\[/*"};
}
```

#### _Passing a list of `IgnoreLib::Pattern` objects_

This is likely to be the least useful way to construct an
`IgnoreLib::IgnoreFile` object, as ignorelib will not take care of any of the
pattern to regex conversions, but it does allow you to integrate custom regex
strings into the search.

[!IMPORTANT]
This method also requires the [re2](https://github.com/google/re2) library from
Google.

```cpp
#include <ignorelib/ignorelib.h>

#include <re2/re2.h>

void LoadFromIgnoreLibPatternExample()
{
    IgnoreLib::Pattern p1;
    p1.Re = std::make_shared<re2::RE2>("path(?:\\/.*\\/|\\/)to\\/[^\\/\\\\]*\\.file");

    // The following members have default values that can be found
    // in the documentation for IgnoreLib::Pattern
    p1.Negated = true;
    p1.TopLevelOnly = true;
    p1.DirsOnly = false;
    p1.SepCount = 1;

    IgnoreLib::IgnoreFile file{p1};
}
```

More information about the constructors for the IgnoreFile class can be found in
the [documentation](#documentation).

### 3. Find ignored/included files. You can either scan a directory or check an individual path.

#### _Scanning directories_

```cpp
#include <ignorelib/ignorelib.h>

namespace fs = std::filesystem;

void ScanDirectoryExample()
{
    IgnoreLib::IgnoreFile f("path/to/ignorefile");

    fs::path toScan = "path/to/directory";

    std::vector<fs::path> ignoredFast = f.ListIgnoredFast(toScan);
    std::vector<fs::path> ignoredFull = f.ListIgnoredFull(toScan);

    std::vector<fs::path> includedFast = f.ListIncludedFast(toScan);
    std::vector<fs::path> includedFull = f.ListIncludedFull(toScan);
}
```

#### _Checking a single file_

```cpp
#include <ignorelib/ignorelib.h>

namespace fs = std::filesystem;

void CheckFileExample()
{
    IgnoreLib::IgnoreFile f("path/to/ignorefile");

    fs::path file = "path/to.file";

    bool isIgnoredFast = f.IgnoredFast(file);
    bool isIgnoredFull = f.IgnoredFull(file);

    bool isIncludedFast = f.IncludedFast(file);
    bool isIncludedFull = f.IncludedFull(file);
}
```

[!NOTE]
The "fast" behavior refers to gitignore's behavior, where an ignored path will
exclude all contained files, even if a contained file is later re-included. The
"full" behavior will re-include this, but it is slower.

## Documentation

Currently, no pre-built documentation exists online. You can build the
documentation by running doxygen:

```sh
doxygen
```

You can then view the generated documentation by opening `docs/html/index.html`
in any web browser or by opening `docs/latex/refman.pdf` in any PDF viewer.

## Contributing

All contributions to ignorelib are welcome. Please check
[CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

###### _ignorelib is licensed under the GPL v3. See [LICENSE](LICENSE) for details._
