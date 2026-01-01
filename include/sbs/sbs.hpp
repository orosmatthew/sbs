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

class Archive {
public:
    static Archive create_serialize(std::function<void(std::span<const std::byte>)>* write_callback)
    {
        return Archive(write_callback);
    }

    static Archive create_deserialize(const std::span<const std::byte> bytes)
    {
        return Archive(bytes);
    }

    template <typename T>
        requires(sizeof(T) <= 1 && std::is_fundamental_v<T>)
    void value8(T& v)
    {
        value(v, 1);
    }

    template <typename T>
        requires(sizeof(T) <= 2 && std::is_fundamental_v<T>)
    void value16(T& v)
    {
        value(v, 2);
    }

    template <typename T>
        requires(sizeof(T) <= 4 && std::is_fundamental_v<T>)
    void value32(T& v)
    {
        value(v, 4);
    }

    template <typename T>
        requires(sizeof(T) <= 8 && std::is_fundamental_v<T>)
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
    std::function<void(std::span<const std::byte>)>* m_write_callback { };
    std::span<const std::byte> m_bytes { };

    explicit Archive(std::function<void(std::span<const std::byte>)>* write_callback)
        : m_mode { Mode::serialize }
        , m_write_callback { write_callback }
    {
    }

    explicit Archive(const std::span<const std::byte> bytes)
        : m_mode { Mode::deserialize }
        , m_bytes { bytes }
    {
    }

    template <typename T>
        requires(std::is_fundamental_v<T>)
    void value(T& value, const size_t size)
    {
        if (m_mode == Mode::serialize) {
            std::span<const std::byte> bytes = std::as_bytes(std::span<const T>(&value, 1));
            // TODO: pad if size > sizeof T.
            std::invoke(*m_write_callback, bytes);
        } else {
            // TODO: assert enough bytes exist.
            std::span<std::byte> bytes = std::as_writable_bytes(std::span<T>(&value, 1));
            std::ranges::copy(m_bytes.subspan(0, size), bytes.begin());
            m_bytes = m_bytes.subspan(size, m_bytes.size() - size);
        }
    }
};

class Serializer {
public:
    explicit Serializer(std::function<void(std::span<const std::byte>)> write_callback)
        : m_write_callback { std::move(write_callback) }
    {
    }

    template <typename T>
        requires(Serializable<T>)
    void serialize(T& object)
    {
        auto archive = Archive::create_serialize(&m_write_callback);
        object.serialize(archive);
    }

private:
    std::function<void(std::span<const std::byte>)> m_write_callback;
};

class Deserializer {
public:
    explicit Deserializer(const std::span<const std::byte> bytes)
        : m_bytes { bytes }
    {
    }

    template <typename T>
        requires(Serializable<T>)
    void deserialize(T& object)
    {
        auto archive = Archive::create_deserialize(m_bytes);
        object.serialize(archive);
    }

private:
    std::span<const std::byte> m_bytes;
};

}
