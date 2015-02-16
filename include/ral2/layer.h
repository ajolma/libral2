#ifndef RAL_LAYER_H
#define RAL_LAYER_H

/**\file ral2/layer.h
   \brief Layer class.

   A layer object is a <a href="storage_8h.html">storage</a> of
   similar geospatial <a href="feature_8h.html">features</a>. Some
   methods of feature class can also used with layers.
*/

extern ral_class ralLayer;

typedef struct ral_layer ral_layer_t;

/**\brief Render a layer on a device.

   The style should be a triplets (key => key-value-pair) hash. The
   main key should be a visual attribute (one from those obtained with
   function ral_visual_attributes()), the key in the key-value pair
   should be a name of an attribute (one obtained with method
   ral_get_attributes()), the value should be a classifier for
   converting the attribute value into a value of a visual attribute
   (color value, line width, font name, ...).
*/
void            ral_render(ral_object_t *layer, 
                           ral_output_device_t *device, 
                           ral_hash_t *style,
                           ral_error_t **e);

#endif
