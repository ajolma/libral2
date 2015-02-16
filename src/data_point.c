#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *DataPoint_new(ral_object_t *_self)
{
    ral_data_point_t * self = _self;
    self->x = NULL;
    self->y = NULL;
    return (void *)self;
}

ral_data_point_t * ral_data_point_new(ral_object_t *x, ral_object_t *y)
{
    ral_data_point_t * p = ral_new(ralDataPoint);
    if (p) {
        p->x = x;
        p->y = y;
    }
    return p;
}

void DataPoint_delete(ral_object_t *_self)
{
    ral_data_point_t * self = _self;
    if (!self->wrap) {
        ral_delete(self->x);
        ral_delete(self->y);
    }
}

int DataPoint_set(ral_object_t *_self, ral_object_t *_b)
{
    ral_data_point_t * self = _self, *b = _b;
    if (class_of(b) != ralDataPoint) return 0;
    if (!self->wrap) {
        ral_delete(self->x);
        ral_delete(self->y);
    }
    if (b->wrap) {
        self->wrap = 1;
        self->x = b->x;
        self->y = b->y;
    } else {
        self->wrap = 0;
        ral_set(self->x, b->x);
        ral_set(self->y, b->y);
    }
    return 1;
}

const char *DataPoint_name()
{
    return "DataPoint";
}

void DataPoint_to_string(ral_object_t *_self)
{
    ral_data_point_t * self = _self;
    const char *x = ral_as_string(self->x);
    const char *y = ral_as_string(self->y);
    if (self->string) free(self->string);
    TEST(self->string = malloc(strlen(x)+strlen(y)+2));
    sprintf(self->string, "%s,%s", x, y);
fail:
    return;
}

int DataPoint_insert(ral_object_t *_self, ral_object_t *object, ral_object_t *key)
{
    ral_data_point_t * self = _self;
    int index = ral_as_int(key);
    if (index < 0) return 0;
    if (index == 0) 
        self->x = object;
    else if (index == 1) 
        self->y = object;
    return 1;
}

static const struct Class _DataPoint = {
    sizeof(struct ral_data_point),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ DataPoint_new,
    /*  2 */ NULL,
    /*  3 */ DataPoint_delete,
    /*  4 */ DataPoint_set,
    /*  5 */ NULL,
    /*  6 */ DataPoint_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ DataPoint_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ DataPoint_insert,
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

ral_class ralDataPoint = &_DataPoint;
