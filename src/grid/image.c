#include "ral2/ral.h"
#include "priv/priv.h"

ral_grid_t *ral_grid_spread(ral_grid_t *grid, ral_array_t * kernel)
{
    ral_grid_t *ret = NULL;
    double *w = NULL;
    FAIL_UNLESS(ret = ral_grid_new_like(grid, real, NULL));
    int d = (ral_number_of_elements(kernel)-1)/2;
    w = CALLOC(d*d, double);
    FAIL_UNLESS(w);
    ral_integer_t * i = ral_integer_new(0);
    for (i->value = 0; i->value < d; i->value++) {
        void *r = ral_lookup(kernel, i);
        ral_integer_t * j = ral_integer_new(0);
        for (j->value = 0; j->value < d; j->value++) {
            ral_real_t * k = ral_lookup(r, j);
            w[i->value*d+j->value] = k->value;
        }
        ral_delete(j);
    }
    ral_delete(i);
    ral_cell_t cell;
    if (grid->type == integer) {
        FOR(cell, grid) {
            if (GCivD(grid, cell)) {
                ral_cell_t c;
                int x = GCiv(grid, cell);
                int i = 0;
                for (c.y = cell.y - d; c.y <= cell.y + d; c.y++)
                    for (c.x = cell.x - d; c.x <= cell.x + d; c.x++) {
                        if (GCin(ret, c))
                            GCnv(ret, c) += w[i] * (double)x;
                        i++;
                    }
            }
        }
    } else {
        FOR(cell, grid) {
            if (GCnvD(grid, cell)) {
                int i = 0;
                double x = GCnv(grid, cell);
                ral_cell_t c;
                for (c.y = cell.y - d; c.y <= cell.y + d; c.y++)
                    for (c.x = cell.x - d; c.x <= cell.x + d; c.x++) {
                        if (GCin(ret, c))
                            GCnv(ret, c) += w[i] * x;
                        i++;
                    }
            }
        }
    }
    if (w) free(w);
    return ret;
fail:
    if (w) free(w);
    ral_delete(ret);
    return NULL;
}

ral_grid_t *ral_grid_spread_random(ral_grid_t *grid, ral_array_t * kernel, ral_error_t **e)
{
    ral_grid_t *ret = NULL;
    double *w = NULL;
    int d = (ral_number_of_elements(kernel)-1)/2;
    ASSERT(grid->type == integer, BAD);
    FAIL_UNLESS(ret = ral_grid_new_like(grid, integer, e));
    w = CALLOC(d*d, double);
    FAIL_UNLESS(w);
    ral_integer_t * i = ral_integer_new(0);
    for (i->value = 0; i->value < d; i->value++) {
        ral_array_t * r = ral_lookup(kernel, i);
        ral_integer_t * j = ral_integer_new(0);
        for (j->value = 0; j->value < d; j->value++) {
            ral_real_t * k = ral_lookup(r, j);
            w[i->value*d+j->value] = k->value;
        }
    }
    ral_cell_t cell;
    FOR(cell, grid) {
        if (GCivD(grid, cell)) {
            int x = GCiv(grid, cell);
            int j;
            for (j = 0; j < x; j++) {
                int i = 0;
                ral_cell_t c;
                double p = rand()/((double)RAND_MAX + 1);
                double x = w[0];
                for (c.y = cell.y - d; c.y <= cell.y + d; c.y++) {
                    for (c.x = cell.x - d; c.x <= cell.x + d; c.x++) {
                        if (p < x) {
                            if (GCin(ret, c))
                                GCiv(ret, c)++;
                            i = -1;
                            break;
                        }
                        x += w[++i];
                    }
                    if (i < 0)
                        break;
                }
            }
        }
    }
    if (w) free(w);
    return ret;
fail:
    if (w) free(w);
    ral_delete(ret);
    return NULL;
}

ral_grid_t *ral_grid_borders(ral_grid_t *gd, ral_error_t **e)
{
    ral_grid_t *borders;
    ral_cell_t c;
    ASSERT(gd->type == integer, BAD);
    FAIL_UNLESS(borders = ral_grid_new_copy(gd, integer, NULL));
    for (c.y = 0; c.y < gd->height; c.y++) {
        int j0 = 0;
        int on = Giv(gd, c.y, 0);
        for (c.x = 0; c.x < gd->width; c.x++)
            if (!GCiv(gd, c)) {
                if (on) {
                    if ((c.y > 0) AND (c.y < gd->height)) {
                        int jc;
                        for (jc = j0+1; jc < c.x-1; jc++) {
                            if (Giv(gd,c.y-1,jc) AND Giv(gd,c.y+1,jc)) 
                                Giv(borders,c.y,jc) = 0;
                        }
                    }
                    on = 0;
                }
            } else if (!on) {
                on = 1;
                j0 = c.x;
            }
        if (on) {
            if ((c.y > 0) AND (c.y < gd->height)) {
                int jc;
                for (jc = j0+1; jc < c.x-1; jc++) {
                    if (Giv(gd,c.y-1,jc) AND Giv(gd,c.y+1,jc)) 
                        Giv(borders,c.y,jc) = 0;
                }
            }
        }
    }
    return borders;
 fail:
    return NULL;
}

void ral_grid_mark_borders(ral_grid_t *gd, ral_rcoords_t * rc, ral_grid_t *visited, ral_grid_t *borders)
{
    ral_cell_t c = {rc->y, rc->x};
    
    int paint = GCiv(gd, c);
    int d;
    GCiv(visited, c) = 1;
    for (d = 0; d < 9; d++) {
        ral_cell_t t = ral_cell_move(c, d);
        if (GCin(gd, t)) {
            if (GCiv(gd, t) != paint)
                GCiv(borders, c) = paint;
            else if (!GCiv(visited, t))
                ral_grid_mark_borders(gd, ral_rcoords_new(t.x, t.y), visited, borders);
        } else 
            GCiv(borders, c) = paint;
    }
}

ral_grid_t *ral_grid_borders_recursive(ral_grid_t *gd, ral_error_t **e)
{
    ral_grid_t *borders, *visited;
    ral_cell_t c;
    ASSERT(gd->type == integer, BAD);
    if (!(borders = ral_grid_new_like(gd, integer, e))) return NULL;
    if (!(visited = ral_grid_new_like(gd, integer, e))) {
        ral_delete(borders);
        return NULL;
    }
    FOR(c, gd)
        if (GCiv(gd, c) AND !GCiv(visited, c))
            ral_grid_mark_borders(gd, ral_rcoords_new(c.x, c.y), visited, borders);
    ral_delete(visited);
    return borders;
fail:
    return NULL;
}
