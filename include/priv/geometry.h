#ifndef	VECTOR_H
#define	VECTOR_H

/**\brief an XYZM point */
struct ral_point {
    double x;
    double y;  
    double z;
    double m;
};
typedef struct ral_point ral_point_t;

int ccw(ral_point_t p0, ral_point_t p1, ral_point_t p2);

/**\brief two points: begin and end */
struct ral_line {
    ral_point_t begin;
    ral_point_t end;
};
typedef struct ral_line ral_line_t;

int intersect(ral_line_t l1, ral_line_t l2);

/**\brief two points: SW and NE corners */
struct ral_rectangle {
    /** bottom left or south west */
    ral_point_t min;
    /** top right or north east */
    ral_point_t max;
};
typedef struct ral_rectangle ral_rectangle_t;

int clip_line_to_rect(ral_line_t *l, ral_rectangle_t r);

#define POINT_IN_RECTANGLE(p,r) \
(((p).x >= (r).min.x) AND ((p).x <= (r).max.x) AND \
 ((p).y >= (r).min.y) AND ((p).y <= (r).max.y))

#define RECTANGLES_OVERLAP(r1, r2) \
(!((r1.max.x < r2.min.x) OR	\
   (r1.min.y > r2.max.y) OR	\
   (r1.min.x > r2.max.x) OR	\
   (r1.max.y < r2.min.y)))

/**\brief an array of points (nodes) */
struct ral_polygon {
    /** nodes[0] must be nodes[n-1] and that is the user's responsibility */
    ral_point_t *nodes;
    /** number of nodes */
    int n;
};
typedef struct ral_polygon ral_polygon_t;

int ral_polygon_init(ral_polygon_t p, int n);
void ral_polygon_finish(ral_polygon_t p);
int ral_pnpoly(ral_point_t p, ral_polygon_t P);
double ral_polygon_area(ral_polygon_t p);

/**\brief for rendering a polygon with holes */
struct ral_active_edge_table {
    ral_polygon_t *p;
    int aet_begin;
    int scanline_at;
    /** nodes in y order */
    int *nodes;
    /** 0 = no, 
	1 = incoming edge is in, 
	2 = outgoing edge is in, 
	3 = both are 
    */
    int *active_edges;
};
typedef struct ral_active_edge_table ral_active_edge_table_t;

void ral_active_edge_table_array_destroy(ral_active_edge_table_t **aet, int n);

/** 
    Returns an array of aet's, one for each polygon.
    The list should be processed from min y to max y.
*/
ral_active_edge_table_t *ral_get_active_edge_table_array(ral_polygon_t *p, int n);

/** returns an ordered list of x's at y from the aet's */
int ral_scanline_at(ral_active_edge_table_t *aet_list, int n, double y, double **x, int *nx);
void ral_delete_scanline(double **x);

typedef struct {
    int points;
    int parts;
} counts;

#endif
