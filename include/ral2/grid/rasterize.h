#ifndef RAL_GRID_RASTERIZE_H
#define RAL_GRID_RASTERIZE_H

/**\file ral2/grid/rasterize.h
   \brief Rasterize methods for grids

*/

/** Drawing methods */

void RAL_CALL ral_grid_line(ral_grid_t *gd, ral_rcoords_t * c1, ral_rcoords_t * c2, ral_object_t *pen);
void RAL_CALL ral_grid_filled_rect(ral_grid_t *gd, ral_rcoords_t * c1, ral_rcoords_t * c2, ral_object_t *pen);
int RAL_CALL ral_grid_filled_polygon(ral_grid_t *gd, ral_geometry_t *g, ral_object_t *pen);
void RAL_CALL ral_grid_filled_circle(ral_grid_t *gd, ral_rcoords_t * c, int r, ral_object_t *pen);

/** 
    - value_field -1 => use value 1
*/
int RAL_CALL ral_grid_rasterize_feature(ral_grid_t *gd, OGRFeatureH f, int value_field, OGRFieldType ft);
int RAL_CALL ral_grid_rasterize_layer(ral_grid_t *gd, OGRLayerH l, int value_field);

ral_array_t * RAL_CALL ral_grid_get_line(ral_grid_t *gd, ral_rcoords_t * c1, ral_rcoords_t * c2);
ral_array_t * RAL_CALL ral_grid_get_rect(ral_grid_t *gd, ral_rcoords_t * c1, ral_rcoords_t * c2);
ral_array_t * RAL_CALL ral_grid_get_circle(ral_grid_t *gd, ral_rcoords_t * c, int r);

/** - marks (0/1) the flooded area in "done" grid if given - it may be null or an integer grid
    - connectivity is either 8 or 4 */
void RAL_CALL ral_grid_floodfill(ral_grid_t *gd, ral_grid_t *filled_area, ral_rcoords_t * c, ral_object_t *pen, int connectivity);

#endif
