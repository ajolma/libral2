#ifndef RAL_DATA_POINT_H
#define RAL_DATA_POINT_H

/**\file ral2/data_point.h
   \brief Data point class.

   A data point is an object used for two dimensional data points.
*/

extern ral_class ralDataPoint;
typedef struct ral_data_point ral_data_point_t;

ral_data_point_t * ral_data_point_new(ral_object_t *x, ral_object_t *y);

#endif
