#include <sbs/sbs.hpp>
#include <sbs/serializers/string.hpp>

struct Player {
    uint32_t id;
    std::string username;
    float score;

    void serialize(sbs::Archive& ar)
    {
        ar.archive(id);
        ar.archive(username);
        ar.archive(score);
    }
};

int main()
{
    Player my_player { .id = 37, .username = "Alice", .score = 95.5f };
    std::vector<std::byte> bytes = sbs::serialize_to_vector(my_player);

    Player player_out { };
    sbs::deserialize_from_span(bytes, player_out);
}
