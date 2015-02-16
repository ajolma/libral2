#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Integer_new(ral_object_t *_self)
{
    ral_integer_t *self = _self;
    self->value = 1.0;
    return self;
}

int Integer_set(ral_object_t *self, ral_object_t *b)
{
    if (class_of(b) == ralInteger) {
        ((ral_integer_t *)self)->value = ((ral_integer_t *)b)->value;
        return 1;
    }
    return 0;
}

const char *Integer_name()
{
    return "Integer";
}

long Integer_as_int(ral_object_t *_self)
{
    ral_integer_t *self = (ral_integer_t *)_self;
    return self->value;
}

double Integer_as_real(ral_object_t *_self)
{
    ral_integer_t *self = (ral_integer_t *)_self;
    return self->value;
}

void Integer_to_string(ral_object_t *_self)
{
    ral_integer_t *self = (ral_integer_t *)_self;
    if (self->string) free(self->string);
    TEST(self->string = malloc(20));
    sprintf(self->string, "%li", self->value);
fail:
    return;
}

int Integer_cmp(ral_object_t  *_self, ral_object_t  *_b)
{
    ral_integer_t *self = (ral_integer_t *)_self;
    ral_integer_t *b = (ral_integer_t*)_b;
    return self->value - b->value;
}

const struct Class _Integer = {
    sizeof(struct ral_integer),
    {&_Ordinal, NULL, NULL, NULL},
    /*  1 */ Integer_new,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ Integer_set,
    /*  5 */ NULL,
    /*  6 */ Integer_name,
    /*  7 */ Integer_as_int,
    /*  8 */ Integer_as_real,
    /*  9 */ Integer_to_string,
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

ral_class ralInteger = &_Integer;

ral_integer_t *ral_integer_new(long value)
{
    ral_integer_t *i = ral_new(ralInteger);
    if (i)
        i->value = value;
    return i;
}

ral_integer_t *ral_integer_set(ral_integer_t *self, long value)
{
    self->value = value;
    return self;
}
