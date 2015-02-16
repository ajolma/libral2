#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *LinearInterpolation_new(ral_object_t *_self)
{
    ral_linear_interpolation_t *self = _self;    
    return (ral_object_t *)self;
}

ral_linear_interpolation_t *ral_linear_interpolation_new(ral_data_point_t * a, ral_data_point_t * b)
{
    ral_linear_interpolation_t *i = ral_new(ralLinearInterpolation);
    if (i) {
        i->a = a;
        i->b = b;
    }
    return i;
}

void LinearInterpolation_delete(ral_object_t *_self)
{
    ral_linear_interpolation_t *self = _self;
    if (!self->wrap) {
        ral_delete((ral_object_t *)self->a);
        ral_delete((ral_object_t *)self->b);
    }
}

int LinearInterpolation_set(ral_object_t *_self, ral_object_t *_b)
{
    ral_linear_interpolation_t *self = _self, *b = _b;
    if (class_of(b) != ralLinearInterpolation) return 0;
    if (!self->wrap) {
        ral_delete((ral_object_t *)self->a);
        ral_delete((ral_object_t *)self->b);
    }
    if (b->wrap) {
        self->wrap = 1;
        self->a = b->a;
        self->b = b->b;
    } else {
        self->wrap = 0;
        ral_set((ral_object_t *)self->a, (ral_object_t *)b->a);
        ral_set((ral_object_t *)self->b, (ral_object_t *)b->b);
    }
    return 1;
}

const char *LinearInterpolation_name()
{
    return "LinearInterpolation";
}

void LinearInterpolation_to_string(ral_object_t *_self)
{
    ral_linear_interpolation_t *self = _self;
    if (self->string) free(self->string);
}

ral_object_t *LinearInterpolation_lookup(ral_object_t *_self, ral_object_t *key)
{
    ral_linear_interpolation_t *self = _self;
    if (class_of(key) == ralReal) { /* now assuming reals but x axis could be time, .. */
        double x = ral_as_real(key);
        double x0 = ral_as_real(self->a->x);
        double x1 = ral_as_real(self->b->x);
        double y0 = ral_as_real(self->a->y);
        double y1 = ral_as_real(self->b->y);
        double y = y0 + (x-x0)*(y1-y0)/(x1-x0);
        return ral_real_new(y);
    } else
        return NULL;
}

int LinearInterpolation_set_object_from_value(ral_object_t *rule, ral_object_t *obj, ral_object_t *val)
{
    return 1;
}

ral_class ral_linear_interpolation_get_output_type(ral_object_t *_self)
{
    ral_linear_interpolation_t *self = _self;
    ral_class ret = class_of(self->a);
    return ret;
}

const struct Class _LinearInterpolation = {
    sizeof(struct ral_linear_interpolation),
    {&_Classifier, NULL, NULL, NULL},
    /*  1 */ LinearInterpolation_new,
    /*  2 */ NULL,
    /*  3 */ LinearInterpolation_delete,
    /*  4 */ LinearInterpolation_set,
    /*  5 */ NULL,
    /*  6 */ LinearInterpolation_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ LinearInterpolation_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ LinearInterpolation_lookup,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ LinearInterpolation_set_object_from_value,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ ral_linear_interpolation_get_output_type
};

ral_class ralLinearInterpolation = &_LinearInterpolation;

void ral_linear_interpolation_set_data_points(ral_linear_interpolation_t * self, ral_data_point_t * p0, ral_data_point_t * p1)
{
    self->a = p0;
    self->b = p1;    
    if (class_of(self->a->x) == ralReal && 
        class_of(self->a->y) == ralReal &&
        class_of(self->b->x) == ralReal && 
        class_of(self->b->y) == ralReal) {
        double dx = ((ral_real_t *)(self->b->x))->value - ((ral_real_t *)(self->a->x))->value;
        double dy = ((ral_real_t *)(self->b->y))->value - ((ral_real_t *)(self->a->y))->value;
        self->min = MIN(((ral_real_t *)(self->b->y))->value, ((ral_real_t *)(self->a->y))->value);
        self->max = MAX(((ral_real_t *)(self->b->y))->value, ((ral_real_t *)(self->a->y))->value);
        self->k = dx == 0 ? 0 : dy / dx;
        /*fprintf(stdout, "interpolate: k=%f x=%f,%f y=%f,%f\n", 
          self->k, ((ral_real_t *)(self->a->x))->value, ((ral_real_t *)(self->b->x))->value, 
          ((ral_real_t *)(self->a->y))->value, ((ral_real_t *)(self->b->y))->value);*/
    } else {
        self->min = 0;
        self->max = 0;
        self->k = 0;
    }
}


ral_object_t *ral_interpolate(ral_linear_interpolation_t *m, ral_object_t *x)
{
    /*fprintf(stdout, "interpolate: k=%f x=%f\n", m->k, ((ral_real_t *)(x))->value);*/
    if (class_of(x) == ralReal) {
        return ral_real_new( 
            MAX(MIN( 
                    ((ral_real_t *)(m->a->y))->value + m->k*(((ral_real_t *)(x))->value - 
                                                       ((ral_real_t *)(m->a->x))->value), 
                    m->max), m->min));
    } else
        return NULL;
}
