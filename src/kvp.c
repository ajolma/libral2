#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *KVP_new(ral_object_t *_self)
{
    ral_kvp_t * self = (ral_kvp_t *)_self;
    self->key = NULL;
    self->value = NULL;
    return _self;
}

ral_kvp_t *ral_kvp_new(const char *key, ral_object_t *value)
{
    ral_kvp_t *self = ral_new(ralKVP);
    TEST(self);
    TEST(self->key = malloc(strlen(key) + 1));
    strcpy(self->key, key);
    self->value = value;
    return self;
fail:
    return NULL;
}

void KVP_delete(ral_object_t *_self)
{
    ral_kvp_t * self = (ral_kvp_t *)_self;
    free(self->key);
    if (!self->wrap) {
        ral_delete(self->value);
    }
}

const char *KVP_name()
{
    return "KVP";
}

void KVP_to_string(ral_object_t *_self)
{
    ral_kvp_t * self = (ral_kvp_t *)_self;
    if (self->string) free(self->string);
    /*fprintf(stderr, "kvp to string: %s => %s\n", ral_cname(self->key), ral_cname(self->value));*/
    const char *value = ral_as_string(self->value);
    TEST(self->string = malloc(strlen(self->key)+strlen(value)+3));
    sprintf(self->string, "%s=>%s", self->key, value);
fail:
    return;
}

static const struct Class _KVP = {
    sizeof(struct ral_kvp),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ KVP_new,
    /*  2 */ NULL,
    /*  3 */ KVP_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ KVP_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ KVP_to_string,
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

ral_class KVP = &_KVP;
ral_class ralKVP = &_KVP;
