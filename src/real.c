#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Real_new(ral_object_t *_self)
{
    ral_real_t *self = _self;
    self->value = 1.0;
    return self;
}

int Real_set(ral_object_t *self, ral_object_t *set)
{
    if (class_of(set) == ralReal) {
        ((ral_real_t *)self)->value = ((ral_real_t *)set)->value;
        return 1;
    }
    return 0;
}

const char *Real_name()
{
    return "Real";
}

long Real_as_int(ral_object_t *_self)
{
    ral_real_t * self = (ral_real_t *)_self;
    return ROUND(self->value);
}

double Real_as_real(ral_object_t *_self)
{
    ral_real_t * self = (ral_real_t *)_self;
    return self->value;
}

void Real_to_string(ral_object_t *_self)
{
    ral_real_t * self = (ral_real_t *)_self;
    if (self->string) free(self->string);
    TEST(self->string = malloc(20));
    sprintf(self->string, "%f", self->value);
fail:
    return;
}

int Real_cmp(ral_object_t *_self, ral_object_t *_b)
{
    ral_real_t * self = (ral_real_t *)_self;
    ral_real_t * b = (ral_real_t *)_b;
    if (self->value < b->value) return -1;
    if (self->value > b->value) return 1;
    return 0;
}

const struct Class _Real = {
    sizeof(struct ral_real),
    {&_Ordinal, NULL, NULL, NULL},
    /*  1 */ Real_new,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ Real_set,
    /*  5 */ NULL,
    /*  6 */ Real_name,
    /*  7 */ Real_as_int,
    /*  8 */ Real_as_real,
    /*  9 */ Real_to_string,
    /* 10 */ NULL,
    /* 11 */ Real_cmp,
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

ral_class ralReal = &_Real;

ral_real_t * ral_real_new(double value)
{
    ral_real_t * x = ral_new(ralReal);
    if (x)
        x->value = value;
    return x;
}

