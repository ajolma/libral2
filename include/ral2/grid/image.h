#ifndef RAL_GRID_IMAGE_H
#define RAL_GRID_IMAGE_H

/**\file ral2/grid/image.h
   \brief Image analysis type methods for grids

*/

/** spread the values at each cell according to the kernel */
ral_grid_t RAL_CALL *ral_grid_spread(ral_grid_t *grid, ral_array_t * kernel);

/** spread the individuals at each cell to their neighborhood; defined only for integer grids */
ral_grid_t RAL_CALL *ral_grid_spread_random(ral_grid_t *grid, ral_array_t * focus, ral_error_t **e);

/** a scanline method, suitable in general for a single area */
ral_grid_t RAL_CALL *ral_grid_borders(ral_grid_t *gd, ral_error_t **e);  

/** a recursive algorithm, suitable for multiple areas */
ral_grid_t RAL_CALL *ral_grid_borders_recursive(ral_grid_t *gd, ral_error_t **e); 


#endif
