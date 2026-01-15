#pragma once

#include <iostream>
#include <string>

void test_case(const std::string& name);

void test_section(const std::string& name);

void test_failed(const char* file, int line);

bool tests_succeeded();

#define END_TESTS                               \
    {                                           \
        if (tests_succeeded()) {                \
            std::cout << "All tests passed!\n"; \
            return EXIT_SUCCESS;                \
        }                                       \
        return EXIT_FAILURE;                    \
    }

#define ASSERT(expression) \
    if (!(expression))     \
        test_failed(__FILE__, __LINE__);

#define ASSERT_FALSE(expression) \
    if (expression)              \
        test_failed(__FILE__, __LINE__);
