#include "ral2/ral.h"
#include "priv/priv.h"

ral_hash_t * ral_grid_get_neighbors(ral_grid_t *gd)
{
    ral_hash_t * neighbors = ral_new(ralHash);
    if (gd->type == integer) {
        ral_cell_t c;
        ral_integer_t * key = ral_integer_new(0);
        FOR(c, gd) {
            key->value = (long)GCiv(gd, c);
            ral_hash_t * neighbor_hash = ral_lookup(neighbors, key);
            if (!neighbor_hash) {
                neighbor_hash = ral_new(ralHash);
                ral_insert(neighbors, neighbor_hash, key);
            }
            int dir;
            RAL_DIRECTIONS(dir) {
                ral_cell_t t = ral_cell_move(c, dir);
                if (GCin(gd, t)) { /* MASKING!!! */
                    ral_integer_t * key = ral_integer_new(0);
                    key->value = (long)GCiv(gd, t);
                    if (!ral_lookup(neighbor_hash, key))
                        ral_insert(neighbor_hash, ral_integer_new(1), key);
                    ral_delete(key);
                }
            }
        }
        ral_delete(key);
    }
    return neighbors;
}

ral_grid_t *ral_grid_ca_step(ral_grid_t *gd, void *k, ral_error_t **e)
{
    ral_grid_t *g = NULL;
    if (gd->type == integer) {
        RAL_INTEGER *a = (RAL_INTEGER *)k;
        ral_cell_t c;
        FAIL_UNLESS(g = ral_grid_new_like(gd, integer, e));
        FOR(c, gd) {
            if (GCivD(gd, c)) {
                RAL_INTEGER result = a[0]*GCiv(gd, c);
                int dir;
                RAL_DIRECTIONS(dir) {
                    ral_cell_t d = ral_cell_move(c, dir);
                    if (GCin(gd, d) AND GCivD(gd, d))
                        result += a[dir]*GCiv(gd, d);
                }
                GCiv(g, c) = result;
            }
        }
    } else {
        RAL_REAL *a = (RAL_REAL *)k;
        ral_cell_t c;
        FAIL_UNLESS(g = ral_grid_new_like(gd, real, NULL));
        FOR(c, gd) {
            if (GCnvD(gd, c)) {
                RAL_REAL result = a[0]*GCnv(gd, c);
                int dir;
                RAL_DIRECTIONS(dir) {
                    ral_cell_t d = ral_cell_move(c, dir);
                    if (GCin(gd, d) AND GCnvD(gd, d))
                        result += a[dir]*GCnv(gd, d);
                }
                GCnv(g, c) = result;
            }
        }
    }
    return g;
 fail:
    ral_delete(g);
    return NULL;
}

ral_grid_t *ral_grid_bufferzone(ral_grid_t *gd, int z, double w, ral_error_t **e)
{
    ral_grid_t *grid;
    ral_cell_t c;
    int r = ROUND(w);
    if (!(grid = ral_grid_new_like(gd, integer, e))) return NULL;
    ASSERT(gd->type == integer, BAD);
    FOR(c, gd) {
        if (GCiv(gd, c) == z)
            FILLED_CIRCLE(grid, c, r, 1, INTEGER_GRID_SET_CELL);
    }
    return grid;
 fail:
    ral_delete(grid);
    return NULL;
}

int ral_grid_zonesize_internal(ral_grid_t *gd, ral_grid_t *visited, ral_cell_t c, int color, int connectivity)
{
    int lastBorder;
    int leftLimit, rightLimit;
    int i2;
    int size = 0;
    if (GCiv(gd, c) != color) return 0;
    /* Seek left */
    leftLimit = -1;
    for (i2 = c.y; i2 >= 0; i2--) {
        if (Giv(gd, i2, c.x) != color) break;
        size++;
        Giv(visited, i2, c.x) = 1;
        if (connectivity == 8)
            leftLimit = MAX(0,i2-1);
        else
            leftLimit = i2;
    }
    if (leftLimit == -1) return 1;
    /* Seek right */
    if (connectivity == 8)
        rightLimit = MIN(gd->height-1,c.y+1);
    else
        rightLimit = c.y;
    for (i2 = c.y+1; i2 < gd->height; i2++) {        
        if (Giv(gd, i2, c.x) != color) break;
        size++;
        Giv(visited, i2, c.x) = 1;
        if (connectivity == 8)
            rightLimit = MIN(gd->height-1,i2+1);
        else
            rightLimit = i2;
    }
    /* Look at lines above and below */
    /* Above */
    if (c.x > 0) {
        ral_cell_t c2;
        c2.x = c.x-1;
        lastBorder = 1;
        for (c2.y = leftLimit; c2.y <= rightLimit; c2.y++) {
            int a;
            a = GCiv(gd, c2);
            if (lastBorder) {
                if (a == color) {        
                    if (!GCiv(visited, c2))
                        size += ral_grid_zonesize_internal(gd, visited, c2, color, connectivity);
                    lastBorder = 0;
                }
            } else if (a != color)
                lastBorder = 1;
        }
    }
    /* Below */
    if (c.x < gd->width - 1) {
        ral_cell_t c2;
        c2.x = c.x+1;
        lastBorder = 1;
        for (c2.y = leftLimit; c2.y <= rightLimit; c2.y++) {
            int a;
            a = GCiv(gd, c2);
            if (lastBorder) {
                if (a == color) {
                    if (!GCiv(visited, c2))
                        size += ral_grid_zonesize_internal(gd, visited, c2, color, connectivity);
                    lastBorder = 0;
                }
            } else if (a != color)
                lastBorder = 1;
        }
    }
    return size;
}

int ral_grid_rzonesize_internal(ral_grid_t *gd, ral_grid_t *visited, ral_cell_t c, RAL_REAL color, int connectivity)
{
    int lastBorder;
    int leftLimit, rightLimit;
    int i2;
    int size = 0;
    if (GCnv(gd, c) != color) return 0;
    /* Seek left */
    leftLimit = -1;
    for (i2 = c.y; i2 >= 0; i2--) {
        if (Gnv(gd, i2, c.x) != color) break;
        size++;
        Giv(visited, i2, c.x) = 1;
        if (connectivity == 8)
            leftLimit = MAX(0,i2-1);
        else
            leftLimit = i2;
    }
    if (leftLimit == -1) return 1;
    /* Seek right */
    if (connectivity == 8)
        rightLimit = MIN(gd->height-1,c.y+1);
    else
        rightLimit = c.y;
    for (i2 = c.y+1; i2 < gd->height; i2++) {        
        if (Gnv(gd, i2, c.x) != color) break;
        size++;
        Giv(visited, i2, c.x) = 1;
        if (connectivity == 8)
            rightLimit = MIN(gd->height-1,i2+1);
        else
            rightLimit = i2;
    }
    /* Look at lines above and below */
    /* Above */
    if (c.x > 0) {
        ral_cell_t c2;
        c2.x = c.x-1;
        lastBorder = 1;
        for (c2.y = leftLimit; c2.y <= rightLimit; c2.y++) {
            int a;
            a = GCnv(gd, c2);
            if (lastBorder) {
                if (a == color) {
                    if (!GCiv(visited, c2))
                        size += ral_grid_rzonesize_internal(gd, visited, c2, color, connectivity);
                    lastBorder = 0;
                }
            } else if (a != color)
                lastBorder = 1;
        }
    }
    /* Below */
    if (c.x < gd->width - 1) {
        ral_cell_t c2;
        c2.x = c.x+1;
        lastBorder = 1;
        for (c2.y = leftLimit; c2.y <= rightLimit; c2.y++) {
            int a;
            a = GCnv(gd, c2);
            if (lastBorder) {
                if (a == color) {
                    if (!GCiv(visited, c2))
                        size += ral_grid_rzonesize_internal(gd, visited, c2, color, connectivity);
                    lastBorder = 0;
                }
            } else if (a != color)
                lastBorder = 1;
        }
    }
    return size;
}

double ral_grid_zonesize(ral_grid_t *gd, ral_rcoords_t * rc, ral_error_t **e)
{
    ral_cell_t c = {rc->y, rc->x};
    
    ral_grid_t *visited = NULL;
    double size = 0;
    ASSERT(GCin(gd, c), COB);
    FAIL_UNLESS(visited = ral_grid_new_like(gd, integer, e));
    if (gd->type == integer) {
        size = gd->cell_size*gd->cell_size*
            (double)ral_grid_zonesize_internal(gd, visited, c, GCiv(gd, c), 8);
    } else {
        size = gd->cell_size*gd->cell_size*
            (double)ral_grid_rzonesize_internal(gd, visited, c, GCnv(gd, c), 8);
    }
    ral_delete(visited);
    return size;
 fail:
    ral_delete(visited);
    return 0;
}

ral_grid_t *ral_grid_areas(ral_grid_t *gd, int k, ral_error_t **e)
{
    ral_grid_t *areas;
    ral_cell_t c;
    ASSERT(gd->type == integer, BAD);
    if (!(areas = ral_grid_new_like(gd, integer, e))) return NULL;
    FOR(c, gd) {
        /* cell is part of an area 
           if there is at least 
           k consecutive nonzero cells as neighbors */
        int r, d, co = 0, cm = 0;
        if (!GCiv(gd, c) OR GCivND(gd, c)) continue;
        for (r = 0; r < 2; r++) {
            for (d = 1; d < 9; d++) {
                ral_cell_t x = ral_cell_move(c, d);
                if (GCin(gd, x) AND GCiv(gd, x))
                    co++;
                else {
                    if (co > cm) cm = co;
                    co = 0;
                }
            }
        }
        if (co > cm) cm = co;
        if (cm >= k) GCiv(areas, c) = GCiv(gd, c);
    }
    return areas;
 fail:
    return NULL;
}

int ral_grid_connect(ral_grid_t *gd)
{
    ral_cell_t c;
    ASSERT(gd->type == integer, BAD);
    FOR(c, gd) {
        /* connect within 3x3 mask */
        if (GCiv(gd, c)) continue;
        else {
            int d, k = 0;
            /* d <-> d + 3,4,5 */
            for (d = 1; d < 9; d++) {
                int e = d + 4;
                ral_cell_t cd = c, ce = c;
                if (e > 8) e -= 8;  
                cd = ral_cell_move(cd, d);
                ce = ral_cell_move(ce, e);
                if (GCin(gd, cd) AND GCin(gd, ce) AND 
                    GCiv(gd, cd) AND GCiv(gd, cd) == GCiv(gd, ce)) {
                    k =  GCiv(gd, cd);
                    break;
                }
            }
            GCiv(gd, c) = k;
        }
    }
    return 1;
 fail:
    return 0;
}

int ral_grid_number_areas(ral_grid_t *gd, int connectivity, ral_error_t **e)
{
    ral_cell_t c;
    ral_grid_t *done = NULL;
    ASSERT(gd->type == integer, BAD);
    FAIL_UNLESS(done = ral_grid_new_like(gd, integer, e));
    ral_integer_t * k = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivD(gd, c) AND !GCiv(done, c)) {
            k->value++;
            ral_grid_floodfill(gd, done, ral_rcoords_new(c.x, c.y), k, connectivity);
        }
    }
    ral_delete(done);
    return 1;
fail:
    return 0;
}
ral_grid_t *ral_grid_distances(ral_grid_t *gd, ral_error_t **e)
{
    ral_grid_t *d = NULL;
    ral_cell_t c;
    FAIL_UNLESS(d = ral_grid_new_like(gd, real, NULL));
    if (gd->type == integer) {
        FOR(c, gd) {
            if (!GCivD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCnv(d, c) = DISTANCE_BETWEEN_CELLS(c, nn) * gd->cell_size;
            }
        }
    } else {
        FOR(c, gd) {
            if (!GCnvD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCnv(d, c) = DISTANCE_BETWEEN_CELLS(c, nn) * gd->cell_size;
            }
        }
    }
    return d;
 fail:
    ral_delete(d);
    return NULL;
}

ral_grid_t *ral_grid_directions(ral_grid_t *gd, ral_error_t **e)
{
    ral_grid_t *d = NULL;
    ral_cell_t c;
    FAIL_UNLESS(d = ral_grid_new_like(gd, real, NULL));
    if (gd->type == integer) {
        FOR(c, gd) {
            if (!GCivD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCnv(d, c) = atan2(c.y-nn.y, nn.x-c.x);
            }
        }
    } else {
        FOR(c, gd) {
            if (!GCnvD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCnv(d, c) = atan2(c.y-nn.y, nn.x-c.x);
            }
        }
    }
    return d;
 fail:
    ral_delete(d);
    return NULL;
}

#define COMPARISON                                      \
    { if (GCivD(gd, x)) {                               \
            double t = DISTANCE_BETWEEN_CELLS(c, x);    \
            if (d < 0 OR t < d) {                       \
                d = t;                                  \
                ret = x;                                \
            }                                           \
        }                                               \
    }

#define RCOMPARISON                                     \
    { if (GCnvD(gd, x)) {                                \
            double t = DISTANCE_BETWEEN_CELLS(c, x);        \
            if (d < 0 OR t < d) {                       \
                d = t;                                  \
                ret = x;                                \
            }                                           \
        }                                               \
    }

ral_rcoords_t * ral_grid_nearest_neighbor(ral_grid_t *gd, ral_rcoords_t * rc)
{
    ral_cell_t c = {rc->y, rc->x};
    int r = 1;
    double d = -1;
    ral_cell_t ret = {-1, -1};
    if (gd->type == integer) {
        /* the first candidate */
        while (d < 0 AND r < MIN(gd->height, gd->width)) {
            ral_cell_t x;
            int imin = c.y-r, jmin = c.x-r, imax = c.y+r+1, jmax = c.x+r+1;
            int left = 1, right = 1, top = 1, bottom = 1;
            if (imin < 0) {imin = 0;top = 0;}
            if (jmin < 0) {jmin = -1;left = 0;}
            if (imax > gd->height) {imax = gd->height;bottom = 0;}
            if (jmax > gd->width) {jmax = gd->width+1;right = 0;}
            if (left) {
                x.x = jmin;
                for (x.y = imin; x.y < imax; x.y++) COMPARISON;
            }
            if (right) {
                x.x = jmax-1;
                for (x.y = imin; x.y < imax; x.y++) COMPARISON;
            }
            if (top) {
                x.y = imin;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) COMPARISON;
            }
            if (bottom) {
                x.y = imax - 1;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) COMPARISON;
            }
            r++;
        }
        ASSERT(d >= 0, "No data found in the grid. Fail.");
        /* we must now enlarge the rectangle until it is
           larger than d and check */
        while (r < d) {
            ral_cell_t x;
            int imin = c.y-r, jmin = c.x-r, imax = c.y+r+1, jmax = c.x+r+1;
            int left = 1, right = 1, top = 1, bottom = 1;
            if (imin < 0) {imin = 0;top = 0;}
            if (jmin < 0) {jmin = -1;left = 0;}
            if (imax > gd->height) {imax = gd->height;bottom = 0;}
            if (jmax > gd->width) {jmax = gd->width+1;right = 0;}
            if (left) {
                x.x = jmin;
                for (x.y = imin; x.y < imax; x.y++) COMPARISON;
            }
            if (right) {
                x.x = jmax-1;
                for (x.y = imin; x.y < imax; x.y++) COMPARISON;
            }
            if (top) {
                x.y = imin;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) COMPARISON;
            }
            if (bottom) {
                x.y = imax - 1;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) COMPARISON;
            }
            r++;
        }
    } else {
        while (d < 0 AND r < MIN(gd->height, gd->width)) {
            ral_cell_t x;
            int imin = c.y-r, jmin = c.x-r, imax = c.y+r+1, jmax = c.x+r+1;
            int left = 1, right = 1, top = 1, bottom = 1;
            if (imin < 0) {imin = 0;top = 0;}
            if (jmin < 0) {jmin = -1;left = 0;}
            if (imax > gd->height) {imax = gd->height;bottom = 0;}
            if (jmax > gd->width) {jmax = gd->width+1;right = 0;}
            if (left) {
                x.x = jmin;
                for (x.y = imin; x.y < imax; x.y++) RCOMPARISON;
            }
            if (right) {
                x.x = jmax-1;
                for (x.y = imin; x.y < imax; x.y++) RCOMPARISON;
            }
            if (top) {
                x.y = imin;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) RCOMPARISON;
            }
            if (bottom) {
                x.y = imax - 1;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) RCOMPARISON;
            }
            r++;
        }
        ASSERT(d >= 0, "No data found in the grid. Fail.");
        /* we must now enlarge the rectangle until it is
           larger than d and check */
        while (r < d) {
            ral_cell_t x;
            int imin = c.y-r, jmin = c.x-r, imax = c.y+r+1, jmax = c.x+r+1;
            int left = 1, right = 1, top = 1, bottom = 1;
            if (imin < 0) {imin = 0;top = 0;}
            if (jmin < 0) {jmin = -1;left = 0;}
            if (imax > gd->height) {imax = gd->height;bottom = 0;}
            if (jmax > gd->width) {jmax = gd->width+1;right = 0;}
            if (left) {
                x.x = jmin;
                for (x.y = imin; x.y < imax; x.y++) RCOMPARISON;
            }
            if (right) {
                x.x = jmax-1;
                for (x.y = imin; x.y < imax; x.y++) RCOMPARISON;
            }
            if (top) {
                x.y = imin;
                for (x.x = jmin+1; x.x < jmax-1; x.x++) RCOMPARISON;
            }
            if (bottom) {
                x.y = imax - 1;
                for (x.x = jmin+1; x.x < jmax-1; x.x++)RCOMPARISON;
            }
            r++;
        }
    }
 fail:
    return ral_rcoords_new(ret.x, ret.y);
}

ral_grid_t *ral_grid_nn(ral_grid_t *gd, ral_error_t **e)
{
    ral_cell_t c;
    ral_grid_t *n = NULL;
    FAIL_UNLESS(n = ral_grid_new_copy(gd, gd->type, NULL));
    if (gd->type == integer) {
        FOR(c, gd) {
            if (!GCivD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCiv(n, c) = GCiv(n, nn);
            }
        }
    } else {
        FOR(c, gd) {
            if (!GCnvD(gd, c)) {
                ral_rcoords_t * rc = ral_grid_nearest_neighbor(gd, ral_rcoords_new(c.x, c.y));
                ral_cell_t nn = {rc->y, rc->x};
                ral_delete(rc);
                CHECK(nn.y >= 0);
                GCnv(n, c) = GCnv(n, nn);
            }
        }
    }
    return n;
fail:
    ral_delete(n);
    return NULL;
}

/* from wikipedia.org */

#define RAL_INFINITY 10e9

void ral_Dijkstra(ral_DijkVertex *graph, int nodecount, int source) 
{
    int i;
    for(i = 0; i < nodecount; i++) {
        if(i == source) {
            graph[i].distance = 0;
            graph[i].isDead = 0;
        } else {
            graph[i].distance = RAL_INFINITY;
            graph[i].isDead = 0;
         }
    }
    for(i = 0; i < nodecount; i++) {
        int next = 0;
        float min = 2*RAL_INFINITY;
        int j;
        for(j = 0; j < nodecount; j++) {
            if(!graph[j].isDead AND graph[j].distance < min) {
                next = j;
                min = graph[j].distance;
            }
        }
        for(j = 0; j < graph[next].numconnect; j++) {
            if(graph[graph[next].connections[j].dest].distance >
               graph[next].distance + graph[next].connections[j].weight)
            {
                graph[graph[next].connections[j].dest].distance =
                    graph[next].distance + graph[next].connections[j].weight;
            }
        }
        graph[next].isDead = 1;
    }
}

ral_grid_t *ral_grid_dijkstra(ral_grid_t *w, ral_rcoords_t * rc, ral_error_t **e)
{
    /* 
       in w are the weights, i,j is the destination 
       the cost to travel from a cell a to neighboring cell b in w is the value
       of w at a times 0.5 or sqrt(2)/2 + value of w at b times 0.5 or sqrt(2)/2
       if the value at w is < 1 the cell cannot be entered
    */
    /* put to graph all cells in w having value > 1 */
    ral_cell_t c = {rc->y, rc->x};
    int nodecount = 0;
    int source = -1;
    ral_DijkVertex *graph = NULL;
    int *cell2node = NULL;
    int node;

    ral_grid_t *cost = NULL;

    FAIL_UNLESS(cost = ral_grid_new_like(w, real, NULL));
    ral_grid_set_all(cost, ral_integer_new(-1), e);
    GCnv(cost, c) = 0;

    TEST(cell2node = CALLOC(w->height*w->width, int));
    {
        ral_cell_t d;
        FOR(d, w) {
            if (GCv(w, d) >= 1) {
                cell2node[GRID_INDEX(d.y, d.x, w->width)] = nodecount;
                if ((d.y == c.y) AND (d.x == c.x)) source = nodecount;
                nodecount++;
            } else {
                cell2node[GRID_INDEX(d.y, d.x, w->width)] = -1;
            }
        }
    }
    TEST(graph = CALLOC(nodecount, ral_DijkVertex));
    
    for (node = 0; node < nodecount; node++)
        graph[node].connections = NULL;
    node = 0;

    FOR(c, w) {
        if (GCv(w, c) >= 1) {
            int dir;
            graph[node].numconnect = 0;
            RAL_DIRECTIONS(dir) {
                ral_cell_t d = ral_cell_move(c, dir);
                if (GCin(w, d) AND (GCv(w, d) >= 1))
                    graph[node].numconnect++;
            }
            if (graph[node].numconnect) {
                int connect = 0;
                TEST(graph[node].connections = 
                           CALLOC(graph[node].numconnect, ral_DijkEdge));
                RAL_DIRECTIONS(dir) {
                    ral_cell_t d = ral_cell_move(c, dir);
                    if (GCin(w, d) AND (GCv(w, d) >= 1)) {
                        /* weight to go from c -> d */
                        graph[node].connections[connect].weight = 
                            RAL_DISTANCE_UNIT(dir)/2 * (GCv(w, c) + GCv(w, d));
                        graph[node].connections[connect].dest = cell2node[GRID_INDEX(d.y, d.x, w->width)];
                        connect++;
                    }
                }
            }
            node++;
        }
    }

    if (source >= 0) 
        ral_Dijkstra(graph, nodecount, source);

    FOR(c, cost) {
        if (cell2node[GRID_INDEX(c.y, c.x, cost->width)] >= 0)
            GCnv(cost, c) = graph[cell2node[GRID_INDEX(c.y, c.x, cost->width)]].distance;
        else 
            GCnv(cost, c) = -1;
    }

    goto ok;
 fail:
    ral_delete(cost);
 ok:
    if (cell2node) free(cell2node);
    if (graph) {
        for (node = 0; node < nodecount; node++) {
            if (graph[node].connections) free(graph[node].connections);
        }
        free(graph);
    }
    return cost;
}
