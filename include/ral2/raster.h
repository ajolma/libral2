#ifndef RAL_RASTER_H
#define RAL_RASTER_H

/**\file ral2/raster.h
   \brief Raster class

   A raster is a raster <a href="layer_8h.html">layer</a>.
*/

extern ral_class ralRaster;
typedef struct ral_raster ral_raster_t;

ral_raster_t * ral_raster_new_from_grid(ral_grid_t *gd);

#endif
