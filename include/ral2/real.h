#ifndef RAL_REAL_H
#define RAL_REAL_H

/**\file ral2/real.h
   \brief Real class

   A real is an object that stores a real number.
*/

extern ral_class ralReal;
typedef struct ral_real ral_real_t;

ral_real_t *ral_real_new(double value);

#endif
