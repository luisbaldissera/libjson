#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <assert.h>
#include <stdio.h>

#define ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            fprintf(stderr, "Assertion failed: %s == %s\n", #expected, #actual); \
            assert(0); \
        } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "Assertion failed: %s is NULL\n", #ptr); \
            assert(0); \
        } \
    } while (0)

#endif // TEST_UTILS_H