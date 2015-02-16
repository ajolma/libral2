#ifndef RAL_HASH_H
#define RAL_HASH_H

/**\file ral2/hash.h
   \brief Hash class

   A hash is a <a href="storage_8h.html">storage</a>, which stores
   objects indexed with a string. A hash is also a <a
   href="classifier_8h.html">classifier</a>, as it maps strings into
   arbitrary objects.
*/

extern ral_class ralHash;
typedef struct ral_hash ral_hash_t;

ral_object_t *ral_hash_lookup(ral_hash_t *self, const char *key);

#endif
