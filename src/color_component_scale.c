#include "ral2/ral.h"
#include "priv/priv.h"

const char *ColorComponentScale_cname()
{
    return "ColorComponentScale";
}

int ColorComponentScale_set_color_from_value(ral_object_t *rule, ral_object_t *obj, ral_object_t *val)
{
    return 1;
}

static const struct Class _ColorComponentScale = {
    sizeof(struct ral_color_component_scale),
    {&_LinearInterpolation, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ ColorComponentScale_cname,
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
    /* 18 */ ColorComponentScale_set_color_from_value,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralColorComponentScale = &_ColorComponentScale;
