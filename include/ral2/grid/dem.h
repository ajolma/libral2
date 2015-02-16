#ifndef RAL_DEM_H
#define RAL_DEM_H

/**\file ral2/grid/dem.h
   \brief Methods for analysing DEM grids

   fdg is a Flow Direction Grid, an integer grid with flow dirs in cells 
   - -1 denotes flat area, 
   - 0 is a pit (local minimum), 
   - 1 is up (-1,0), 
   - 2 is up-right (-1,1), 
   - 3 is right (0,1), etc
*/

#define RAL_N 1
#define RAL_NE 2
#define RAL_E 3
#define RAL_SE 4
#define RAL_S 5
#define RAL_SW 6
#define RAL_W 7
#define RAL_NW 8

#define RAL_FLAT_AREA -1
#define RAL_PIT_CELL 0

ral_grid_t RAL_CALL *ral_dem_aspect(ral_grid_t *dem);

/** z_factor is the unit of z dived by the unit of x and y */
ral_grid_t RAL_CALL *ral_dem_slope(ral_grid_t *dem, double z_factor);

#define RAL_D8 1
#define RAL_RHO8 2
#define RAL_MANY8 3

/**
   all flowpaths end in a pit, a 'no data' cell, or outside the grid
   methods are 
   - D8: deterministic eight-neighbors
   - RHO8: stochastic eight-neighbors (Fairfield & Leymarie, WRR 27(5) 709-717)
   - MANY8: codes all 8 neighbor cells which are lower
   - default method is D8
*/
ral_grid_t RAL_CALL *ral_dem_fdg(ral_grid_t *dem, int method, ral_error_t **e);

/**
   returns the outlet cell (cell which drains to pit, flat, no data, or outside) 
   of the catchment in which c is 
*/
ral_rcoords_t * RAL_CALL ral_fdg_outlet(ral_grid_t *fdg, ral_grid_t *streams, ral_rcoords_t * c);

/** 
   upslope cells grid, upslope cells are all 8-neighbors which are higher, 
   upslope cells are coded with bits in the cell value
 */
ral_grid_t RAL_CALL *ral_dem_ucg(ral_grid_t *dem, ral_error_t **e);

int RAL_CALL ral_fdg_catchment(ral_grid_t *fdg, ral_grid_t *mark, ral_rcoords_t * c, int m);

/** 
    Drain flat areas by iteratively draining flat cells to non-higher
    lying cells whose drainagege is resolved. Return value < 0 denotes error.
*/
int RAL_CALL ral_fdg_drain_flat_areas1(ral_grid_t *fdg, ral_grid_t *dem);

/** 
    Drain flat areas to the inner pour point. Make inner pour point a
    pit if its pour point is higher else drain it to the outer pour
    point. Return value < 0 denotes error.
*/
int RAL_CALL ral_fdg_drain_flat_areas2(ral_grid_t *fdg, ral_grid_t *dem, ral_error_t **e);

/** Raise single cell pits. */

int RAL_CALL ral_dem_raise_pits(ral_grid_t *dem, double z_limit);

/** Lower single cell peaks. */

int RAL_CALL ral_dem_lower_peaks(ral_grid_t *dem, double z_limit);

/**
   Return the depressions in the FDG, each depression is marked with a
   unique integer if inc_m is true.
*/
ral_grid_t RAL_CALL *ral_fdg_depressions(ral_grid_t *fdg, int inc_m, ral_error_t **e);

/**
   fills the depressions in the dem, returns the number of filled depressions
*/
int RAL_CALL ral_dem_fill_depressions(ral_grid_t *dem, ral_grid_t *fdg, ral_error_t **e);

/** An implementation of the breach algorithm in Martz and Garbrecht (1998). */
int RAL_CALL ral_dem_breach(ral_grid_t *dem, ral_grid_t *fdg, int limit, ral_error_t **e);

/**  
     inverts the path from the pit cell to the lowest pour point of the depression
*/
int RAL_CALL ral_fdg_drain_depressions(ral_grid_t *fdg, ral_grid_t *dem, ral_error_t **e);

/** Route water downstream. Routes to all downstream cells that share
    boundary with a cell (0 to 4 cells) unless FDG is given. Returns
    the changes in cell water storage. Amount of water routed
    downstream is computed as k*Sqrt(slope)*water. r is the unit of
    elevation divided by the unit of x and y in DEM.
 */
ral_grid_t RAL_CALL *ral_water_route(ral_grid_t *water, ral_grid_t *dem, ral_grid_t *fdg, ral_grid_t *k, double r);

/** the path as defined by the fdg (it goes through cell center
    points), the path ends at non-direction cell in fdg, at the border of
    the grid, or (if stop is given) at a cell where stop > 0 */
ral_grid_t RAL_CALL *ral_fdg_path(ral_grid_t *fdg, ral_rcoords_t * c, ral_grid_t *stop, ral_error_t **e);

/** the path as in ral_fdg_path, length is calculated using cell_size,
    if op is given and is 'no data value, that part of the path is not included */
ral_grid_t RAL_CALL *ral_fdg_path_length(ral_grid_t *fdg, ral_grid_t *stop, ral_grid_t *op);

/** as ral_fdg_path_length, computes the weighted sum of op along the path */
ral_grid_t RAL_CALL *ral_fdg_path_sum(ral_grid_t *fdg, ral_grid_t *stop, ral_grid_t *op);

/** focal sum, focal defined as upslope cells and possibly self */
ral_grid_t RAL_CALL *ral_fdg_upslope_sum(ral_grid_t *fdg, ral_grid_t *op, int include_self);

/** focal mean, focal defined as upslope cells and possibly self */
/*ral_grid_t RAL_CALL *ral_fdg_upslope_mean(ral_grid_t *fdg, ral_grid_t *op, int include_self);*/

/** focal variance, focal defined as upslope cells and possibly self */
/*ral_grid_t RAL_CALL *ral_fdg_upslope_variance(ral_grid_t *fdg, ral_grid_t *op, int include_self);*/

/** focal count, focal defined as upslope cells and possibly self, 
    if op == NULL counts all upslope cells are
 */
ral_grid_t RAL_CALL *ral_fdg_upslope_count(ral_grid_t *fdg, ral_grid_t *op, int include_self);

/** focal count_of, focal defined as upslope cells and possibly self */
/*ral_grid_t RAL_CALL *ral_fdg_upslope_count_of(ral_grid_t *fdg, ral_grid_t *op, int include_self, RAL_INTEGER value);*/

/** create a grid of the subcatchments defined by the streams grid, fdg grid, and root cell c
    each subcatchment is marked with unique id */
ral_grid_t RAL_CALL *ral_streams_subcatchments(ral_grid_t *streams, ral_grid_t *fdg, ral_rcoords_t * c, ral_error_t **e);

/** create a grid of all subcatchments defined by the streams and fdg grids
    each subcatchment is marked with unique id */
ral_grid_t RAL_CALL *ral_streams_subcatchments2(ral_grid_t *streams, ral_grid_t *fdg, ral_error_t **e);

/**
   number each stream section with a unique id 
*/
int RAL_CALL ral_streams_number(ral_grid_t *streams, ral_grid_t *fdg, ral_rcoords_t * c, int sid0);

/**
   number each stream section with a unique id 
*/
int RAL_CALL ral_streams_number2(ral_grid_t *streams, ral_grid_t *fdg, int sid0);

/*                          */
/* lakes grid is used below */
/*                          */

/* lakes should have (by default) only one outlet */

/** this removes other outlets from the lakes than the one with max ua by changing the fdg */
int RAL_CALL ral_fdg_kill_extra_outlets(ral_grid_t *fdg, ral_grid_t *lakes, ral_grid_t *uag, ral_error_t **e);

/**
   pruning removes streams shorter than min_l (give min_l in grid scale)
*/
int RAL_CALL ral_streams_prune(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, ral_rcoords_t * c, double min_l, ral_error_t **e);

/**
   pruning removes streams shorter than min_l (give min_l in grid scale)
*/
int RAL_CALL ral_streams_prune2(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, double min_l, ral_error_t **e);

/**
  renumbers the upstream reach of a stream which flows through a lake
  without junctions
  - break streams actually broken by a lake 
  - nsid is the first available stream id
*/
int RAL_CALL ral_streams_break(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, int nsid);

int RAL_CALL ral_streams_vectorize(ral_grid_t *streams, ral_grid_t *fdg, int row, int col);

#endif
