#ifndef RAL_ARRAY_H
#define RAL_ARRAY_H

/**\file ral2/array.h
   \brief Array class.

   An array is a <a href="storage_8h.html">storage</a>, where the key
   is a non-negative integer. An array is also a <a
   href="classifier_8h.html">classifier</a>, as it maps integers into
   arbitrary objects.
*/

extern ral_class ralArray;

typedef struct ral_array ral_array_t;

void ral_array_insert(ral_array_t *self, ral_object_t *obj, int index);
void ral_array_push(ral_array_t *self, ral_object_t *obj);

void ral_array_sort(ral_array_t * self);

/* the following work for multidimensional arrays as long as they are
 * strictly integer and real arrays */
ral_object_t *ral_array_sum(ral_array_t * self);
ral_object_t *ral_array_mean(ral_array_t * self);
ral_object_t *ral_array_variance(ral_array_t * self);
ral_object_t *ral_array_count(ral_array_t * self);
ral_object_t *ral_array_count_of(ral_array_t * self, ral_object_t *value);
ral_interval_t * ral_array_range(ral_array_t * self);

#endif
