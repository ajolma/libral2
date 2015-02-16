#ifndef RAL_COORDS_H
#define RAL_COORDS_H

/**\file ral2/coords.h
   \brief Coordinate system classes.

   A rcoords is a location on a raster, a wcoords is a location on a
   map, and wbox is a rectangular region on a map.
*/

extern ral_class ralRCoords;
extern ral_class ralWCoords;
extern ral_class ralWBox;
typedef struct ral_rcoords ral_rcoords_t;
typedef struct ral_wcoords ral_wcoords_t;
typedef struct ral_wbox ral_wbox_t;

ral_rcoords_t * ral_rcoords_new(int x, int y);
void ral_rcoords_set(ral_rcoords_t * self, int x, int y);
int ral_rcoords_get_x(ral_rcoords_t * self);
int ral_rcoords_get_y(ral_rcoords_t * self);

ral_wcoords_t * ral_wcoords_new(double x, double y);
void ral_wcoords_set(ral_wcoords_t * self, double x, double y);
double ral_wcoords_get_x(ral_wcoords_t * self);
double ral_wcoords_get_y(ral_wcoords_t * self);

ral_wbox_t * ral_wbox_new(double x_min, double y_min, double x_max, double y_max);
double ral_wbox_get_x_min(ral_wbox_t * self);
double ral_wbox_get_y_min(ral_wbox_t * self);
double ral_wbox_get_x_max(ral_wbox_t * self);
double ral_wbox_get_y_max(ral_wbox_t * self);


#endif
