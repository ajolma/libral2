#ifndef RAL_GEOMETRY_H
#define RAL_GEOMETRY_H

/**\file ral2/geometry.h
   \brief Geometry classes.

   A geometry object is a point, line, rectangle, polygon, or a collection of those. 
*/

extern ral_class ralGeometry;
typedef struct ral_geometry ral_geometry_t;

#ifdef XX
typedef struct point ral_point;
typedef struct line ral_line;
typedef struct rectangle ral_rectangle;
typedef struct polygon ral_polygon;

int RAL_CALL ral_ccw(ral_point p0, ral_point p1, ral_point p2);
int RAL_CALL ral_intersect(ral_line l1, ral_line l2);
int RAL_CALL ral_clip_line_to_rect(ral_line *l, ral_rectangle r);
int RAL_CALL ral_polygon_init(ral_polygon p, int n);
void RAL_CALL ral_polygon_finish(ral_polygon p);
int RAL_CALL ral_pnpoly(ral_point p, ral_polygon P);
double RAL_CALL ral_polygon_area(ral_polygon p);
#endif

int ral_geometry_set(ral_geometry_t *self, int n_points, int n_parts);
int ral_geometry_set_from_ogr_geometry(ral_geometry_t *self, OGRGeometryH geom);

#endif
