#ifndef RAL_INTEGER_H
#define RAL_INTEGER_H

/**\file ral2/integer.h
   \brief Integer class

   An integer object stores an integer.
*/

extern ral_class ralInteger;
typedef struct ral_integer ral_integer_t;

ral_integer_t *ral_integer_new(long value);
ral_integer_t *ral_integer_set(ral_integer_t *self, long value);

#endif
