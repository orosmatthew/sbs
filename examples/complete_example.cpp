#include <sbs/sbs.hpp>

#include <sbs/serializers/chrono.hpp>
#include <sbs/serializers/optional.hpp>
#include <sbs/serializers/string.hpp>
#include <sbs/serializers/variant.hpp>
#include <sbs/serializers/vector.hpp>

#include <cmath>

struct User {
    uint32_t id;
    std::string name;
    std::optional<float> rating;
    std::vector<std::chrono::system_clock::time_point> login_history;

    void serialize(sbs::Archive& ar)
    {
        ar.archive(id);
        ar.archive(name);
        ar.archive(rating);
        ar.archive(login_history);
    }
};

static_assert(sbs::MethodSerializable<User>);

struct GameSave {
    uint64_t timestamp;
    std::variant<int, std::string, bool> player_state;
};

void serialize(sbs::Archive& ar, GameSave& save)
{
    // Example separate serialization/deserialization logic.
    if (ar.serializing()) {
        uint64_t time = std::chrono::system_clock::now().time_since_epoch().count();
        ar.archive(time);
        ar.archive(save.player_state);
    } else { // ar.deserializing() == true
        ar.archive(save.timestamp);
        ar.archive(save.player_state);
    }
}

static_assert(sbs::FunctionSerializable<GameSave>);

struct ThirdPartyData {
    double value;
};

struct ThirdPartyDataSerializer {
    void operator()(sbs::Archive& ar, ThirdPartyData& data) const
    {
        // Example serialization validation.
        if (ar.serializing()) {
            if (std::isnan(data.value)) {
                throw std::runtime_error("cannot serialize NaN values");
            }
        }

        ar.archive(data.value);

        // Example deserialization validation.
        if (ar.deserializing()) {
            if (std::isnan(data.value)) {
                throw std::runtime_error("deserialized NaN value");
            }
        }
    }
};

static_assert(sbs::Serializer<ThirdPartyDataSerializer, ThirdPartyData>);

void profile_example()
{
    User profile { .id = 12345,
                   .name = "Alice",
                   .rating = 4.5f,
                   .login_history
                   = { std::chrono::system_clock::now(), std::chrono::system_clock::now() - std::chrono::hours(24) } };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(profile, std::endian::big);

    User profile_out { };
    sbs::deserialize_from_span(bytes, profile_out, std::endian::big);
    // profile == profile_out

    // Example reading/writing using callback.
    /*

    sbs::serialize_using_callback(
        profile,
        [](const std::span<const std::byte> bytes) {
            // Example write to TCP socket.
            socket.write(bytes);
        },
        std::endian::big);

    sbs::deserialize_using_callback(profile, [](const size_t bytes_requested) {
        // Example read from TCP socket.
        std::span<const std::byte> bytes = socket.read(bytes_requested);
        return bytes;
    }, std::endian::big);

    */
}

void game_save_example()
{
    GameSave save { .timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count()),
                    .player_state = "playing" };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(save);

    GameSave save_out { };
    sbs::deserialize_from_span(bytes, save_out);
    // save == save_out
}

void third_party_example()
{
    ThirdPartyData data { .value = 3.14159 };
    std::vector<std::byte> bytes = sbs::serialize_to_vector<ThirdPartyDataSerializer>(data);

    ThirdPartyData data_out { };
    sbs::deserialize_from_span<ThirdPartyDataSerializer>(bytes, data_out);
    // data == data_out
}

int main()
{
    profile_example();
    game_save_example();
    third_party_example();
}
