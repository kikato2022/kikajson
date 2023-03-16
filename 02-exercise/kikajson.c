#include "kikajson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <errno.h>   /* errno, ERANCE*/
#include <math.h>    /* HUGE_VAL*/

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}kika_context;

static void kika_parse_whitespace(kika_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int kika_parse_literal(kika_context* c, kika_value* v, const char* literal, kika_type type) {
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; i++)
        if (c->json[i] != literal[i + 1])
            return KIKA_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return KIKA_PARSE_OK;
}

static int kika_parse_number(kika_context* c, kika_value* v) {
    const char* p = c->json;
    /* \TODO validate number */
    // (optional) plus or minus sign
    if (*p == '-') p++;
    // nonempty sequence of decimal digits optionally containing decimal-point character
    if (*p == '0') 
        p++;
    else {
        if (!ISDIGIT1TO9(*p))
            return KIKA_PARSE_INVALID_VALUE;
        for (p++;ISDIGIT(*p);p++);
    }
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return KIKA_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    // (optional) e or E followed with optional minus or plus sign 
    // and nonempty sequence of decimal digits (defines exponent to base 10)
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return KIKA_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p);p++);
    }
    errno = 0;
    v->n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return KIKA_PARSE_NUMBER_TOO_BIG;

    v->type = KIKA_NUMBER;
    c->json = p;
    return KIKA_PARSE_OK;



    
    


    v->type = KIKA_NUMBER;
    return KIKA_PARSE_OK;
}

static int kika_parse_value(kika_context* c, kika_value* v) {
    switch (*c->json) {
        case 'n':  return kika_parse_literal(c, v, "null", KIKA_NULL);
        case 'f':  return kika_parse_literal(c, v, "false", KIKA_FALSE);
        case 't':  return kika_parse_literal(c, v, "true", KIKA_TRUE);
        case '\0': return KIKA_PARSE_EXPECT_VALUE;
        default:   return kika_parse_number(c, v);
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
        if (*c.json != '\0') {
            v->type = KIKA_NULL;
            return KIKA_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

kika_type kika_get_type(const kika_value* v) {
    assert(v != NULL);
    return v->type;
}

double kika_get_number(const kika_value* v) {
    assert(v != NULL && v->type == KIKA_NUMBER);
    return v->n;
}
