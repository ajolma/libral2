#include "ral2/ral.h"
#include "priv/priv.h"

const char *Grayscale_cname()
{
    return "Grayscale";
}

int Grayscale_set_color_from_value(ral_object_t *rule, ral_object_t *obj, ral_object_t *val)
{
    ral_grayscale_t *self = rule;
    ral_color_t * color = (ral_color_t *)obj;
    double x = ral_as_real(val);
    double x0 = ral_as_real(self->super.a->x);
    double x1 = ral_as_real(self->super.b->x);
    double y0 = ral_as_real(self->super.a->y);
    double y1 = ral_as_real(self->super.b->y);
    double y = y0 + (x-x0)*(y1-y0)/(x1-x0);
    color->red = color->green = color->blue = round(y);
    color->alpha = 255;
    /*fprintf(stderr, "gray: %f %f %f, %f %f %f\n", x0, x, x1, y0, y, y1);*/
    return 1;
}

static const struct Class _Grayscale = {
    sizeof(struct ral_grayscale),
    {&_LinearInterpolation, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Grayscale_cname,
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
    /* 18 */ Grayscale_set_color_from_value,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralGrayscale = &_Grayscale;
