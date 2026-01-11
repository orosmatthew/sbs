#pragma once

#include <algorithm>
#include <climits>
#include <cstddef>
#include <functional>
#include <span>
#include <vector>

namespace sbs {

static_assert(CHAR_BIT == 8, "SBS only supports platforms with 8-bit bytes.");

class Archive;

template <class Type>
concept IntegerSerializable
    = std::is_integral_v<Type> || (std::is_enum_v<Type> && std::is_integral_v<std::underlying_type_t<Type>>);

template <class Type>
concept FloatSerializable = std::is_floating_point_v<Type>;

template <class Type>
concept ValueSerializable = IntegerSerializable<Type> || FloatSerializable<Type>;

template <class Type>
concept MethodSerializable = requires(Type type, Archive& archive) { type.serialize(archive); };

template <class Type>
concept FunctionSerializable = requires(Type type, Archive& archive) { serialize(type, archive); };

template <class Type>
concept ObjectSerializable = MethodSerializable<Type> || FunctionSerializable<Type>;

template <class Type>
concept DefaultSerializable = ValueSerializable<Type> || ObjectSerializable<Type>;

template <class SerializeType, class Type>
concept Serializer = requires(SerializeType serialize, Type& value, Archive& archive) {
    { serialize.operator()(value, archive) } -> std::same_as<void>;
} && std::is_default_constructible_v<SerializeType>;

using WriteCallback = std::function<void(std::span<const std::byte>)>;
using ReadCallback = std::function<std::span<const std::byte>(size_t)>;

enum class Mode { serialize, deserialize };

template <class Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerializer;

class Archive {
public:
    static Archive create_serialize(WriteCallback* write_callback)
    {
        return Archive(write_callback);
    }

    static Archive create_deserialize(ReadCallback* read_callback)
    {
        return Archive(read_callback);
    }

    template <ValueSerializable Value>
    void archive_value(Value& value)
    {
        if (m_mode == Mode::serialize) {
            std::span<const std::byte> bytes = std::as_bytes(std::span<const Value>(&value, 1));
            std::invoke(*m_write_callback, bytes);
        } else {
            std::span<const std::byte> source = std::invoke(*m_read_callback, sizeof(Value));
            std::span<std::byte> dest = std::as_writable_bytes(std::span<Value>(&value, 1));
            std::ranges::copy(source, dest.begin());
        }
    }

    template <class Type>
        requires(DefaultSerializable<Type>)
    void archive(Type& value)
    {
        DefaultSerializer<Type> { }(value, *this);
    }

    template <class SerializeType, class Type>
        requires(Serializer<SerializeType, Type>)
    void archive(Type& value)
    {
        SerializeType { }(value, *this);
    }

    template <class Type>
        requires(DefaultSerializable<Type> && std::copyable<Type>)
    void archive_copy(const Type& value)
    {
        Type copy = value;
        DefaultSerializer<Type> { }(copy, *this);
    }

    template <class SerializeType, class Type>
        requires(Serializer<SerializeType, Type> && std::copyable<Type>)
    void archive_copy(const Type& value)
    {
        Type copy = value;
        std::invoke(SerializeType { }, copy, *this);
    }

    [[nodiscard]] Mode mode() const
    {
        return m_mode;
    }

    [[nodiscard]] bool serializing() const
    {
        return m_mode == Mode::serialize;
    }

    [[nodiscard]] bool deserializing() const
    {
        return m_mode == Mode::deserialize;
    }

private:
    Mode m_mode;
    WriteCallback* m_write_callback { };
    ReadCallback* m_read_callback { };

    explicit Archive(std::function<void(std::span<const std::byte>)>* write_callback)
        : m_mode { Mode::serialize }
        , m_write_callback { write_callback }
    {
    }

    explicit Archive(std::function<std::span<const std::byte>(size_t)>* read_callback)
        : m_mode { Mode::deserialize }
        , m_read_callback { read_callback }
    {
    }
};

template <class Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerializer {
    void operator()(Type& value, Archive& archive) const
    {
        if constexpr (ValueSerializable<Type>) {
            archive.archive_value(value);
        } else if constexpr (MethodSerializable<Type>) {
            value.serialize(archive);
        } else if constexpr (FunctionSerializable<Type>) {
            serialize(value, archive);
        }
    }
};

template <class Type>
    requires(DefaultSerializable<Type>)
void serialize_using_callback(Type& value, WriteCallback write_callback)
{
    auto archive = Archive::create_serialize(&write_callback);
    archive(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void deserialize_using_callback(Type& object, ReadCallback read_callback)
{
    auto archive = Archive::create_deserialize(&read_callback);
    object.serialize(archive);
}

template <class Type>
    requires(DefaultSerializable<Type>)
std::vector<std::byte> serialize_to_vector(Type& value)
{
    std::vector<std::byte> result;
    WriteCallback callback
        = [&result](std::span<const std::byte> bytes) { result.insert(result.end(), bytes.begin(), bytes.end()); };
    auto archive = Archive::create_serialize(&callback);
    archive.archive(value);
    return result;
}

template <class Type>
    requires(DefaultSerializable<Type>)
void deserialize_from_span(Type& value, std::span<const std::byte> bytes)
{
    ReadCallback callback = [&bytes](const size_t size) {
        std::span<const std::byte> subspan = bytes.subspan(0, size);
        bytes = bytes.subspan(size, bytes.size() - size);
        return subspan;
    };
    auto archive = Archive::create_deserialize(&callback);
    archive.archive(value);
}
}
