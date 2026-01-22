// [sbs] Simple Binary Serialization. A C++ Binary Serialization Library.
// MIT Licensed Copyright (c) 2026-present Matthew Oros
// v0.1.0

#ifndef SBS_HPP
#define SBS_HPP

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

namespace sbs {

static_assert(CHAR_BIT == 8, "SBS only supports platforms with 8-bit bytes.");

class Archive;

template <class Type>
concept IntegerSerializable
    = std::is_integral_v<Type> || (std::is_enum_v<Type> && std::is_integral_v<std::underlying_type_t<Type>>);

template <class Type>
concept FloatSerializable = std::is_floating_point_v<Type> && std::numeric_limits<Type>::is_iec559;

template <class Type>
concept ValueSerializable = IntegerSerializable<Type> || FloatSerializable<Type>;

template <class Type>
concept MethodSerializable = requires(Type type, Archive& archive) { type.serialize(archive); };

template <class Type>
concept FunctionSerializable = requires(Type type, Archive& archive) { serialize(archive, type); };

template <class Type>
concept ObjectSerializable = MethodSerializable<Type> || FunctionSerializable<Type>;

template <class Type>
concept DefaultSerializable = ValueSerializable<Type> || ObjectSerializable<Type>;

template <class SerializeType, class Type>
concept Serializer = requires(SerializeType serialize, Archive& archive, Type& value) {
    { serialize.operator()(archive, value) } -> std::same_as<void>;
} && std::is_default_constructible_v<SerializeType>;

using WriteCallback = std::function<void(std::span<const std::byte>)>;
using ReadCallback = std::function<std::span<const std::byte>(size_t)>;

template <class Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerializer;

class Archive {
public:
    static Archive create_serialize(WriteCallback write_callback, const std::endian endian)
    {
        return Archive(std::move(write_callback), endian);
    }

    static Archive create_deserialize(ReadCallback read_callback, const std::endian endian)
    {
        return Archive(std::move(read_callback), endian);
    }

    template <class Value>
        requires(ValueSerializable<Value>)
    void archive_value(Value& value)
    {
        if (m_mode == Mode::serialize) {
            std::span<const std::byte> bytes = std::as_bytes(std::span<const Value>(&value, 1));
            if (m_endian == std::endian::native) {
                m_write_callback(bytes);
            } else {
                std::array<std::byte, sizeof(Value)> reversed;
                std::ranges::copy(bytes | std::views::reverse, reversed.begin());
                m_write_callback(reversed);
            }
        } else {
            std::span<const std::byte> source = m_read_callback(sizeof(Value));
            if (source.size() < sizeof(Value)) {
                throw std::runtime_error("Read callback returned insufficient data");
            }
            std::span<std::byte> dest = std::as_writable_bytes(std::span<Value>(&value, 1));
            if (m_endian == std::endian::native) {
                std::ranges::copy(source, dest.begin());
            } else {
                std::ranges::copy(source | std::views::reverse, dest.begin());
            }
        }
    }

    template <class Type>
        requires(DefaultSerializable<Type>)
    void archive(Type& value)
    {
        DefaultSerializer<Type>()(*this, value);
    }

    template <class SerializeType, class Type>
        requires(Serializer<SerializeType, Type>)
    void archive(Type& value)
    {
        SerializeType()(*this, value);
    }

    template <class Type>
        requires(DefaultSerializable<Type> && std::copyable<Type>)
    void archive_copy(const Type& value)
    {
        Type copy = value;
        DefaultSerializer<Type>()(*this, copy);
    }

    template <class SerializeType, class Type>
        requires(Serializer<SerializeType, Type> && std::copyable<Type>)
    void archive_copy(const Type& value)
    {
        Type copy = value;
        SerializeType()(*this, copy);
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
    enum class Mode { serialize, deserialize };

    Mode m_mode;
    std::endian m_endian;
    WriteCallback m_write_callback { };
    ReadCallback m_read_callback { };

    explicit Archive(WriteCallback write_callback, const std::endian endian)
        : m_mode { Mode::serialize }
        , m_endian { endian }
        , m_write_callback { std::move(write_callback) }
    {
    }

    explicit Archive(ReadCallback read_callback, const std::endian endian)
        : m_mode { Mode::deserialize }
        , m_endian { endian }
        , m_read_callback { std::move(read_callback) }
    {
    }
};

template <class Type>
    requires(DefaultSerializable<Type>)
struct DefaultSerializer {
    void operator()(Archive& archive, Type& value) const
    {
        if constexpr (ValueSerializable<Type>) {
            archive.archive_value(value);
        } else if constexpr (MethodSerializable<Type>) {
            value.serialize(archive);
        } else if constexpr (FunctionSerializable<Type>) {
            serialize(archive, value);
        }
    }
};

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
void serialize_using_callback(Type& value, WriteCallback write_callback, const std::endian endian = std::endian::little)
{
    auto ar = Archive::create_serialize(std::move(write_callback), endian);
    ar.archive<TypeSerializer>(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void serialize_using_callback(Type& value, WriteCallback write_callback, const std::endian endian = std::endian::little)
{
    serialize_using_callback<DefaultSerializer<Type>>(value, std::move(write_callback), endian);
}

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
void deserialize_using_callback(Type& value, ReadCallback read_callback, const std::endian endian = std::endian::little)
{
    auto ar = Archive::create_deserialize(std::move(read_callback), endian);
    ar.archive<TypeSerializer>(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void deserialize_using_callback(Type& value, ReadCallback read_callback, const std::endian endian = std::endian::little)
{
    deserialize_using_callback<DefaultSerializer<Type>>(value, std::move(read_callback), endian);
}

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
std::vector<std::byte> serialize_to_vector(Type& value, std::endian endian = std::endian::little)
{
    std::vector<std::byte> result;
    auto ar = Archive::create_serialize(
        [&result](std::span<const std::byte> bytes) { result.insert(result.end(), bytes.begin(), bytes.end()); },
        endian);
    ar.template archive<TypeSerializer>(value);
    return result;
}

template <class Type>
    requires(DefaultSerializable<Type>)
std::vector<std::byte> serialize_to_vector(Type& value, std::endian endian = std::endian::little)
{
    return serialize_to_vector<DefaultSerializer<Type>>(value, endian);
}

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
void deserialize_from_span(std::span<const std::byte> bytes, Type& value, std::endian endian = std::endian::little)
{
    auto ar = Archive::create_deserialize(
        [&bytes](const size_t size) {
            if (bytes.size() < size) {
                throw std::runtime_error("Insufficient data to deserialize");
            }
            std::span<const std::byte> subspan = bytes.subspan(0, size);
            bytes = bytes.subspan(size, bytes.size() - size);
            return subspan;
        },
        endian);
    ar.template archive<TypeSerializer>(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void deserialize_from_span(std::span<const std::byte> bytes, Type& value, std::endian endian = std::endian::little)
{
    deserialize_from_span<DefaultSerializer<Type>>(bytes, value, endian);
}

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
void serialize_to_file(const std::filesystem::path& path, Type& value, std::endian endian = std::endian::little)
{
    std::ofstream file { path, std::ios::binary };
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + path.string());
    }
    auto ar = Archive::create_serialize(
        [&path, &file](const std::span<const std::byte> bytes) {
            file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
            if (file.bad()) {
                throw std::runtime_error("Error writing to file: " + path.string());
            }
        },
        endian);
    ar.template archive<TypeSerializer>(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void serialize_to_file(const std::filesystem::path& path, Type& value, std::endian endian = std::endian::little)
{
    serialize_to_file<DefaultSerializer<Type>>(path, value, endian);
}

template <class TypeSerializer, class Type>
    requires(Serializer<TypeSerializer, Type>)
void deserialize_from_file(const std::filesystem::path& path, Type& value, std::endian endian = std::endian::little)
{
    std::ifstream file { path, std::ios::binary };
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + path.string());
    }
    std::vector<std::byte> buffer;
    auto ar = Archive::create_deserialize(
        [&path, &file, &buffer](const size_t size) {
            if (buffer.size() < size) {
                buffer.resize(size);
            }
            file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));
            if (file.bad()) {
                throw std::runtime_error("Error reading file: " + path.string());
            }
            const std::streamsize bytes_read = file.gcount();
            return std::span<const std::byte>(buffer.data(), bytes_read);
        },
        endian);
    ar.template archive<TypeSerializer>(value);
}

template <class Type>
    requires(DefaultSerializable<Type>)
void deserialize_from_file(const std::filesystem::path& path, Type& value, std::endian endian = std::endian::little)
{
    deserialize_from_file<DefaultSerializer<Type>>(path, value, endian);
}

}

#endif // SBS_HPP