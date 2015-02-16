#ifndef RAL_ORDINAL_H
#define RAL_ORDINAL_H

/**\file ral2/ordinal.h
   \brief An abstract base class for ordinal classes

   An ordinal object is one that can be compared to another object of the same class.
*/

int             ral_cmp(ral_object_t *object, ral_object_t *other);

#endif
