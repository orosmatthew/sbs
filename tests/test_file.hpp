#pragma once

#include <sbs/sbs.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

inline void clean_test_files()
{

    assert(std::filesystem::exists("tests/files/"));
    for (const auto& entry : std::filesystem::directory_iterator("tests/files/")) {
        std::filesystem::remove_all(entry.path());
    }
}

template <class T, class Equal = std::equal_to<T>>
void test_file(
    const std::string& name,
    const T& value,
    const std::vector<std::byte>& data,
    std::endian endian = std::endian::little)
{
    const std::filesystem::path path = "tests/files/" + name + ".bin";
#ifdef SBS_WRITE_TEST_FILES
    assert(!std::filesystem::exists(path));
    std::ofstream file { path, std::ios::binary };
    file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
#else
    const uintmax_t file_size = std::filesystem::file_size(path);
    std::vector<std::byte> file_data { file_size };
    std::ifstream file { path, std::ios::binary };
    assert(file.is_open());
    file.read(reinterpret_cast<char*>(file_data.data()), static_cast<std::streamsize>(file_size));
    auto value_out = T();
    sbs::deserialize_from_span(file_data, value_out, endian);
    const bool equal = Equal()(value, value_out);
    TEST_ASSERT(equal);
    if (!equal) {
        std::cerr << "\t file: " << name << std::endl;
    }
#endif
}