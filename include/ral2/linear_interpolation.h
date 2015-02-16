#ifndef RAL_LINEAR_INTERPOLATION_H
#define RAL_LINEAR_INTERPOLATION_H

/**\file ral2/linear_interpolation.h
   \brief Linear interpolation class.

   A linear interpolation is an object used for interpolating in a
   dimension. A linear interpolation is a <a
   href="classifier_8h.html">classifier</a>, as it maps ordinal values
   into other ordinal values.
*/

extern ral_class ralLinearInterpolation;
typedef struct ral_linear_interpolation ral_linear_interpolation_t;

ral_linear_interpolation_t * ral_linear_interpolation_new(ral_data_point_t * a, 
                                                          ral_data_point_t * b);
void ral_linear_interpolation_set_data_points(ral_linear_interpolation_t * self, 
                                              ral_data_point_t * p0, 
                                              ral_data_point_t * p1);
ral_object_t *ral_interpolate(ral_linear_interpolation_t * self, ral_object_t *x);

#endif
