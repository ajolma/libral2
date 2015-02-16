#ifndef RAL_KVP_H
#define RAL_KVP_H

/**\file ral2/kvp.h
   \brief Key-value pair class

   A kvp object is a key - value pair. The key is a string and the value is an object.
*/

extern ral_class ralKVP;
typedef struct ral_kvp ral_kvp_t;

ral_kvp_t *ral_kvp_new(const char *key, ral_object_t *value);

#endif
