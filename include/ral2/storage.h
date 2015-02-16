#ifndef RAL_STORAGE_H
#define RAL_STORAGE_H

/**\file ral2/storage.h
   \brief Storage class.

   Storage is an object, which stores other objects and makes them
   available through a unique key.
*/

int             ral_insert(ral_object_t *storage, 
                           ral_object_t *value, 
                           ral_object_t *key);

int             ral_remove(ral_object_t *storage, 
                           ral_object_t *key);

ral_array_t    *ral_keys(ral_object_t *storage);

int             ral_number_of_elements(ral_object_t *storage);

ral_object_t   *ral_lookup(ral_object_t *storage, 
                           ral_object_t *key);

ral_interval_t *ral_get_value_range(ral_object_t *obj, 
                                    const char *attr);

#endif
