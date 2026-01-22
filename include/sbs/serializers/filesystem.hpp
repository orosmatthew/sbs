#ifndef SBS_SERIALIZERS_FILESYSTEM_HPP
#define SBS_SERIALIZERS_FILESYSTEM_HPP

#include <sbs/sbs.hpp>

#include <sbs/serializers/string.hpp>

#include <filesystem>

namespace sbs {

struct FilesystemPathSerializer {
    void operator()(Archive& ar, std::filesystem::path& path) const
    {
        if (ar.serializing()) {
            std::u8string u8string = path.u8string();
            ar.archive(u8string);
        } else {
            path.clear();
            std::u8string u8string;
            ar.archive(u8string);
            path = u8string;
        }
    }
};

inline void serialize(Archive& ar, std::filesystem::path& path)
{
    FilesystemPathSerializer()(ar, path);
}

}

#endif // SBS_SERIALIZERS_FILESYSTEM_HPP