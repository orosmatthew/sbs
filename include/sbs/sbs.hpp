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
concept ValueSerializable = std::is_fundamental_v<Value>;

template <typename Object, typename Archive = Archive>
concept MethodSerializable = requires(Object& object, Archive& archive) { object.serialize(archive); };

template <typename Object, typename Archive = Archive>
concept FunctionSerializable = requires(Object& object, Archive& archive) { serialize(object, archive); };

template <typename Object, typename Archive = Archive>
concept ObjectSerializable = MethodSerializable<Object, Archive> || FunctionSerializable<Object, Archive>;

template <typename T, typename Archive = Archive>
concept Serializable = ValueSerializable<T> || ObjectSerializable<T, Archive>;

using WriteCallback = std::function<void(std::span<const std::byte>)>;
using ReadCallback = std::function<std::span<const std::byte>(size_t)>;

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

    template <typename Value>
        requires(ValueSerializable<Value> && sizeof(Value) == 1)
    void value8(Value& value)
    {
        value_common(value, 1);
    }

    template <typename Value>
        requires(ValueSerializable<Value> && sizeof(Value) == 2)
    void value16(Value& value)
    {
        value_common(value, 2);
    }

    template <typename Value>
        requires(ValueSerializable<Value> && sizeof(Value) == 4)
    void value32(Value& value)
    {
        value_common(value, 4);
    }

    template <typename Value>
        requires(ValueSerializable<Value> && sizeof(Value) == 8)
    void value64(Value& value)
    {
        value_common(value, 8);
    }

    template <typename Object>
        requires(ObjectSerializable<Object>)
    void object(Object& object)
    {
        if constexpr (MethodSerializable<Object>) {
            object.serialize(*this);
        } else if (FunctionSerializable<Object>) {
            serialize(object, *this);
        }
    }

    template <typename T>
        requires(Serializable<T>)
    void generic(T& value)
    {
        if constexpr (ValueSerializable<T>) {
            if constexpr (sizeof(T) == 1) {
                value8(value);
            } else if constexpr (sizeof(T) == 2) {
                value16(value);
            } else if constexpr (sizeof(T) == 4) {
                value32(value);
            } else if constexpr (sizeof(T) == 8) {
                value64(value);
            } else {
                static_assert(false, "Invalid value type size.");
            }
        } else if constexpr (ObjectSerializable<T>) {
            object(value);
        } else {
            static_assert(false, "Invalid Serializable type.");
        }
    }

private:
    enum class Mode { serialize, deserialize };

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

    template <typename Value>
        requires(ValueSerializable<Value>)
    void value_common(Value& value, const size_t size)
    {
        if (m_mode == Mode::serialize) {
            std::span<const std::byte> bytes = std::as_bytes(std::span<const Value>(&value, 1));
            std::invoke(*m_write_callback, bytes);
        } else {
            std::span<const std::byte> source = std::invoke(*m_read_callback, size);
            std::span<std::byte> dest = std::as_writable_bytes(std::span<Value>(&value, 1));
            std::ranges::copy(source, dest.begin());
        }
    }
};

template <typename T>
    requires(Serializable<T>)
void serialize_using_callback(T& value, WriteCallback write_callback)
{
    auto archive = Archive::create_serialize(&write_callback);
    archive.generic(value);
}

template <typename T>
    requires(Serializable<T>)
void deserialize_using_callback(T& object, ReadCallback read_callback)
{
    auto archive = Archive::create_deserialize(&read_callback);
    object.serialize(archive);
}

template <typename T>
    requires(Serializable<T>)
std::vector<std::byte> serialize_to_vector(T& value)
{
    std::vector<std::byte> result;
    WriteCallback callback
        = [&result](std::span<const std::byte> bytes) { result.insert(result.end(), bytes.begin(), bytes.end()); };
    auto archive = Archive::create_serialize(&callback);
    archive.generic(value);
    return result;
}

template <typename T>
    requires(Serializable<T>)
void deserialize_from_span(T& value, std::span<const std::byte> bytes)
{
    ReadCallback callback = [&bytes](const size_t size) {
        std::span<const std::byte> subspan = bytes.subspan(0, size);
        bytes = bytes.subspan(size, bytes.size() - size);
        return subspan;
    };
    auto archive = Archive::create_deserialize(&callback);
    archive.generic(value);
}

}
