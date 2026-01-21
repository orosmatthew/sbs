# [sbs] Documentation

## Table of Contents

- [Installation](#installation)
- [Serializable Types](#serializable-types)
- [Serializers](#serializers)

## Installation

sbs is a header only library that can be used by including the `include/sbs` directory into your project's include path.

You could also use the library using CMake via:

```cmake
add_subdirectory(sbs)
target_link_libraries(my_executable PRIVATE sbs)
```

## Serializable Types

sbs makes use of [C++20 Concepts](https://en.cppreference.com/w/cpp/language/constraints.html) to determine of types are
serialized at compile-time.

## Default Serializable

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

These types are built-in to the `sbs::Archive` class which is responsible for statically dispatching how to serialize a given type.

### Object Serializable

Types that satisfy the `sbs::ObjectSerializable` concept are what most custom types will be required to implement to be
default-serializable. `sbs::ObjectSerializable` requires the type to be either `sbs::MethodSerializable` or
`sbs::FunctionSerializable`.

`sbs::MethodSerializable` requires the type to implement a non-const `void serialize(sbs::Archive&)` method.

`sbs::FunctionSerializable` uses [ADL](https://en.cppreference.com/w/cpp/language/adl.html) to find a function
implementation `void serialize(sbs::Archive&, T&)` function for a given type `T`.

## Serializers

Serializers are [function objects](https://en.cppreference.com/w/cpp/functional.html), or functors, that implement serialization logic similar to an object being object-serializable.

There are two use-cases for a serializer:

1. They can implement serialization logic for a type without having to add a method to the type or a function in the
   type's namespace.

2. They can be used to override the serialization logic for an already default-serializable type.

Serializers must implement the `sbs::Serializer` concept which requires the serializer to be default-constructible and to implement `operator()(sbs::Archive&, T&)` for the `T` type that the serializer is serializing.

Using a serializer requires explicitly passing it into a serialization method/function.

An example serializer and usage:

```c++
#include <sbs/sbs.hpp>

struct User {
    uint32_t id;
    float score;
};

struct UserSerializer {
    void operator()(sbs::Archive& ar, User& user) const {
        ar.archive(user.id);
        ar.archive(score.id);
    }
};

int main() {
    User my_user { .id = 37, .score = 95.5f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector<UserSerializer>(my_user);
    // `sbs::serialize_to_vector(my_user)` would fail because `User` is not
    // default serializable and so a serializer must be explicitely passed.
}
```
