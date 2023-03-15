#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kikajson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_PASS(Type, json)\
    do {\
        kika_value v;\
        v.type = KIKA_FALSE;\
        EXPECT_EQ_INT(KIKA_PARSE_OK, kika_parse(&v, json));\
        EXPECT_EQ_INT(Type, kika_get_type(&v));\
    } while(0)

static void test_parse_null() {
    TEST_PASS(KIKA_NULL, "null");
}

static void test_parse_false() {
    TEST_PASS(KIKA_FALSE, "false");
}

static void test_parse_true() {
    TEST_PASS(KIKA_TRUE, "true");
}

#define TEST_NUMBER(expect, json)\
    do {\
        kika_value v;\
        EXPECT_EQ_INT(KIKA_PARSE_OK, kika_parse(&v, json));\
        EXPECT_EQ_INT(KIKA_NUMBER, kika_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, kika_get_number(&v));\
    } while(0)

static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

#define TEST_ERROR(error, json)\
    do {\
        kika_value v;\
        v.type = KIKA_FALSE;\
        EXPECT_EQ_INT(error, kika_parse(&v, json));\
        EXPECT_EQ_INT(KIKA_NULL, kika_get_type(&v));\
    } while(0)

static void test_parse_expect_value() {
    TEST_ERROR(KIKA_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(KIKA_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "?");

#if 0
    /* invalid number */
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(KIKA_PARSE_INVALID_VALUE, "nan");
#endif
}

static void test_parse_root_not_singular() {
    TEST_ERROR(KIKA_PARSE_ROOT_NOT_SINGULAR, "null x");

#if 0
    /* invalid number */
    TEST_ERROR(KIKA_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
    TEST_ERROR(KIKA_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(KIKA_PARSE_ROOT_NOT_SINGULAR, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 0
    TEST_ERROR(KIKA_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(KIKA_PARSE_NUMBER_TOO_BIG, "-1e309");
#endif
}

static void test_parse() {
    test_parse_null();
    test_parse_false();
    test_parse_true();
    test_parse_number();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
