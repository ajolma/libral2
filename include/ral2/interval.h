#ifndef RAL_INTERVAL_H
#define RAL_INTERVAL_H

/**\file ral2/interval.h
   \brief Interval class

   An interval object is used for an interval in a dimension.
*/

extern ral_class ralInterval;
typedef struct ral_interval ral_interval_t;

ral_interval_t * ral_interval_new(ral_object_t *min, 
                                  ral_object_t *max, 
                                  int end_point_inclusion);
ral_object_t *ral_interval_min(ral_interval_t * self);
ral_object_t *ral_interval_max(ral_interval_t * self);
int ral_interval_end_point_inclusion(ral_interval_t * self);

#endif
