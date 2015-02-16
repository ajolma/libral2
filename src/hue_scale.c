#include "ral2/ral.h"
#include "priv/priv.h"

const char *HueScale_cname()
{
    return "HueScale";
}

int HueScale_set_color_from_value(ral_object_t *rule, ral_object_t *obj, ral_object_t *val)
{
    return 1;
}

static const struct Class _HueScale = {
    sizeof(struct ral_hue_scale),
    {&_LinearInterpolation, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ HueScale_cname,
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
    /* 18 */ HueScale_set_color_from_value,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class HueScale = &_HueScale;
ral_class ralHueScale = &_HueScale;

