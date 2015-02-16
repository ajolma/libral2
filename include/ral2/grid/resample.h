#ifndef RAL_GRID_RESAMPLE_H
#define RAL_GRID_RESAMPLE_H

/**\file ral2/grid/resample.h
   \brief Resampling methods for grids

*/

/** 
    - the returned grid has no data outside of gd, g1, or g2 
    - in join data from g1 takes precedence over data from g2
*/
ral_grid_t RAL_CALL *ral_grid_clip(ral_grid_t *gd, ral_rcoords_t * ul, ral_rcoords_t * dr, ral_error_t **e);
ral_grid_t RAL_CALL *ral_grid_join(ral_grid_t *g1, ral_grid_t *g2, ral_error_t **e);

/** 
    - pick values from src to dest, the picking method is simple look up
    - based on the center point of a cell
    - fails if src is real and dest is int and src contains too large numbers
*/
void RAL_CALL ral_grid_pick(ral_grid_t *dest, ral_grid_t *src);

#endif
