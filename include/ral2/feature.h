#ifndef RAL_FEATURE_H
#define RAL_FEATURE_H

/**\file ral2/feature.h
   \brief Feature class.

   A feature is an object which has attributes and values associated with those.
*/

extern ral_class ralFeature;

typedef struct ral_feature ral_feature_t;

ral_array_t    *ral_get_attributes(ral_object_t *feature);

ral_class       ral_get_attr_type(ral_object_t *feature, 
                                  const char *attr);

void            ral_feature_get_value(ral_object_t *feature,
                                      ral_object_t *value, 
                                      const char *attr);

#endif
