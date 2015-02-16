#include "ral2/ral.h"
#include "priv/priv.h"

int RCoords_set(ral_object_t *_self, ral_object_t *_set)
{
    ral_rcoords_t *self = _self;
    ral_rcoords_t *set = _set;
    if (class_of(set) == ralRCoords) {
        self->x = set->x;
        self->y = set->y;
        return 1;
    }
    return 0;
}

const char *RCoords_name(void *_self)
{
    return "RCoords";
}

static const struct Class _RCoords = {
    sizeof(struct ral_rcoords),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ RCoords_set,
    /*  5 */ NULL,
    /*  6 */ RCoords_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
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

ral_class RCoords = &_RCoords;
ral_class ralRCoords = &_RCoords;

ral_rcoords_t *ral_rcoords_new(int x, int y)
{
    ral_rcoords_t * r = ral_new(ralRCoords);
    r->x = x;
    r->y = y;
    return r;
}

void ral_rcoords_set(ral_rcoords_t * self, int x, int y)
{
    self->x = x;
    self->y = y;
}

int ral_rcoords_get_x(ral_rcoords_t * self)
{
    return self->x;
}

int ral_rcoords_get_y(ral_rcoords_t * self)
{
    return self->y;
}
