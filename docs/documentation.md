# [sbs] Documentation

## Table of Contents

- [Installation](#installation)
- [Serialization Functions](#serialization-functions)
- [Serializable Types](#serializable-types)
- [Serializers](#serializers)
- [Archive Class](#archive-class)
- [Binary Format](#binary-format)
- [Error Handling](#error-handling)

## Installation

sbs is a header only library that can be used by including the `include/sbs` directory into your project's include path.

You could also use the library with CMake:

```cmake
add_subdirectory(sbs)
target_link_libraries(my_executable PRIVATE sbs)
```

## Serialization Functions

The primary functions for serialization and deserialization use callbacks for reading and writing bytes. The write callback accepts a `std::span<const std::byte>` to then write. The read callback accepts a `size_t` number of bytes expected to be returned and returns a `std::span<const std::byte>` to deserialize from. Both functions accept which endian to use to serialize/deserialize which defaults to little endian.

```c++
using WriteCallback = std::function<void(std::span<const std::byte>)>;
using ReadCallback = std::function<std::span<const std::byte>(size_t)>

void serialize_using_callback(
    Type& value, 
    WriteCallback write_callback, 
    std::endian endian = std::endian::little);

void deserialize_using_callback(
	Type& value
    ReadCallback read_callback,
    std::endian endian = std::endian::little);
```

Note that since sbs is primarily a bidirectional serialization library where serialization implementations on types are bidirectional, serialization functions always accept a non-const reference even if it is not modified. 

Deserialization in sbs is mutation-based meaning that deserialization is an action performed on an already constructed object. This is why a reference is accepted into deserialization functions.

There are also some helper functions that are built on these callback functions.

```c++
std::vector<std::byte> serialize_to_vector(
    Type& value, std::endian endian = std::endian::little);

void deserialize_from_span(
    std::span<const std::byte> bytes, 
    Type& value, 
    std::endian endian = std::endian::little);

void serialize_to_file(
    const std::filesystem::path& path, 
    Type& value, 
    std::endian endian = std::endian::little);

void deserialize_from_file(
    const std::filesystem::path& path, 
    Type& value, 
    std::endian endian = std::endian::little)
```

## Serializable Types

sbs makes use of [C++20 Concepts](https://en.cppreference.com/w/cpp/language/constraints.html) to determine how types are serialized at compile-time.

### Default Serializable

The most ergonomic way for types to be serialized in sbs is for them to satisfy the `sbs::DefaultSerializable` concept. `sbs::DefaultSerializable` requires type either be `sbs::ValueSerializable` or `sbs::ObjectSerializable`.

If a type is default-serializable then no explicit template arguments for serialization are required to serializing the type.

For example:

```c++
#include <sbs/sbs.hpp>

struct User {
    uint32_t id;
    float score;
    
    void serialize(sbs::Archive& ar) {
        // `id` and `score` are default-serializable because they
        // both satisfy the `sbs::ValueSerializable` concept.
        ar.archive(id);
        ar.archive(score);
    }
};

int main() {
    std::vector<std::byte> bytes;
    
    // `my_int` is default-serializable because it satisfies
    // the `sbs::ValueSerializable` concept.
    uint32_t my_int = 7;
    bytes = sbs::serialize_to_vector(my_int);
    
    // `my_user` is default-serializable because it satisfies 
    // the `sbs::ObjectSerializable` concept.
    User my_user { .id = 37, .score = 95.5f };
    bytes = sbs::serialize_to_vector(my_user);
    
}
```

### Value Serializable

The `sbs::ValueSerializable` concept allows a type to be directly bitwise copied for serialization (while taking endianness into account). This mostly applies to fundamental primitive types. `sbs::ValueSerializable` requires the type to be either `sbs::IntegerSerializable` or `sbs::FloatSerializable`.

`sbs::IntegerSerializable` applies to all integral types or enums whose underlying types are integrals.

`sbs::FloatSerializable` applies to floating-point values that are represented as IEC 559 (IEEE 754).

Serialization for these types are handled internally by the `sbs::Archive::archive_value` method requiring no user-defined logic.

### Object Serializable

Types that satisfy the `sbs::ObjectSerializable` concept are what most custom types will satisfy to be default-serializable. `sbs::ObjectSerializable` requires the type to be either `sbs::MethodSerializable` or `sbs::FunctionSerializable`.

`sbs::MethodSerializable` requires the type to implement a non-const `void serialize(sbs::Archive&)` method.

`sbs::FunctionSerializable` uses [ADL](https://en.cppreference.com/w/cpp/language/adl.html) to find a function implementation `void serialize(sbs::Archive&, Type&)` function for a given `Type`.

## Serializers

Serializers are [function objects](https://en.cppreference.com/w/cpp/functional.html), or functors, that implement serialization logic similar to an object being object-serializable.

There are two use-cases for a serializer:

1. They can implement serialization logic for a type without having to add a method to the type or a function in the type's namespace.
   
2. They can be used to override the serialization logic for an already default-serializable type.

Serializers must implement the `sbs::Serializer` concept which requires the serializer to be default-constructible and to implement `operator()(sbs::Archive&, Type&)` for a `Type` that the serializer is serializing.

Using a serializer requires explicitly passing it into a serialization/archive method/function.

An example serializer and usage:

```c++
#include <sbs/sbs.hpp>

struct SaveFileSerializer {
    void operator()(sbs::Archive& ar, SaveFile& save_file);
}

struct Player {
    uint32_t id;
    float score;
    SaveFile save_file;
};

struct PlayerSerializer {
    void operator()(sbs::Archive& ar, User& user) const {
        ar.archive(user.id);
        ar.archive(user.score);
        // Assume SaveFile is not default-serializable.
        // This will require a serializer to be explicitly passed as such.
        ar.archive<SaveFileSerializer>(user.save_file);
    }
};

int main() {
    Player my_player { .id = 37, .score = 95.5f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector<PlayerSerializer>(my_player);
    // `sbs::serialize_to_vector(my_player)` would fail because `Player` is not
    // default serializable and so a serializer must be explicitely passed.
}
```

### Default Serializer

If no serializer is provided to a serialize function or archive method, then the `sbs::DefaultSerializer` is used. The default serializer chooses the approach strategy to serialize a type at compile-time based on the concepts the type satisfies. For example, assume `sbs::Archive::archive(value)` is called with a `Type value` without any explicit serializer passed:

* If the type satisfies `sbs::ValueSerializable`, it will call `sbs::Archive::archive_value(value)`. 
* If it satisfies `sbs::MethodSerializable`, it will call `value.serialize(archive)`. 
* If it satisfies`sbs::FunctionSerializable`, it will call `serialize(archive, value)`.

If none of these concepts are satisfied, a compilation error will occur. You will then either have to satisfy one of the above concepts to make the type default-serializable or pass in a serializer explicitly.

### Standard Library Serializers

sbs providers [serializers](#serializers) for many standard library types. These implementations can be found in `sbs/serializers/*.hpp`. The serializers can be used explicitly but also provide functions that satisfy the function-serializable concept which makes them default-serializable.

```c++
#include <sbs/sbs.hpp>
#include <sbs/serializers/string.hpp>
#include <sbs/serializers/vector.hpp>
#include <sbs/serializers/chrono.hpp>

struct Employee {
    std::string name;
    std::vector<uint32_t> assigned_task_ids;
    std::chrono::time_point<std::chrono::system_clock> last_login;
    
    void serialize(sbs::Archive& ar) {
        ar.archive(name); 
        // or ar.archive<StringSerializer>(name);
   		
        ar.archive(assigned_task_ids); 
        // or ar.archive<VectorSerializer>(assigned_task_ids);
        
        ar.archive(last_login);
        // or ar.archive<ChronoTimePointSerializer>(last_login)
    }
}
```

If an sbs serialization implementation of standard library type is insufficient for your purposes, you can explicitly pass in your own serializers.

```c++
struct Employee {
    std::string name;
    std::vector<uint32_t> assigned_task_ids;
    std::chrono::time_point<std::chrono::system_clock> last_login;
    
    void serialize(sbs::Archive& ar) {
        ar.archive<MyCustomStringSerializer>(name); 
        ar.archive<MyCustomVectorSerializer>(assigned_task_ids); 
        ar.archive<MyCustomTimePointSerializer>(last_login);
    }
}
```

## Archive Class

The `sbs::Archive` class is the primary engine of sbs. It is normally not constructed directly but instead indirectly by a [serialization function](#serialization-functions). The Archive class is passed into all serialization implementations for types whether it be method-based, function-based, or a serializer. The primary method of the Archive class is `sbs::Archive::archive` which accepts either a default-serializable object or an object with an explicit serializer.

```c++
struct Book {
    uint32_t pages;
    Author author;
    
    void serialize(sbs::Archive& ar) {
        // `uint32_t` is default-serializable.
        ar.archive(pages); 
        // Explicit serializer passed to either override default author
        // serialization logic or if author is not default-serializable.
        ar.archive<AuthorSerializer>(author);
    }
}
```

Since sbs is a bidirectional serialization library, the Archive class provides methods for determining the direction of serialization for instances where separate serialization vs. deserialization logic is requried.

```c++
struct SimpleVectorSerializer {
	void operator()(sbs::Archive& ar, std::vector<T>& vector) const {
        if (ar.serializing()) {
            uint64_t size = vector.size();
            ar.archive(size); // size is now serialized.
            // serialize items...
        } else { // ar.deserializing() == true
            vector.clear();
            uint64_t size = 0;
            ar.archive(size); // size is now deserialized.
            vector.reserve(size);
            // deserialize items...
        }
    }
}
```

## Binary Format

sbs does not implement any special binary format. Binary serialization is implemented as a non-padded stream of bitwise copied value-serializable types (while taking endianness into account). This means compiler/platform-specific padding is not a factor. No type information or metadata is encoded in the output. This means the output is not self-describing which means it cannot be introspected without explicitly knowing the exact format beforehand. This also applies to endianness which must be agreed upon by both serialization and deserialization.

Serialization/Deserialization is performed in an "immediate-style" meaning that it does not support random-access/out-of-order reading/writing and thus the order of `sbs::Archive::archive` method calls is order dependent.

sbs is not a zero-copy serialization library. Serialization/Deserialization always copies bytes to/from runtime objects and the input/output bytes. This is a purposeful decision to decouple runtime representation and the output binary representation.

sbs does not provide any kind of versioning support. This responsibility is passed to the user of the library.

## Error Handling

Error handling is only provided via exceptions. Since the binary format is not self-describing, meaning it does not contain any type information, it is up to the implementation of serializers for specific types to catch and throw their own exceptions based on the data it receives.

Exceptions can also be thrown is some helper [serialization functions](#serialization-functions) such as on a filesystem error or when insufficient data is provided. 

`sbs::Archive` will throw an exception on deserialization if it does not receive enough bytes to deserialize a given type.

