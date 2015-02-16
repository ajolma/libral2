#include "ral2/ral.h"
#include "priv/priv.h"

int WCoords_set(ral_object_t *_self, ral_object_t *_set)
{
    ral_wcoords_t *self = _self;
    ral_wcoords_t *set = _set;
    if (class_of(set) == ralWCoords) {
        self->x = set->x;
        self->y = set->y;
        return 1;
    }
    return 0;
}

const char *WCoords_name(void *_self)
{
    return "WCoords";
}

static const struct Class _WCoords = {
    sizeof(struct ral_wcoords),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ WCoords_set,
    /*  5 */ NULL,
    /*  6 */ WCoords_name,
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

ral_class WCoords = &_WCoords;
ral_class ralWCoords = &_WCoords;

ral_wcoords_t * ral_wcoords_new(double x, double y)
{
    ral_wcoords_t * r = ral_new(ralWCoords);
    r->x = x;
    r->y = y;
    return r;
}

void ral_wcoords_set(ral_wcoords_t * self, double x, double y)
{
    self->x = x;
    self->y = y;
}

double ral_wcoords_get_x(ral_wcoords_t * self)
{
    return self->x;
}

double ral_wcoords_get_y(ral_wcoords_t * self)
{
    return self->y;
}
