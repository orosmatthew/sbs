#include "basic_tests.hpp"
#include "std_tests.hpp"

#include "test_helper.hpp"

int main()
{
#ifdef SBS_WRITE_TEST_FILES
    clean_test_files();
#endif

    test_endianness();
    serialize_ints();
    serialize_chars();
    serialize_bool();
    serialize_floats();
    serialize_enum();
    serialize_function_serializable();
    serialize_nested_structs();

    serialize_array();
    serialize_bitset();
    serialize_chrono();
    serialize_complex();
    serialize_deque();
    serialize_filesystem();
    serialize_forward_list();
    serialize_list();
    serialize_map();
    serialize_memory();
    serialize_optional();
    serialize_set();
    serialize_string();
    serialize_unordered_map();
    serialize_unordered_set();
    serialize_utility();
    serialize_variant();

    END_TESTS;
}
