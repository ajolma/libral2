#include "ral2/ral.h"
#include "priv/priv.h"

int WBox_set(ral_object_t *_self, ral_object_t *_set)
{
    ral_wbox_t *self = _self;
    ral_wbox_t *set = _set;
    if (class_of(set) == ralWBox) {
        self->x_min = set->x_min;
        self->y_min = set->y_min;
        self->x_max = set->x_max;
        self->y_max = set->y_max;
        return 1;
    }
    return 0;
}

const char *WBox_name(void *_self)
{
    return "WBox";
}

void WBox_to_string(ral_object_t *_self)
{
    ral_wbox_t * self = _self;
    unsigned len = 3;
    char *str;
    if (self->string) free(self->string);
    TEST(self->string = malloc(len));
    strcpy(self->string, "(");
    
    TEST(str = malloc(20));

    sprintf(str, "%f ", self->x_min);
    len += strlen(str)+1;
    TEST(self->string = realloc(self->string, len));
    strcat(self->string, str);

    sprintf(str, "%f ", self->y_min);
    len += strlen(str)+1;
    TEST(self->string = realloc(self->string, len));
    strcat(self->string, str);

    sprintf(str, "%f ", self->x_max);
    len += strlen(str)+1;
    TEST(self->string = realloc(self->string, len));
    strcat(self->string, str);

    sprintf(str, "%f", self->y_max);
    len += strlen(str)+1;
    TEST(self->string = realloc(self->string, len));
    strcat(self->string, str);

    free(str);

    strcat(self->string, ")");
fail:;
}

static const struct Class _WBox = {
    sizeof(struct ral_wbox),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ WBox_set,
    /*  5 */ NULL,
    /*  6 */ WBox_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ WBox_to_string,
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

ral_class WBox = &_WBox;
ral_class ralWBox = &_WBox;

ral_wbox_t * ral_wbox_new(double x_min, double y_min, double x_max, double y_max)
{
    ral_wbox_t * r = ral_new(ralWBox);
    if (r) {
        r->x_min = x_min;
        r->y_min = y_min;
        r->x_max = x_max;
        r->y_max = y_max;
        return r;
    } else
        return NULL;
}

double ral_wbox_get_x_min(ral_wbox_t * self)
{
    return self->x_min;
}

double ral_wbox_get_y_min(ral_wbox_t * self)
{
    return self->y_min;
}

double ral_wbox_get_x_max(ral_wbox_t * self)
{
    return self->x_max;
}

double ral_wbox_get_y_max(ral_wbox_t * self)
{
    return self->y_max;
}
