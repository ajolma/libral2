#ifndef RAL_VECTOR_H
#define RAL_VECTOR_H

/**\file ral2/vector.h
   \brief Vector class

   A vector is a vector <a href="layer_8h.html">layer</a>.
*/

extern ral_class ralVector;

typedef struct ral_vector ral_vector_t;

ral_vector_t * ral_vector_new_from_OGR(OGRLayerH ogr_layer);

#endif
