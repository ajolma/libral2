#ifndef RAL_GRID_NEIGHBORHOOD_H
#define RAL_GRID_NEIGHBORHOOD_H

/**\file ral2/grid/neighborhood.h
   \brief Neighborhood methods for grids

*/

/** Returns a hash of hashes whose keys are the values in the grid */
ral_hash_t * RAL_CALL ral_grid_get_neighbors(ral_grid_t *gd);

/** 
    - value(cell_t+1) = sum(i=0..8; k[i]*value[neighbor]) neighbor 0 is the cell itself 
    - k _has to_ point to a 9 element table of RAL_REAL's (for real grids) or RAL_INTEGER (for integer grids)
*/
ral_grid_t RAL_CALL *ral_grid_ca_step(ral_grid_t *gd, void *k, ral_error_t **e);

/** a buffer zone around cells with value z. returns a binary grid */
ral_grid_t RAL_CALL *ral_grid_bufferzone(ral_grid_t *gd, int z, double w, ral_error_t **e);

double RAL_CALL ral_grid_zonesize(ral_grid_t *gd, ral_rcoords_t * c, ral_error_t **e);

ral_grid_t RAL_CALL *ral_grid_areas(ral_grid_t *gd, int k, ral_error_t **e);

int RAL_CALL ral_grid_connect(ral_grid_t *gd);

/** mark connected areas with unique numbers */
int RAL_CALL ral_grid_number_areas(ral_grid_t *gd, int connectivity, ral_error_t **e);

/** the returned grid has in the 'no data' cells of gd
    the distance to the nearest data cell in gd
*/
ral_grid_t RAL_CALL *ral_grid_distances(ral_grid_t *gd, ral_error_t **e);

/** the returned grid has in the 'no data' cells of gd
    the direction to the nearest data cell in gd
*/
ral_grid_t RAL_CALL *ral_grid_directions(ral_grid_t *gd, ral_error_t **e);

/** returns the nearest cell to cell c containing data
 */
ral_rcoords_t * RAL_CALL ral_grid_nearest_neighbor(ral_grid_t *gd, ral_rcoords_t * c);

/** nearest neighbor interpolation, each 'no data' cell will get the value of
    its nearest neighbor (in distance sense) 
*/
ral_grid_t RAL_CALL *ral_grid_nn(ral_grid_t *gd, ral_error_t **e); 

/** calculates a cost-to-go raster from a cost raster and target location */
ral_grid_t RAL_CALL *ral_grid_dijkstra(ral_grid_t *w, ral_rcoords_t * c, ral_error_t **e);

#endif
