#pragma once

#include <algorithm>
#include <climits>
#include <cstddef>
#include <functional>
#include <span>

namespace sbs {

static_assert(CHAR_BIT == 8, "SBS only supports platforms with 8-bit bytes.");

class Archive;

template <typename Value>
concept ValueSerializable = std::is_fundamental_v<Value> || std::is_enum_v<Value>;

template <typename Object, typename Archive = Archive>
concept MethodSerializable = requires(Object& object, Archive& archive) { object.serialize(archive); };

template <typename Object, typename Archive = Archive>
concept FunctionSerializable = requires(Object& object, Archive& archive) { serialize(object, archive); };

template <typename Object, typename Archive = Archive>
concept ObjectSerializable = MethodSerializable<Object, Archive> || FunctionSerializable<Object, Archive>;

template <typename T, typename Archive = Archive>
concept DefaultSerializable = ValueSerializable<T> || ObjectSerializable<T, Archive>;

template <typename SerializeType, typename Type, typename Archive = Archive>
concept Serialize = requires(const SerializeType serialize, Type& value, const Archive& archive) {
    { serialize.operator()(value, archive) } -> std::same_as<void>;
} && std::is_default_constructible_v<SerializeType>;

using WriteCallback = std::function<void(std::span<const std::byte>)>;
using ReadCallback = std::function<std::span<const std::byte>(size_t)>;

enum class Mode { serialize, deserialize };

template <typename Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerialize;

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
    void archive_value(Value& value) const
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

    template <typename Type>
        requires(DefaultSerializable<Type>)
    void archive(Type& value) const
    {
        DefaultSerialize<Type> { }(value, *this);
    }

    template <typename SerializeType, typename Type>
        requires(Serialize<SerializeType, Type, Archive>)
    void archive(Type& value) const
    {
        std::invoke(SerializeType { }, value, *this);
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

template <typename Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerialize {
    void operator()(Type& value, const Archive& archive) const
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

template <typename T>
    requires(DefaultSerializable<T>)
void serialize_using_callback(T& value, WriteCallback write_callback)
{
    auto archive = Archive::create_serialize(&write_callback);
    archive(value);
}

template <typename T>
    requires(DefaultSerializable<T>)
void deserialize_using_callback(T& object, ReadCallback read_callback)
{
    auto archive = Archive::create_deserialize(&read_callback);
    object.serialize(archive);
}

template <typename T>
    requires(DefaultSerializable<T>)
std::vector<std::byte> serialize_to_vector(T& value)
{
    std::vector<std::byte> result;
    WriteCallback callback
        = [&result](std::span<const std::byte> bytes) { result.insert(result.end(), bytes.begin(), bytes.end()); };
    auto archive = Archive::create_serialize(&callback);
    archive.archive(value);
    return result;
}

template <typename T>
    requires(DefaultSerializable<T>)
void deserialize_from_span(T& value, std::span<const std::byte> bytes)
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
