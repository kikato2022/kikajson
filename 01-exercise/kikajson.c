#include "kikajson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}kika_context;

static void kika_parse_whitespace(kika_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int kika_parse_null(kika_context* c, kika_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return KIKA_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = KIKA_NULL;
    return KIKA_PARSE_OK;
}

static int kika_parse_false(kika_context* c, kika_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return KIKA_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = KIKA_FALSE;
    return KIKA_PARSE_OK;
}

static int kika_parse_true(kika_context* c, kika_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return KIKA_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = KIKA_TRUE;
    return KIKA_PARSE_OK;
}

static int kika_parse_value(kika_context* c, kika_value* v) {
    switch (*c->json) {
        case 'n':  return kika_parse_null(c, v);
        case 'f':  return kika_parse_false(c, v);
        case 't':  return kika_parse_true(c, v);
        case '\0': return KIKA_PARSE_EXPECT_VALUE;
        default:   return KIKA_PARSE_INVALID_VALUE;
    }
}

int kika_parse(kika_value* v, const char* json) {
    kika_context c;
    assert(v != NULL);
    c.json = json;
    v->type = KIKA_NULL;
    kika_parse_whitespace(&c);
    int ret;
    if ((ret = kika_parse_value(&c, v) )== KIKA_PARSE_OK) {
        kika_parse_whitespace(&c);
        if (*c.json != '\0')
            return KIKA_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

kika_type kika_get_type(const kika_value* v) {
    assert(v != NULL);
    return v->type;
}
