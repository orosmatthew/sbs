#pragma once

#include <algorithm>
#include <climits>
#include <cstddef>
#include <functional>
#include <span>

namespace sbs {

static_assert(CHAR_BIT == 8, "SBS only supports platforms with 8-bit bytes.");

class Archive;

template <typename T, typename S = Archive>
concept Serializable = requires(T& t, S& s) { t.serialize(s); };

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

    template <typename T>
        requires(sizeof(T) == 1 && std::is_fundamental_v<T>)
    void value8(T& v)
    {
        value(v, 1);
    }

    template <typename T>
        requires(sizeof(T) == 2 && std::is_fundamental_v<T>)
    void value16(T& v)
    {
        value(v, 2);
    }

    template <typename T>
        requires(sizeof(T) == 4 && std::is_fundamental_v<T>)
    void value32(T& v)
    {
        value(v, 4);
    }

    template <typename T>
        requires(sizeof(T) == 8 && std::is_fundamental_v<T>)
    void value64(T& v)
    {
        value(v, 8);
    }

    template <typename T>
        requires(!std::is_fundamental_v<T> && Serializable<T>)
    void object(T& o)
    {
        o.serialize(*this);
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

    template <typename T>
        requires(std::is_fundamental_v<T>)
    void value(T& value, const size_t size)
    {
        if (m_mode == Mode::serialize) {
            std::span<const std::byte> bytes = std::as_bytes(std::span<const T>(&value, 1));
            std::invoke(*m_write_callback, bytes);
        } else {
            std::span<const std::byte> source = std::invoke(*m_read_callback, size);
            std::span<std::byte> dest = std::as_writable_bytes(std::span<T>(&value, 1));
            std::ranges::copy(source, dest.begin());
        }
    }
};

template <typename T>
    requires(Serializable<T>)
void serialize_using_callback(T& object, WriteCallback write_callback)
{
    auto archive = Archive::create_serialize(&write_callback);
    object.serialize(archive);
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
std::vector<std::byte> serialize_to_vector(T& object)
{
    std::vector<std::byte> result;
    WriteCallback callback
        = [&result](std::span<const std::byte> bytes) { result.insert(result.end(), bytes.begin(), bytes.end()); };
    auto archive = Archive::create_serialize(&callback);
    object.serialize(archive);
    return result;
}

template <typename T>
    requires(Serializable<T>)
void deserialize_from_span(T& object, std::span<const std::byte> bytes)
{
    ReadCallback callback = [&bytes](const size_t size) {
        std::span<const std::byte> subspan = bytes.subspan(0, size);
        bytes = bytes.subspan(size, bytes.size() - size);
        return subspan;
    };
    auto archive = Archive::create_deserialize(&callback);
    object.serialize(archive);
}

}
