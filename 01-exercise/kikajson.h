#ifndef KIKAJSON_H__
#define KIKAJSON_H__

typedef enum { KIKA_NULL, KIKA_FALSE, KIKA_TRUE, KIKA_NUMBER, KIKA_STRING, KIKA_ARRAY, KIKA_OBJECT } kika_type;

typedef struct {
    kika_type type;
}kika_value;

enum {
    KIKA_PARSE_OK = 0,
    KIKA_PARSE_EXPECT_VALUE,
    KIKA_PARSE_INVALID_VALUE,
    KIKA_PARSE_ROOT_NOT_SINGULAR
};

int kika_parse(kika_value* v, const char* json);

kika_type kika_get_type(const kika_value* v);

#endif /* KIKAJSON_H__ */
