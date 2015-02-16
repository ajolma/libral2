#include "ral2/ral.h"
#include "priv/priv.h"

const char *Object_cname()
{
    return "Object";
}

const struct Class _Object = {
    sizeof(struct ral_object),
    {NULL, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Object_cname,
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

ral_class ralObject = &_Object;

const struct ral_object _Undef = {
    &_Object,
    0,
    NULL
};

ral_object_t *ral_undef = (ral_object_t *)&_Undef;
