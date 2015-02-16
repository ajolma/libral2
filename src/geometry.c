#include "ral2/ral.h"
#include "priv/priv.h"

/* counterclockwise from Sedgewick: Algorithms in C */
int ccw(ral_point_t p0, ral_point_t p1, ral_point_t p2)
{
    double dx1, dx2, dy1, dy2;
    dx1 = p1.x - p0.x; dy1 = p1.y - p0.y;
    dx2 = p2.x - p0.x; dy2 = p2.y - p0.y;
    if (dx1*dy2 > dy1*dx2) return +1;
    if (dx1*dy2 < dy1*dx2) return -1;
    if ((dx1*dx2 < 0) OR (dy1*dy2 < 0)) return -1;
    if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2)) return +1;
    return 0;
}

/* from Sedgewick: Algorithms in C */
int intersect(ral_line_t l1, ral_line_t l2)
{
    return ((ccw(l1.begin, l1.end, l2.begin)
	     *ccw(l1.begin, l1.end, l2.end)) <= 0)
	&& ((ccw(l2.begin, l2.end, l1.begin)
	     *ccw(l2.begin, l2.end, l1.end)) <= 0);
}

int clip_line_to_rect(ral_line_t *l, ral_rectangle_t r)
{
    if (POINT_IN_RECTANGLE(l->begin,r) AND POINT_IN_RECTANGLE(l->end,r))
	return 1;
    if (((l->begin.x <= r.min.x) AND (l->end.x <= r.min.x)) OR
	((l->begin.x >= r.max.x) AND (l->end.x >= r.max.x)) OR
	((l->begin.y <= r.min.y) AND (l->end.y <= r.min.y)) OR
	((l->begin.y >= r.max.y) AND (l->end.y >= r.max.y)))
	return 0;
    /* scissors */
    if (l->begin.x != l->end.x) {
	double k = (l->end.y - l->begin.y)/(l->end.x - l->begin.x);
	if (l->begin.x <= r.min.x) {
	    l->begin.y = l->begin.y + k*(r.min.x - l->begin.x);
	    l->begin.x = r.min.x;
	} else if (l->end.x <= r.min.x) {
	    l->end.y = l->begin.y + k*(r.min.x - l->begin.x);
	    l->end.x = r.min.x;
	} else if (l->begin.x >= r.max.x) {
	    l->begin.y = l->begin.y + k*(r.max.x - l->begin.x);
	    l->begin.x = r.max.x;
	} else if (l->end.x >= r.max.x) {
	    l->end.y = l->begin.y + k*(r.max.x - l->begin.x);
	    l->end.x = r.max.x;
	} else if (l->begin.y <= r.min.y) {
	    l->begin.x = l->begin.x + (r.min.y - l->begin.y)/k;
	    l->begin.y = r.min.y;
	} else if (l->end.y <= r.min.y) {
	    l->end.x = l->begin.x + (r.min.y - l->begin.y)/k;
	    l->end.y = r.min.y;
	} else if (l->begin.y >= r.max.y) {
	    l->begin.x = l->end.x + (r.max.y - l->end.y)/k;
	    l->begin.y = r.max.y;
	} else if (l->end.y >= r.max.y) {
	    l->end.x = l->begin.x + (r.max.y - l->begin.y)/k;
	    l->end.y = r.max.y;
	}
    } else {
	l->begin.y = MIN(MAX(l->begin.y,r.min.y),r.max.y);
	l->end.y = MIN(MAX(l->end.y,r.min.y),r.max.y);
    }
    return 1;
}

int ral_polygon_init(ral_polygon_t p, int n)
{
    p.nodes = NULL;
    TEST(p.nodes = CALLOC(n, ral_point_t));
    p.n = n;
    return 1;
 fail:
    return 0;
}

void ral_polygon_free(ral_polygon_t p)
{
    if (p.nodes) free(p.nodes);
    p.nodes = NULL;
}

/*
  from http://exaflop.org/docs/cgafaq1/cga2.html: It returns 1 for
  strictly interior points, 0 for strictly exterior, and 0 or 1 for
  points on the boundary. The boundary behavior is complex but
  determined; | in particular, for a partition of a region into
  polygons, each point | is "in" exactly one polygon.
 */
int ral_pnpoly(ral_point_t p, ral_polygon_t P)
{
    int i, j, c = 0;
    for (i = 0, j = P.n-1; i < P.n; j = i++) {
	if ((((P.nodes[i].y <= p.y) AND (p.y < P.nodes[j].y)) OR
	     ((P.nodes[j].y <= p.y) AND (p.y < P.nodes[i].y))) AND
	    (p.x < (P.nodes[j].x - P.nodes[i].x) * (p.y - P.nodes[i].y) / 
	     (P.nodes[j].y - P.nodes[i].y) + P.nodes[i].x))
	    c = !c;
    }
    return c;
}

/* from http://astronomy.swin.edu.au/~pbourke/geometry/clockwise/source1.c */
double ral_polygon_area(ral_polygon_t p)
{
    int i;
    double A = 0;
    for (i=0;i<p.n-1;i++) {
	A += p.nodes[i].x * p.nodes[i+1].y - p.nodes[i+1].x * p.nodes[i].y;
    }
    return A/2;
}

void sort_nodes(ral_polygon_t p, int *nodes, int begin, int end)
{
    if (end > begin) {
	int pivot = nodes[begin];
	int l = begin + 1;
	int r = end;
	int temp;
	while (l < r) {
	    if (p.nodes[nodes[l]].y <= p.nodes[pivot].y) {
		l++;
	    } else {
		r--;
		swap(nodes[l], nodes[r], temp);
	    }
	}
	l--;
	swap(nodes[begin], nodes[l], temp);
	sort_nodes(p, nodes, begin, l);
	sort_nodes(p, nodes, r, end);
    }
}

void sort_double(double *array, int begin, int end)
{
    if (end > begin) {
	double pivot = array[begin];
	int l = begin + 1;
	int r = end;
	double temp;
	while (l < r) {
	    if (array[l] <= pivot) {
		l++;
	    } else {
		r--;
		swap(array[l], array[r], temp);
	    }
	}
	l--;
	swap(array[begin], array[l], temp);
	sort_double(array, begin, l);
	sort_double(array, r, end);
    }
}

void ral_active_edge_table_array_destroy(ral_active_edge_table_t **aet, int n)
{
    int i;
    if (!*aet) return;
    for (i = 0; i < n; i++) {
	if ((*aet)[i].nodes) free((*aet)[i].nodes);
	if ((*aet)[i].active_edges) free((*aet)[i].active_edges);
    }
    free(*aet);
    *aet = NULL;
}

ral_active_edge_table_t *ral_get_active_edge_table_array(ral_polygon_t *p, int n)
{
    ral_active_edge_table_t *aet;
    int i;
    TEST(aet = CALLOC(n, ral_active_edge_table_t));
    for (i = 0; i < n; i++) {
	aet[i].p = &(p[i]);
	aet[i].aet_begin = 0;
	aet[i].scanline_at = 0;
	aet[i].nodes = NULL;
	aet[i].active_edges = NULL;
    }
    for (i = 0; i < n; i++) {
	TEST(aet[i].nodes = CALLOC(p[i].n-1, int));
	TEST(aet[i].active_edges = CALLOC(p[i].n-1, int));
    }
    for (i = 0; i < n; i++) {
	int j;
	for (j = 0; j < p[i].n-1; j++) { /* the last node is the same as first, skip that */
	    aet[i].nodes[j] = j;
	    aet[i].active_edges[j] = 0;
	}
	sort_nodes(p[i], aet[i].nodes, 0, p[i].n-1); /* smallest to biggest */
    }
    return aet;
 fail:
    ral_active_edge_table_array_destroy(&aet, n);
    return NULL;
}

void ral_delete_scanline(double **x)
{
    free(*x);
    *x = NULL;
}

int ral_scanline_at(ral_active_edge_table_t *aet_list, int n, double y, double **x, int *nx)
{
    int i;
    *nx = 0;
    *x = NULL;

    for (i = 0; i < n; i++) { /* working on polygon aet_list[i].p */

	ral_active_edge_table_t *aet = &(aet_list[i]);

	/* process all nodes from current scanline_at to nodes whose y is <= y */
	while ((aet->scanline_at < aet->p->n-1) AND 
	       (aet->p->nodes[ aet->nodes[aet->scanline_at] ].y <= y)) {

	    int prev_node = aet->nodes[aet->scanline_at];
	    int next_node = aet->nodes[aet->scanline_at];
	    if (prev_node < 1)
		prev_node = aet->p->n-2; /* the last node is the same as first */
	    else
		prev_node--;
	    if (next_node >= aet->p->n-2)
		next_node = 0;
	    else
		next_node++;

	    /* process node aet->nodes[aet->scanline_at] */

	    /* incoming node is already in aet if outgoing node of prev node is */

	    /* outgoing node is already in aet if incoming node of next node is */

	    if (!(aet->active_edges[prev_node] & 2) AND !(aet->active_edges[next_node] & 1)) {

		/* if neither node is in (this is minimum node), just add them */
		aet->active_edges[aet->nodes[aet->scanline_at]] = 3;

	    } else if ((aet->active_edges[prev_node] & 2) AND (aet->active_edges[next_node] & 1)) {

		/* if both are in (this is maximum node), just remove them */
		aet->active_edges[prev_node] &= 1;
		aet->active_edges[next_node] &= 2;

	    } else if (aet->active_edges[prev_node] & 2) {

		/* remove the outgoing from the prev and add outgoing to this */
		aet->active_edges[prev_node] &= 1;
		aet->active_edges[aet->nodes[aet->scanline_at]] = 2;

	    } else { /*if (aet->active_edges[next_node] & 1) {*/

		/* remove the incoming from the next and add incoming to this */
		aet->active_edges[next_node] &= 2;
		aet->active_edges[aet->nodes[aet->scanline_at]] = 1;

	    }

	    (aet->scanline_at)++;
	}

	/* check if we can forget some nodes with small y */
	while ((aet->aet_begin < aet->scanline_at) AND (aet->active_edges[aet->nodes[aet->aet_begin]] == 0)) {
	    aet->aet_begin++;
	}

	/* collect all x values from active edges */
	{
	    int j,n = 0;
	    for (j = aet->aet_begin; j < aet->scanline_at; j++) {
		int status = aet->active_edges[ aet->nodes[j] ];
		if (status == 3)
		    n += 2;
		else if (status > 0)
		    n++;
	    }
	    if (n > 0) {
		TEST(*x = REALLOC(*x, *nx+n, double));
		n = *nx;
		for (j = aet->aet_begin; j < aet->scanline_at; j++) {
		    
		    int node = aet->nodes[j];
		    
		    /* x is the x of the intersection of line y = y, and the edge */
		    
		    if (aet->active_edges[node] & 1) { 
			
			/* incoming edge of the node is in aet */
			
			ral_point_t *nodes = aet->p->nodes;
			int prev_node = node;
			if (prev_node < 1)
			    prev_node = aet->p->n-2; /* the last node is the same as first */
			else
			prev_node--;
		    
			if (nodes[node].y == nodes[prev_node].y)
			    (*x)[n] = MIN(nodes[node].x,nodes[prev_node].x);
			else if (nodes[node].x == nodes[prev_node].x)
			    (*x)[n] = nodes[node].x;
			else 
			    (*x)[n] = nodes[node].x + 
				(y-nodes[node].y)*(nodes[prev_node].x-nodes[node].x)/
				(nodes[prev_node].y-nodes[node].y);			
			n++;

		    }

		    if (aet->active_edges[node] & 2) { 

			/* outgoing edge of node aet->nodes[j] is in aet */
			
			ral_point_t *nodes = aet->p->nodes;
			int next_node = node;
			if (next_node >= aet->p->n-2)
			    next_node = 0;
			else
			    next_node++;
		    
			if (nodes[node].y == nodes[next_node].y)
			    (*x)[n] = MIN(nodes[node].x,nodes[next_node].x);
			else if (nodes[node].x == nodes[next_node].x)
			    (*x)[n] = nodes[node].x;
			else 
			    (*x)[n] = nodes[node].x + 
				(y-nodes[node].y)*(nodes[next_node].x-nodes[node].x)/
				(nodes[next_node].y-nodes[node].y);
			n++;
			
		    }

		}
		*nx = n;
	    }
	    
	}
    }
    sort_double(*x, 0, *nx); /* smallest to biggest */
    return 1;
 fail:
    return 0;
}

ral_object_t *Geometry_new(ral_object_t *_self)
{
    ral_geometry_t *self = _self;
    self->points = NULL;
    self->n_points = 0;
    self->parts = NULL;
    self->n_parts = 0;
    self->part_types = NULL;
    self->type = 0;
    return _self;
}

void Geometry_delete(ral_object_t *_self)
{
    ral_geometry_t *self = _self;
    if (self->points) free(self->points);
    if (self->parts) free(self->parts);
    if (self->part_types) free(self->part_types);
    return;
}

const char *Geometry_name()
{
    return "Geometry";
}

static const struct Class _Geometry = {
    sizeof(struct ral_geometry),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Geometry_new,
    /*  2 */ NULL,
    /*  3 */ Geometry_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Geometry_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralGeometry = &_Geometry;

int ral_geometry_set(ral_geometry_t *self, int n_points, int n_parts)
{
    TEST(self->points = CALLOC(n_points, ral_point_t));
    TEST(self->parts = CALLOC(n_parts, ral_polygon_t));
    TEST(self->part_types = CALLOC(n_parts, OGRwkbGeometryType));
    self->n_points = n_points;
    self->n_parts = n_parts;
    return 1;
 fail:
    return 0;
}

counts get_counts(OGRGeometryH geom)
{
    counts c = {0,0};
    int k = OGR_G_GetGeometryCount(geom);
    if (k) {
	int i;
	for (i = 0; i < k; i++) {
	    counts x = get_counts(OGR_G_GetGeometryRef(geom, i));
	    c.points += x.points;
	    c.parts += x.parts;
	}
    } else {
	c.points += OGR_G_GetPointCount(geom);
	c.parts++;
    }
    return c;
}

void fill_geometry(ral_geometry_t *self, OGRGeometryH geom, counts *c)
{
    int k = OGR_G_GetGeometryCount(geom);
    if (k) {
	int i;
	for (i = 0; i < k; i++) {
	    fill_geometry(self, OGR_G_GetGeometryRef(geom, i), c);
	}
    } else {
	int i;
	int k = OGR_G_GetPointCount(geom);
	self->part_types[c->parts] = OGR_G_GetGeometryType(geom);
	self->parts[c->parts].nodes = &(self->points[c->points]);
	self->parts[c->parts].n = k;
	for (i = 0; i < k; i++) {
	    self->points[c->points+i].x = OGR_G_GetX(geom, i);
	    self->points[c->points+i].y = OGR_G_GetY(geom, i);
	    self->points[c->points+i].z = OGR_G_GetZ(geom, i);
	    /* todo: M */
	}
	c->points+=k;
	c->parts++;
    }
}

int ral_geometry_set_from_ogr_geometry(ral_geometry_t *self, OGRGeometryH geom)
{
    /* geometry = array of geometries or array of points */
    ASSERT(geom, BAD)
    counts c = get_counts(geom);
    FAIL_UNLESS(ral_geometry_set(self, c.points, c.parts));
    self->type = OGR_G_GetGeometryType(geom);
    c.points = 0;
    c.parts = 0;
    fill_geometry(self, geom, &c);
    return 1;
 fail:
    return 0;
}
