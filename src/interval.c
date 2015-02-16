#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Interval_new(ral_object_t *_self)
{
    ral_interval_t *self = _self;
    self->min = NULL;
    self->max = NULL;
    self->end_point_inclusion = 3;
    return _self;
}

ral_interval_t * ral_interval_new(ral_object_t *min, ral_object_t *max, int end_point_inclusion)
{
    ral_interval_t *i = ral_new(ralInterval);
    i->min = min;
    i->max = max;
    i->end_point_inclusion = end_point_inclusion;
    return i;
}

void Interval_delete(ral_object_t *_self)
{
    ral_interval_t *self = _self;
    if (!self->wrap) {
        ral_delete(self->min);
        ral_delete(self->max);
    }
}

int Interval_set(ral_object_t *_self, ral_object_t *_b)
{
    ral_interval_t *self = _self, *b = _b;
    if (class_of(b) != ralInterval)
        return 0;    
    if (!self->wrap) {
        ral_delete(self->min);
        ral_delete(self->max);
    }
    if (b->wrap) {
        self->wrap = 1;
        self->min = b->min;
        self->max = b->max;
        self->end_point_inclusion = b->end_point_inclusion;
    } else {
        self->wrap = 0;
        ral_set(self->min, b->min);
        ral_set(self->max, b->max);
        self->end_point_inclusion = b->end_point_inclusion;
    }
    return 1;
}

const char *Interval_name()
{
    return "Interval";
}

void Interval_to_string(ral_object_t *_self)
{
    ral_interval_t *self = _self;
    const char *min = ral_as_string(self->min);
    const char *max = ral_as_string(self->max);
    if (self->string) free(self->string);
    TEST(self->string = malloc(strlen(min)+strlen(max)+5));
    switch(self->end_point_inclusion) {
    case 0: 
        sprintf(self->string, "(%s..%s)", min, max);
        break;
    case 1: 
        sprintf(self->string, "[%s..%s)", min, max);
        break;
    case 2: 
        sprintf(self->string, "(%s..%s]", min, max);
        break;
    case 3: 
        sprintf(self->string, "[%s..%s]", min, max);
        break;
    }
fail:
    return;
}

static const struct Class _Interval = {
    sizeof(struct ral_interval),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Interval_new,
    /*  2 */ NULL,
    /*  3 */ Interval_delete,
    /*  4 */ Interval_set,
    /*  5 */ NULL,
    /*  6 */ Interval_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ Interval_to_string,
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

ral_class ralInterval = &_Interval;

ral_object_t *ral_interval_min(ral_interval_t * self)
{
    return self->min;
}

ral_object_t *ral_interval_max(ral_interval_t * self)
{
    return self->max;
}

int ral_interval_end_point_inclusion(ral_interval_t * self)
{
    return self->end_point_inclusion;
}
