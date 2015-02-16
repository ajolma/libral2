#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Hash_new(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    self->size_of_table = 64;
    TEST(self->table = calloc(self->size_of_table, sizeof(ral_kvp_t *)));
    return _self;
fail:
    return NULL;
}

void Hash_delete(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    free(self->table);
}

const char *Hash_name()
{
    return "Hash";
}

void Hash_to_string(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    int len = 3;
    if (self->string) free(self->string);
    TEST(self->string = malloc(len));
    strcpy(self->string, "(");
    ral_array_t * keys = ral_keys(self);
    ral_integer_t * i = ral_new(ralInteger);
    int n = ral_number_of_elements(keys);
    for (i->value = 0; i->value < n; i->value++) {
        ral_object_t *key = ral_lookup(keys, i);
        ral_object_t *value = ral_lookup(self, key);
        const char *x = ral_as_string(key);
        const char *y = ral_as_string(value);
        len += strlen(x)+strlen(y)+8;
        TEST(self->string = realloc(self->string, len));
        strcat(self->string, x);
        strcat(self->string, " => ");
        strcat(self->string, y);
        if (i->value != n-1)
            strcat(self->string, ", ");
    }
    strcat(self->string, ")");
fail:;
}

unsigned hash_fct(const char *key)
{
    unsigned hash = 5381;
    int c;
    const char *str = key;
    assert(key);
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

int Hash_insert(ral_object_t *_self, ral_object_t *object, ral_object_t *key)
{
    ral_hash_t *self = _self;
    if (!key || key == ral_undef) return 0;
    const char *k = ral_as_string(key);
    /*fprintf(stderr, "insert %s(%p %s) => %s(%s) into hash %p\n", k, key, ral_cname(key), as_string(object), ral_cname(object), self);*/
    unsigned val = hash_fct(k) % self->size_of_table;
    ral_kvp_t *kvp = ral_kvp_new(k, object);
    int new = 1;
    ral_kvp_t *i, *j; /* cursors */
    if (self->table[val]) {
        for (j = NULL, i = self->table[val]; i; j = i, i = i->next)
            if (strcmp(k, i->key) == 0) {
                if (j) j->next = i->next;
                ral_delete(i);
                i = kvp;
                if (j) {
                    i->next = j->next;
                    j->next = i;
                }
                new = 0;
            }
        if (new) j = (self->table)[val];
    } else
        j = NULL;
    if (new) {
        kvp->next = j;
        (self->table)[val] = kvp;
    }
    return 1;
}

unsigned Hash_size(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    unsigned i;
    unsigned count = 0;
    for (i = 0; i < self->size_of_table; i++) {
        ral_kvp_t * cursor = self->table[i];
        while (cursor) {
            count++;
            cursor = cursor->next;
        }
    }
    return count;
}

ral_object_t *Hash_lookup(ral_object_t *_self, ral_object_t *key)
{
    if (!key || key == ral_undef) return NULL;
    return ral_hash_lookup((ral_hash_t *)_self, ral_as_string(key));
}

ral_object_t *ral_hash_lookup(ral_hash_t * self, const char *key)
{
    /*fprintf(stderr, "lookup %s from hash %p\n",  key, self);*/
    unsigned val = hash_fct(key) % self->size_of_table;
    ral_kvp_t * i;
    ral_object_t *value = NULL;
    if ((self->table)[val])
        for (i = (self->table)[val]; i; i = i->next) {
            /*fprintf(stderr, "cmp %s to %s\n", key, i->key);*/
            if (strcmp(key, i->key) == 0) {
                value = i->value;
                break;
            }
        }
    return value;
}

int Hash_take_out(ral_object_t *_self, ral_object_t *key)
{
    ral_hash_t *self = _self;
    const char *k = ral_as_string(key);
    unsigned val = hash_fct(k) % self->size_of_table;
    ral_kvp_t *i, *j;
    int success = 0;
    if ((self->table)[val])
        for (j = NULL, i = (self->table)[val]; i; j = i, i = i->next) {
            if (strcmp(k, i->key) == 0) {
                if (j)
                    j->next = i->next;
                else
                    self->table[val] = i->next;
                ral_delete(i);
                success = 1;
                break;
            }
        }
    return success;
}

ral_array_t * Hash_keys(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    ral_object_t *keys = ral_new(ralArray);
    if (keys) {
        int i;
        ral_integer_t * j = ral_new(ralInteger);
        j->value = 0;
        for (i = 0; i < self->size_of_table; i++) {
            ral_kvp_t * cursor = self->table[i];
            while (cursor) {
                ral_string_t * s = ral_new(ralString);
                ral_string_set(s, cursor->key);
                ral_insert(keys, s, j);
                j->value++;
                cursor = cursor->next;
            }
        }
    }
    return keys;
}

int Hash_set_object(ral_object_t *self, ral_object_t *object, ral_object_t *value)
{
    ral_object_t *obj = ral_lookup(self, value);
    if (obj)
        return ral_set(object, obj);
    return 0;
}

ral_class ral_hash_get_output_type(ral_object_t *_self)
{
    ral_hash_t *self = _self;
    ral_class ret = NULL;
    int i;
    for (i = 0; i < self->size_of_table; i++) {
        ral_kvp_t * cursor;
        for (cursor = (self->table)[i]; cursor; cursor = cursor->next) {
            if (cursor != ral_undef) {
                ral_class c = class_of(cursor);
                if (ret == NULL || ret == ralString || (ret == ralInteger && c == ralReal))
                    ret = c;
            }        
        }
    }
    return ret;
}

static const struct Class _Hash = {
    sizeof(struct ral_hash),
    {&_Classifier, &_Storage, NULL, NULL},
    /*  1 */ Hash_new,
    /*  2 */ NULL,
    /*  3 */ Hash_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Hash_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ Hash_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ Hash_insert,
    /* 13 */ Hash_size,
    /* 14 */ NULL,
    /* 15 */ Hash_lookup,
    /* 16 */ Hash_take_out,
    /* 17 */ Hash_keys,
    /* 18 */ Hash_set_object,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ ral_hash_get_output_type
};

ral_class ralHash = &_Hash;
