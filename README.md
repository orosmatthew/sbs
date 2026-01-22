# [sbs] Simple Binary Serialization

[![Compile and Test](https://github.com/orosmatthew/sbs/actions/workflows/ci.yaml/badge.svg?branch=main)](https://github.com/orosmatthew/sbs/actions/workflows/ci.yaml)

A modern, header-only C++20 binary serialization library.

## Features

- **Modern C++20**: Heavily utilizes concepts.
- **Header-Only**: All library code is in `include/sbs`.
- **STL Support**: Built-in serializers for standard containers (`std::vector`, `std::map`, `std::optional`,
  `std::variant`, etc.)
- **Custom Types**: You can implement custom serialization for types via method, function, or serializer
  function object.
- **Bidirectional First**: Serialization implementations are designed to be bidirectional which means minimal duplication of serialization and deserialization logic.
- **Endianness Control**: You can explicitly control whether serialization is little or big endian.
- **Simple Binary Format**: No special binary format, just the raw bytes of data without any extra type information or metadata.
- **No Macros**: No macros outside of tests. All compile-time logic is implemented via templates, concepts, and constexpr logic.
- **Callback-based I/O**: For simple integration.

## Documentation

Full documentation can be found in [docs/documentation.md](docs/documentation.md)

## Basic Example

```c++
#include <sbs/sbs.hpp>
#include <sbs/serializers/string.hpp>

struct User {
    uint32_t id;
    std::string username;
    float score;
    
    // Member approach
    void serialize(sbs::Archive& ar) {
        ar.archive(id);
        ar.archive(username);
        ar.archive(score);
    }
};

/* Function approach
void serialize(sbs::Archive& ar, User user) {
    ar.archive(user.id);
    ar.archive(user.username);
    ar.archive(user.score);
}
*/

int main() {
    User user_in { .id = 1, .username = "Alice", .score = 95.5f };
    std::vector<std::byte> data = sbs::serialize_to_vector(user_in);
    
    User user_out { };
    sbs::deserialize_from_span(data, user_out);
}
```

Other examples can be found under [examples/](examples).

## Installation

Because sbs is header-only, you can simply copy the `include/sbs` directory into your project's include path.

You can also use the sbs using cmake via `add_subdirectory`:

```cmake
add_subdirectory(sbs)
target_link_libraries(my_executable PRIVATE sbs)
```

## STL Support

Serializers for many standard library containers can be found and included in `include/sbs/serializers`.

All template arguments are accepted meaning you are free to use custom allocators, hash functions, etc.

## Building and Running Tests

Tests can be built and run by toggling the `SBS_BUILD_TESTS=On` CMake option and running the `sbs_tests` executable.

Part of testing is to compare serialization with binary files located in `tests/files`. If these files are to be regenerated, the `SBS_WRITE_TEST_FILES=On` CMake option can be toggled.

## Why Another Serialization Library?

* sbs is header-only and does not use any external dependencies. The core of the library is a single header `sbs/sbs.hpp` where standard library support can be optionally removed by not including `sbs/serializers/*.hpp`
* sbs heavily utilizes C++20 concepts which make source code and error message much more readable.
* sbs does not use any macros outside of its tests. All compile-time logic is implemented with templates and constexpr.
* sbs has extensive support for standard library containers with full ability to customize allocators, hash functions, etc.
* sbs's binary format is extremely simple because there really isn't one. There is no extra metadata, type information, or padding; simply a stream of contiguous bytes representing the primitive data from all the serialized types.
* sbs does not directly bitwise copy structs or types outside of primitives which makes it very portable across platforms and compiler implementations.
* sbs does not store any data internally. Serialization and deserialization operations are performed in an "immediate-style" where data is passed to and from callbacks as data is processed.

## License

MIT Licensed. See [LICENSE.txt](LICENSE.txt)
