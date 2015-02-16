#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Array_new(ral_object_t *_self)
{
    ral_array_t *self = _self;
    self->largest_index = -1;
    self->size = 256;
    self->array = calloc(self->size, sizeof(ral_object_t *));
    if (!self->array)
        return NULL;
    else
        return self;
}

void Array_delete(ral_object_t *_self)
{
    ral_array_t * self = _self;
    int i;
    if (!self->wrap)
        for (i = 0; i < self->size; i++)
            ral_delete(self->array[i]);
    free(self->array);
}

const char *Array_name()
{
    return "Array";
}

unsigned Array_size(ral_object_t *_self)
{
    ral_array_t *self = _self;
    return self->largest_index + 1;
}

void Array_to_string(ral_object_t *_self)
{
    ral_array_t * self = _self;
    unsigned i, len = 3;
    if (self->string) free(self->string);
    TEST(self->string = malloc(len));
    strcpy(self->string, "(");
    for (i = 0; i <= self->largest_index; i++) {
        const char *x = ral_as_string(self->array[i]);
        if (!x) x = "NUL";
        len += strlen(x)+1;
        TEST(self->string = realloc(self->string, len));
        strcat(self->string, x);
        if (i != self->largest_index)
            strcat(self->string, " ");
    }
    strcat(self->string, ")");
fail:;
}

int Array_insert(ral_object_t *_self, ral_object_t *object, ral_object_t *key)
{
    ral_array_t * self = _self;
    int index = ral_as_int(key);
    /*fprintf(stderr, "array insert %s at %s\n", ral_as_string(object), ral_as_string(key));*/
    if (index < 0) return 0;
    if (index >= self->size) {
        /*fprintf(stderr, "array insert, extend %p from %i to %i\n", self->array, self->size, index+256);*/
        self->size = index+256;
        TEST(self->array = realloc(self->array, self->size));
    }
    if (index > self->largest_index) self->largest_index = index;
    self->array[index] = object;
    /*fprintf(stderr, "array insert %s at %i of %i\n", ral_as_string(object), index, self->largest_index);*/
    return 1;
fail:
    return 0;
}

ral_object_t *Array_lookup(ral_object_t *_self, ral_object_t *key)
{
    ral_array_t * self = _self;
    int index = ral_as_int(key);
    /*fprintf(stderr, "lookup from array at %i\n", index);*/
    if (index < 0 || index > self->largest_index) return ral_undef;
    return self->array[index] ? self->array[index] : ral_undef;
}

int Array_take_out(ral_object_t *_self, ral_object_t *key)
{
    ral_array_t * self = _self;
    int index = ral_as_int(key);
    if (index < 0 || index > self->largest_index) return 1;
    if (self->array[index]) {
        ral_delete(self->array[index]);
        self->array[index] = NULL;
        if (index == self->largest_index)
            self->largest_index--;
        return 1;
    } else
        return 0;
}

int Array_set_object(ral_object_t *_self, ral_object_t *obj, ral_object_t *val)
{
    ral_array_t *self = _self;
    if (class_of(val) == ralInteger) {
        int index = ((ral_integer_t *)val)->value;
        if (index >= 0 && index <= self->largest_index)
            return ral_set(obj, self->array[index]);
    }
    return 0;
}

ral_interval_t *Array_get_value_range(ral_object_t *_self, const char *attr)
{
    ral_array_t * self = _self;
    ral_interval_t *ab = NULL;
    int i;
    for (i = 0; i <= self->largest_index; i++) {
        if (!self->array[i]) break;
        if (ab) {
            if (ral_cmp(self->array[i], ab->min) < 0)
                ral_set(ab->min, self->array[i]);
            else if (ral_cmp(self->array[i], ab->max) > 0)
                ral_set(ab->max, self->array[i]);
        } else
            ab = ral_interval_new(ral_clone(self->array[i]), ral_clone(self->array[i]), 3);
    }
    return ab;
}

ral_class ral_array_get_output_type(ral_object_t *_self)
{
    ral_array_t * self = _self;
    int i;
    for (i = 0; i <= self->largest_index; i++) {
        if (!self->array[i]) 
            break;
        else if (self->array[i] == ral_undef) 
            continue;
        else if (class_of(self->array[i]) == ralArray)
            return ral_array_get_output_type(self->array[i]);
        else
            return class_of(self->array[i]);
    }
    return NULL;
}

static const struct Class _Array = {
    sizeof(struct ral_array),
    {&_Classifier, &_Storage, NULL, NULL},
    /*  1 */ Array_new,
    /*  2 */ NULL,
    /*  3 */ Array_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Array_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ Array_to_string,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ Array_insert,
    /* 13 */ Array_size,
    /* 14 */ NULL,
    /* 15 */ Array_lookup,
    /* 16 */ Array_take_out,
    /* 17 */ NULL,
    /* 18 */ Array_set_object,
    /* 19 */ NULL,
    /* 20 */ Array_get_value_range,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ ral_array_get_output_type
};

ral_class ralArray = &_Array;

void ral_array_sort(ral_array_t * a)
{
    int len = (size_t)a->array[1]-(size_t)a->array[0];
    qsort(a->array[0], Array_size((ral_object_t *)a), len, (__compar_fn_t)ral_cmp);
}

void array_iterate_sum(ral_object_t *item, ral_object_t *sum, ral_object_t *b, ral_object_t *c)
{
    if (!item || item == ral_undef) return;
    if (class_of(sum) == ralInteger && class_of(item) == ralInteger)
        ((ral_integer_t *)(sum))->value += ((ral_integer_t *)(item))->value;
    double x;
    if (class_of(item) == ralInteger)
        x = ((ral_integer_t *)(item))->value;
    else if (class_of(item) == ralReal)
        x = ((ral_real_t *)(item))->value;
    else 
        return;
    if (class_of(sum) == ralReal)
        ((ral_real_t *)(sum))->value += x;
}

void array_iterate_mean(ral_object_t *item, ral_object_t *mean, ral_object_t *index, ral_object_t *c)
{
    if (!item || item == ral_undef) return;
    double i = ++((ral_integer_t *)(index))->value;
    double x;
    if (class_of(item) == ralInteger)
        x = ((ral_integer_t *)(item))->value;
    else if (class_of(item) == ralReal)
        x = ((ral_real_t *)(item))->value;
    else 
        return;
    ((ral_real_t *)(mean))->value = ((i-1)/i)*((ral_real_t *)(mean))->value + x/i;
}

void array_iterate_variance(ral_object_t *item, ral_object_t *variance, ral_object_t *mean, ral_object_t *index)
{
    if (!item || item == ral_undef) return;
    double i = ++((ral_integer_t *)(index))->value;
    double x;
    if (class_of(item) == ralInteger)
        x = ((ral_integer_t *)(item))->value;
    else if (class_of(item) == ralReal)
        x = ((ral_real_t *)(item))->value;
    else 
        return;
    double m = ((ral_real_t *)(mean))->value;
    ((ral_real_t *)(mean))->value = m = ((i-1)/i)*m + x/i;
    double v = ((ral_real_t *)(variance))->value;
    ((ral_real_t *)(variance))->value = v = i == 1 ? 0 : ((i-1)/i)*v + (1/(i-1))*pow(x-m, 2.0);
    /*fprintf(stderr, "variance: x=%f, m=%f, v=%f, i=%i\n", x, m, v, ((ral_integer_t *)(index))->value);*/
}

void array_iterate_count(ral_object_t *item, ral_object_t *count, ral_object_t *b, ral_object_t *c)
{
    if (item && item != ral_undef) ((ral_integer_t *)(count))->value++;
}

void array_iterate_count_of(ral_object_t *item, ral_object_t *count, ral_object_t *value, ral_object_t *c)
{
    if (item && 
        item != ral_undef && 
        class_of(item) == ralInteger && 
        ((ral_integer_t *)(item))->value == ((ral_integer_t *)(value))->value)
        ((ral_integer_t *)(count))->value++;
}

void array_iterate_range(ral_object_t *item, ral_object_t *range, ral_object_t *b, ral_object_t *c)
{
    ral_interval_t * r = range;
    if (!item || item == ral_undef) return;
    if (class_of(item) == ralInteger) {
        long x = ((ral_integer_t *)(item))->value;
        if (r->min == ral_undef)
            r->min = (ral_object_t *)ral_integer_new(x);
        else {
            FAIL_UNLESS(class_of(r->min) == ralInteger);
            ((ral_integer_t *)(r->min))->value = MIN(x, ((ral_integer_t *)(r->min))->value);
        }
        if (r->max == ral_undef)
            r->max = ral_integer_new(x);
        else {
            FAIL_UNLESS(class_of(r->max) == ralInteger);
            ((ral_integer_t *)(r->max))->value = MAX(x, ((ral_integer_t *)(r->max))->value);
        }
    } else if (class_of(item) == ralReal) {
        double x = ((ral_real_t *)(item))->value;
        if (r->min == ral_undef)
            r->min = ral_real_new(x);
        else {
            FAIL_UNLESS(class_of(r->min) == ralReal);
            ((ral_real_t *)(r->min))->value = MIN(x, ((ral_real_t *)(r->min))->value);
        }
        if (r->max == ral_undef)
            r->max = ral_real_new(x);
        else {
            FAIL_UNLESS(class_of(r->max) == ralReal);
            ((ral_real_t *)(r->max))->value = MAX(x, ((ral_real_t *)(r->max))->value);
        }
    }
fail:;
}

typedef void (iterate_fct)(ral_object_t *item, ral_object_t *sum, ral_object_t *b, ral_object_t *c);

void iterate(ral_array_t * self, ral_object_t *a, ral_object_t *b, ral_object_t *c, iterate_fct fct)
{
    int i;
    for (i = 0; i <= self->largest_index; i++) {
        if (!self->array[i]) 
            continue;
        else if (self->array[i] == ral_undef) 
            continue;
        else if (class_of(self->array[i]) == ralArray) {
            iterate(self->array[i], a, b, c, fct);
        } else
            fct(self->array[i], a, b, c);
    }
}

ral_object_t *ral_array_sum(ral_array_t * self)
{
    ral_object_t *s;
    if (ral_array_get_output_type(self) == ralInteger)
        s = ral_integer_new(0);
    else if (ral_array_get_output_type(self) == ralReal)
        s = ral_real_new(0.0);
    else
        return ral_undef;
    iterate(self, s, NULL, NULL, array_iterate_sum);
    return s;
}

ral_object_t *ral_array_mean(ral_array_t * self)
{
    ral_real_t * m = ral_real_new(0.0);
    ral_integer_t * i = ral_integer_new(0);
    iterate(self, (ral_object_t *)m, (ral_object_t *)i, NULL, array_iterate_mean);
    return (ral_object_t *)m;
}

ral_object_t *ral_array_variance(ral_array_t * self)
{
    ral_real_t * v = ral_real_new(0.0);
    ral_real_t * m = ral_real_new(0.0);
    ral_integer_t * i = ral_integer_new(0);
    iterate(self, (ral_object_t *)v, (ral_object_t *)m, (ral_object_t *)i, array_iterate_variance);
    return (ral_object_t *)v;
}

ral_object_t *ral_array_count(ral_array_t * self)
{
    ral_integer_t * count = ral_integer_new(0);
    iterate(self, (ral_object_t *)count, NULL, NULL, array_iterate_count);
    return (ral_object_t *)count;
}

ral_object_t *ral_array_count_of(ral_array_t * self, ral_object_t *value)
{
    ral_integer_t * count = ral_integer_new(0);
    iterate(self, (ral_object_t *)count, value, NULL, array_iterate_count_of);
    return (ral_object_t *)count;
}

ral_interval_t * ral_array_range(ral_array_t * self)
{
    ral_interval_t * r = ral_interval_new(ral_undef, ral_undef, 3);
    iterate(self, (ral_object_t *)r, NULL, NULL, array_iterate_range);
    return r;
}

void ral_array_insert(ral_array_t *_self, ral_object_t *obj, int index)
{
    ral_array_t * self = _self;
    if (index < 0) return;
    if (index >= self->size) {
        self->size = index+256;
        TEST(self->array = realloc(self->array, self->size));
    }
    self->array[index] = obj;
    if (index > self->largest_index) self->largest_index = index;
fail:;
}

void ral_array_push(ral_array_t *self, ral_object_t *obj)
{
    if (self->largest_index+1 >= self->size) {
        self->size = self->largest_index+256;
        TEST(self->array = realloc(self->array, self->size));
    }
    self->largest_index++;
    self->array[self->largest_index] = obj;
fail:;
}
