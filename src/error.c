#include "ral2/ral.h"
#include "priv/priv.h"

ral_error_t *ral_error_new(int level, char *format, ...)
{
    ral_error_t *self = ral_new(ralError);
    TEST(self);
    TEST(self->string = malloc(100));
    self->level = level;
    va_list ap;
    va_start(ap, format);
    vsnprintf(self->string, 99, format, ap);
    va_end(ap);
    return self;
fail:
    return NULL;
}

void _ral_error_to_string(ral_object_t *_self)
{
}

int _ral_error_insert(ral_object_t *_self, ral_object_t *item, ral_object_t *key) 
{ 
    ral_error_t *self = (ral_error_t *)_self;
    const char *msg = ral_as_string(key);
    int n = 99-strlen(self->string);
    strncat(self->string, "\n", n);
    n--;
    strncat(self->string, msg, n);
    return 1;
}

static const struct Class _Error = {
    sizeof(struct ral_error),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ _ral_error_to_string,
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

ral_class ralError = &_Error;

int ral_error_level(ral_error_t *e)
{
    return e->level;
}
