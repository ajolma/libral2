#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Bin_new(ral_object_t *_self)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    self->size = 64;
    self->bins = NULL;
    self->values = NULL;
    TEST(self->bins = calloc(self->size, sizeof(void*)));
    TEST(self->values = calloc(self->size, sizeof(void*)));
    return _self;
fail:
    if (self->bins) free(self->bins);
    if (self->values) free(self->values);
    return NULL;
}

void Bin_delete(ral_object_t *_self)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    int i;
    if (!self->wrap) {
        for (i = 0; i < self->size; i++) {
            ral_delete(self->bins[i]);
            ral_delete(self->values[i]);
        }
        free(self->bins);
        free(self->values);
    }
}

const char *Bin_name()
{
    return "Bin";
}

void Bin_to_string(ral_object_t *_self)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    unsigned i, len = 3;
    if (self->string) free(self->string);
    TEST(self->string = malloc(len));
    strcpy(self->string, "(");
    const char *x = NULL;
    for (i = 0; i < self->size; i++) {
        const char *y = ral_as_string(self->values[i]);
        if (!self->bins[i]) {
            len += strlen(x)+strlen(y)+6;
            TEST(self->string = realloc(self->string, len));
            strcat(self->string, "[>");
            strcat(self->string, x);
            strcat(self->string, ",");
            strcat(self->string, y);
            strcat(self->string, "]");
            break;
        }
        x = ral_as_string(self->bins[i]);
        len += strlen(x)+strlen(y)+6;
        TEST(self->string = realloc(self->string, len));
        strcat(self->string, "[<=");
        strcat(self->string, x);
        strcat(self->string, ",");
        strcat(self->string, y);
        strcat(self->string, "] ");
    }
    strcat(self->string, ")");
fail:
    return;
}

int Bin_find_bin(ral_bin_t * self, ral_object_t *key)
{
    int i = 0;
    while (1) {
        if (!self->bins[i] || ral_cmp(self->bins[i], key) >= 0)
            return i;
        i++;
    }
    return -1; /* will never happen */
}

int Bin_insert(ral_object_t *_self, ral_object_t *value, ral_object_t *bin)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    int i;
    if (!value) { /* add a new bin */
        for (i = 0; i < self->size; i++) {
            ral_delete(self->values[i]);
            self->values[i] = NULL;
        }
        int n = self->size;
        int k = Bin_find_bin(self, bin); /* 0..n-1 */
        if (k == n-1) { /* the bin has to be expanded */
            self->size += 32;
            TEST(self->bins = realloc(self->bins, self->size));
            TEST(self->values = realloc(self->values, self->size));
        }
        for (i = n-1; i > k; i--)
            self->bins[i] = self->bins[i-1];
        self->bins[k] = ral_clone(bin);
    } else { /* set value to the interval defined by the bin */
        int k = Bin_find_bin(self, bin);
        /*fprintf(stderr, "insert %s into bin at %i\n", as_string(value), k);*/
        self->values[k] = value;
    }
    return 1;
fail:
    return 0;
}

unsigned Bin_size(ral_object_t *_self)
{
    ral_bin_t * self = _self;
    int i, k = -1;
    for (i = 0; i < self->size; i++)
        if (self->bins[i])
            k = i;
        else
            break;
    return k+2;
}

ral_object_t *Bin_lookup(ral_object_t *_self, ral_object_t *key)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    int i = Bin_find_bin(self, key);
    /*fprintf(stderr, "lookup %s = %i = %s\n", as_string(key), i, as_string(self->values[i]));*/
    return self->values[i];
}

int Bin_take_out(ral_object_t *_self, ral_object_t *key)
{
    /*ral_bin_t * self = (ral_bin_t *)_self;*/
    return 0;
}

ral_array_t * Bin_keys(ral_object_t *_self)
{
    /*ral_bin_t * self = (ral_bin_t *)_self;*/
    return NULL;
}

int Bin_set_object(ral_object_t *_self, ral_object_t *obj, ral_object_t *val)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    int k = Bin_find_bin(self, val);
    if (self->values[k])
        return ral_set(obj, self->values[k]);
    return 0;
}

ral_class Bin_get_output_type(ral_object_t *_self)
{
    ral_bin_t * self = (ral_bin_t *)_self;
    ral_class ret = NULL;
    int i;
    for (i = 0; i < self->size; i++) {
        if (self->values[i] && self->values[i] != ral_undef) {
            ral_class c = class_of(self->values[i]);
            if (ret == NULL || ret == ralString || (ret == ralInteger && c == ralReal))
                ret = c;
        }        
        if (!self->bins[i]) break;
    }
    return ret;
}

static const struct Class _Bin = {
    sizeof(struct ral_bin),
    {&_Classifier, &_Storage, NULL, NULL},
    /*  1 */ Bin_new,
    /*  2 */ NULL,
    /*  3 */ Bin_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Bin_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ Bin_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ Bin_insert,
    /* 13 */ Bin_size,
    /* 14 */ NULL,
    /* 15 */ Bin_lookup,
    /* 16 */ Bin_take_out,
    /* 17 */ Bin_keys,
    /* 18 */ Bin_set_object,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ Bin_get_output_type
};

ral_class ralBin = &_Bin;

void ral_bin_set(ral_bin_t * self, ral_array_t * bins, ral_array_t * values)
{
    int i;
    if (!self->wrap)
        for (i = 0; i < self->size; i++) {
            ral_delete(self->bins[i]);
            ral_delete(self->values[i]);
        }
    int n = ral_number_of_elements(bins);
    if (n > self->size) {
        self->size = n;
        TEST(self->bins = realloc(self->bins, self->size));
        TEST(self->values = realloc(self->values, self->size));
    }
    for (i = 0; i < self->size; i++) {
        self->bins[i] = NULL;
        self->values[i] = NULL;
    }
    {
        ral_integer_t * i = ral_integer_new(0);
        for (i->value = 0; i->value < n; i->value++) {
            self->bins[i->value] = ral_clone(ral_lookup(bins, i));
            self->values[i->value] = ral_clone(ral_lookup(values, i));
        }
        ral_delete(i);
    }
    self->values[n] = ral_clone(values->array[n]);
fail:;
}

void ral_bin_set_bins(ral_bin_t * self, ral_array_t * bins)
{
    int i;
    if (!self->wrap)
        for (i = 0; i < self->size; i++) {
            ral_delete(self->bins[i]);
            ral_delete(self->values[i]);
        }
    int n = ral_number_of_elements(bins);
    if (n > self->size) {
        self->size = n;
        TEST(self->bins = realloc(self->bins, self->size));
        TEST(self->values = realloc(self->values, self->size));
    }
    for (i = 0; i < self->size; i++) {
        self->bins[i] = NULL;
        self->values[i] = NULL;
    }
    {
        ral_integer_t * i = ral_integer_new(0);
        for (i->value = 0; i->value < n; i->value++) {
            self->bins[i->value] = ral_clone(ral_lookup(bins, i));
        }
        ral_delete(i);
    }
fail:;
}

ral_object_t *ral_bin_bin(ral_bin_t * self, int i)
{
    if (i >= 0 && i < self->size-1)
        return self->bins[i];
    else
        return NULL;
}

ral_object_t *ral_bin_value(ral_bin_t * self, int i)
{
    if (i >= 0 && i < self->size)
        return self->values[i];
    else
        return NULL;
}
