#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *String_new(ral_object_t *_self)
{
    ral_string_t * self = (ral_string_t *)_self;
    self->text = NULL;
    return _self;
}

ral_string_t *ral_string_new(const char *str)
{
    ral_string_t * self = ral_new(ralString);
    TEST(self);
    TEST(self->text = malloc(strlen(str) + 1));
    strcpy(self->text, str);
    return self;
fail:
    return NULL;
}

ral_string_t *ral_string_new_wrap(const char *str)
{
    ral_string_t * self = ral_new(ralString);
    self->text = (char *)str;
    self->wrap = 1;
    return self;
}

ral_string_t * ral_string_wrap(ral_string_t * self, const char *str)
{
    if (!self->wrap && self->text) free(self->text);
    self->wrap = 1;
    self->text = (char*)str;
    return self;
}

void String_delete(ral_object_t *_self)
{
    ral_string_t * self = (ral_string_t *)_self;
    if (!self->wrap && self->text)
        free(self->text);
}

int String_set(ral_object_t *_self, ral_object_t *_b)
{
    ral_string_t *self = _self, *b = _b;
    if (class_of(_b) != ralString) return 0;
    if (!self->wrap) free(self->text);
    if (b->wrap) {
        self->wrap = 1;
        self->text = b->text;
    } else {
        self->wrap = 0;
        TEST(self->text = malloc(strlen(b->text) + 1));
        strcpy(self->text, b->text);
    }
fail:
    return 1;
}

ral_string_t * ral_string_set(ral_string_t * self, const char *str)
{
    if (!self->wrap && self->text) free(self->text);
    self->wrap = 0;
    TEST(self->text = malloc(strlen(str) + 1));
    strcpy(self->text, str);
    return self;
fail:
    return NULL;
}

ral_object_t *String_clone(ral_object_t *_self)
{
    ral_string_t * self = _self;
    ral_string_t * s = ral_new(ralString);
    ral_string_set(s, self->text);
    return (ral_object_t *)s;
}

const char *String_cname()
{
    return "String";
}

long String_as_int(ral_object_t *_self)
{
    const ral_string_t * self = (ral_string_t *)_self;
    return atoi(self->text);
}

double String_as_real(ral_object_t *_self)
{
    const ral_string_t * self = (ral_string_t *)_self;
    return atof(self->text);
}

void String_to_string(ral_object_t *_self)
{
    ral_string_t * self = (ral_string_t *)_self;
    if (self->string) {
        free(self->string);
        self->string = NULL;
    }
    if (self->text) {    
        TEST(self->string = malloc(strlen(self->text) + 1));
        strcpy(self->string, self->text);
    }
fail:
    return;
}

int String_cmp(ral_object_t *_self, ral_object_t *_b)
{
    ral_string_t * self = (ral_string_t *)_self;
    ral_string_t * b = (ral_string_t *)_b;
    return strcmp(self->text, b->text);
}

unsigned String_size(ral_object_t *_self)
{
    const ral_string_t * self = (ral_string_t *)_self;
    if (!self->text) return 0;
    return strlen(self->text);   
}

static const struct Class _String = {
    sizeof(struct ral_string),
    {&_Ordinal, NULL, NULL, NULL},
    /*  1 */ String_new,
    /*  2 */ NULL,
    /*  3 */ String_delete,
    /*  4 */ String_set,
    /*  5 */ NULL,
    /*  6 */ String_cname,
    /*  7 */ String_as_int,
    /*  8 */ String_as_real,
    /*  9 */ String_to_string,
    /* 10 */ NULL,
    /* 11 */ String_cmp,
    /* 12 */ NULL,
    /* 13 */ String_size,
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

ral_class String = &_String;
ral_class ralString = &_String;
