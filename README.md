# [sbs] Simple Binary Serialization

A modern, header-only C++20 binary serialization library.

## Features

- **Modern C++20**: Utilizes concepts, `std::span`, and ranges.
- **Header-Only**: All library code is in `include/sbs`.
- **STL Support**: Built-in serializers for standard containers (`std::vector`, `std::map`, `std::optional`,
  `std::variant`, etc.)
- **Custom Types**: You can implement custom serialization for types via method, function, or serializer
  function object.
- **Bidirectional First**: Serialization implementations are designed to be bidirectional which means minimal
  duplication of serialization and deserialization logic.
- **Endianness Control**: You can explicitly control whether serialization is little or big endian.
- **No Macros**: No macros outside of tests. All compile-time logic is implemented via templates, concepts, and
  constexpr logic.

## Documentation

Full documentation can be found under [docs](docs).

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
    User user_in { .id = 1, "Alice", 95.5f };
    std::vector<std::byte> data = sbs::serialize_to_vector(user_in);
    
    User user_out { };
    sbs::deserialize_from_span(data, user_out);
}
```

## Installation

Because sbs is header-only, you can simply copy the `include/sbs` directory into your project's include path.

You can also use the sbs using cmake via `add_subdirectory`:

```cmake
add_subdirectory(sbs)
target_link_libraries(my_executable PRIVATE sbs)
```

## STL Support

Serializers for many standard library containers can be found and included in `include/sbs/serializers`.

## License

MIT Licensed. See [LICENSE.txt](LICENSE.txt)
