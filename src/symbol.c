#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Symbol_new(ral_object_t *_self)
{
    ral_symbol_t * self = _self;
    self->wkt = NULL;
    return _self;
}

void Symbol_delete(ral_object_t *_self)
{
    ral_symbol_t * self = _self;
    if (!self->wrap && self->wkt)
        free(self->wkt);
}

int Symbol_set(ral_object_t *_self, ral_object_t *_b)
{
    ral_symbol_t * self = _self, *b = _b;
    if (class_of(b) != ralSymbol) return 0;
    if (!self->wrap) free(self->wkt);
    if (b->wrap) {
        self->wrap = 1;
        self->wkt = b->wkt;
    } else {
        self->wrap = 0;
        TEST(self->wkt = malloc(strlen(b->wkt) + 1));
        strcpy(self->wkt, b->wkt);
    }
fail:
    return 1;
}

const char *Symbol_name()
{
    return "Symbol";
}

long Symbol_as_int(ral_object_t *_self)
{
    return 0;
}

void Symbol_to_string(ral_object_t *_self)
{
    ral_symbol_t * self = _self;
    if (self->string) free(self->string);
    TEST(self->string = malloc(strlen(self->wkt) + 1));
    strcpy(self->string, self->wkt);
fail:
    return;
}

const struct Class _Symbol = {
    sizeof(struct ral_symbol),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Symbol_new,
    /*  2 */ NULL,
    /*  3 */ Symbol_delete,
    /*  4 */ Symbol_set,
    /*  5 */ NULL,
    /*  6 */ Symbol_name,
    /*  7 */ Symbol_as_int,
    /*  8 */ NULL,
    /*  9 */ Symbol_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class Symbol = &_Symbol;
ral_class ralSymbol = &_Symbol;

void ral_symbol_set_built_in(ral_symbol_t *self, const char *symbol)
{
    if (!self->wrap && self->wkt)
        free(self->wkt);
    self->wrap = 1;
    self->wkt = (char*)symbol;
}
