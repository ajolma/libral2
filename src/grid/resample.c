#include "ral2/ral.h"
#include "priv/priv.h"

ral_grid_t *ral_grid_clip(ral_grid_t *gd, ral_rcoords_t * ul, ral_rcoords_t * dr, ral_error_t **e)
{
    ral_window_t w;
    ral_grid_t *g = NULL;
    ral_wcoords_t * up_left;
    w.up_left.y = ul->y;
    w.up_left.x = ul->x;
    w.down_right.y = dr->y;
    w.down_right.x = dr->x;
    FAIL_UNLESS(g = ral_grid_new(gd->type, RAL_WINDOW_HEIGHT(w), RAL_WINDOW_WIDTH(w)));
    up_left = ral_grid_cell2point_upleft(gd, ul);
    ral_grid_set_bounds(g, gd->cell_size, up_left->x, up_left->y-gd->cell_size*gd->height);
    ral_delete(up_left);
    if (gd->type == real) {
        ral_cell_t c;
        ral_grid_set_all(g, ral_undef, e);
        for (c.y = MAX(w.up_left.y, 0); c.y <= MIN(w.down_right.y, gd->height-1); c.y++)
            for (c.x = MAX(w.up_left.x, 0); c.x <= MIN(w.down_right.x, gd->width-1); c.x++)
                Gnv(g, c.y - w.up_left.y, c.x - w.up_left.x) = GCnv(gd, c);
    } else { /* if (gd->type == integer) { */
        ral_cell_t c;
        ral_grid_set_all(g, ral_undef, e);
        for (c.y = MAX(w.up_left.y, 0); c.y <= MIN(w.down_right.y, gd->height-1); c.y++)
            for (c.x = MAX(w.up_left.x, 0); c.x <= MIN(w.down_right.x, gd->width-1); c.x++)
                Giv(g, c.y - w.up_left.y, c.x - w.up_left.x) = GCiv(gd, c);
    }
    return g;
 fail:
    return NULL;
}

ral_grid_t *ral_grid_join(ral_grid_t *g1, ral_grid_t *g2, ral_error_t **e)
{
    ral_grid_t *g = NULL;
    double ddw, ddh;
    int dw, dh, new_M, new_N;

    ASSERT(g1->cell_size == g2->cell_size, "Cannot join grids with different cell_sizes.");

    /* check the alignment */
    ddw = (g2->world.min.x - g1->world.min.x)/g1->cell_size;
    ddh = (g2->world.max.y - g1->world.max.y)/g1->cell_size;
    dw = ROUND(ddw);
    dh = ROUND(ddh);
    ASSERT(abs(ddw-(double)dw) < 0.1, "Cannot join grids with different horizontal alignment.");
    ASSERT(abs(ddh-(double)dh) < 0.1, "Cannot join grids with different vertical alignment.");

    if (dh >= 0)
        new_M = MAX(g1->height + dh, g2->height);
    else
        new_M = MAX(g1->height, g2->height - dh);

    if (dw >= 0)
        new_N = MAX(g1->width, dw + g2->width);
    else
        new_N = MAX(g1->width - dw, g2->width);
    
    FAIL_UNLESS(g = ral_grid_new(MAX(g1->type, g2->type), new_M, new_N));

    /* g1 is fixed and g2 may be aligned a bit */
    ral_grid_set_bounds(g, g1->cell_size,
                        MIN(g1->world.min.x, g1->world.min.x + dw*g1->cell_size),
                        MIN(g1->world.min.y,  g1->world.max.y - dh*g1->cell_size));
 
    if (g->type == integer) {
        ral_cell_t c;
        FOR(c, g) {
            ral_cell_t c2 = c, c1 = c;
            if (dh >= 0)
                c1.y -= dh;
            else
                c2.y += dh;
            if (dw >= 0)
                c2.x -= dw;
            else
                c1.x += dw;
            if (GCin(g1, c1) AND GCivD(g1, c1)) {
                GCiv(g, c) = GCiv(g1, c1);
            } else if (GCin(g2, c2) AND GCivD(g2, c2)) {
                GCiv(g, c) = GCiv(g2, c2);
            } else {
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
            }
        }
    } else if (g1->type == integer) {
        ral_cell_t c;
        FOR(c, g) {
            ral_cell_t c1 = c, c2 = c;
            if (dh >= 0)
                c1.y -= dh;
            else
                c2.y += dh;                
            if (dw >= 0)
                c2.x -= dw;
            else
                c1.x += dw;
            if (GCin(g1, c1) AND GCivD(g1, c1)) {
                GCnv(g, c) = GCiv(g1, c1);
            } else if (GCin(g2, c2) AND GCnvD(g2, c2)) {
                GCnv(g, c) = GCnv(g2, c2);
            } else {
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
            }
        }
    } else if (g2->type == integer) {
        ral_cell_t c;
        FOR(c, g) {
            ral_cell_t c1 = c, c2 = c;
            if (dh >= 0)
                c1.y -= dh;
            else
                c2.y += dh;                
            if (dw >= 0)
                c2.x -= dw;
            else
                c1.x += dw;
            if (GCin(g1, c1) AND GCnvD(g1, c1)) {
                GCnv(g, c) = GCnv(g1, c1);
            } else if (GCin(g2, c2) AND GCivD(g2, c2)) {
                GCnv(g, c) = GCiv(g2, c2);
            } else {
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
            }
        }
    } else {
        ral_cell_t c;
        FOR(c, g) {
            ral_cell_t c1 = c, c2 = c;
            if (dh >= 0)
                c1.y -= dh;
            else
                c2.y += dh;                
            if (dw >= 0)
                c2.x -= dw;
            else
                c1.x += dw;
            if (GCin(g1, c1) AND GCnvD(g1, c1)) {
                GCnv(g, c) = GCnv(g1, c1);
            } else if (GCin(g2, c2) AND GCnvD(g2, c2)) {
                GCnv(g, c) = GCnv(g2, c2);
            } else {
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
            }
        }
    } 

    return g;
 fail:
    ral_delete(g);
    return NULL;
}

void ral_grid_pick(ral_grid_t *dest, ral_grid_t *src)
{
    ral_cell_t c;
    switch (dest->type) {
    case real: {
        switch (src->type) {
        case real: {
            FOR(c, dest) {
                ral_rcoords_t * rd = ral_grid_point2cell(src, ral_grid_cell2point(dest, ral_rcoords_new(c.x, c.y)));
                ral_cell_t d = {rd->y, rd->x};
                ral_delete(rd);
                if (GCin(src, d) AND GCnvD(src, d)) { /* should also test src->mask */
                    GCnv(dest, c) = GCnv(src, d);
                }
            }
            break;
        }
        case integer: {
            FOR(c, dest) {
                ral_rcoords_t * rd = ral_grid_point2cell(src, ral_grid_cell2point(dest, ral_rcoords_new(c.x, c.y)));
                ral_cell_t d = {rd->y, rd->x};
                ral_delete(rd);
                if (GCin(src, d) AND GCivD(src, d)) { /* should also test src->mask */
                    GCnv(dest, c) = GCiv(src, d);
                }
            }
            break;
        }
        }
        break;
    }
    case integer: {
        switch (src->type) {
        case real: {
            FOR(c, dest) {
                ral_rcoords_t * rd = ral_grid_point2cell(src, ral_grid_cell2point(dest, ral_rcoords_new(c.x, c.y)));
                ral_cell_t d = {rd->y, rd->x};
                ral_delete(rd);
                if (GCin(src, d) AND GCnvD(src, d)) { /* should also test src->mask */
                    RAL_INTEGER i = ROUND(GCnv(src, d));
                    GCiv(dest, c) = i;
                }
            }
            break;
        }
        case integer: {
            FOR(c, dest) {
                ral_rcoords_t * rd = ral_grid_point2cell(src, ral_grid_cell2point(dest, ral_rcoords_new(c.x, c.y)));
                ral_cell_t d = {rd->y, rd->x};
                ral_delete(rd);
                if (GCin(src, d) AND GCivD(src, d)) { /* should also test src->mask */
                    GCiv(dest, c) = GCiv(src, d);
                }
            }
            break;
        }
        }
        break;
    }
    }
}

ral_grid_t *ral_grid_transform(ral_grid_t *gd, double tr[], int M, int N, int pick, int value, ral_error_t **e) {
    /* i0=ai + bi * i + ci * j                                
       j0=aj + bj * i + cj * j
       tr = (ai,bi,ci,aj,bj,cj) */
    ral_grid_t *g = NULL;
    ral_cell_t c, p;

    if ((pick > 0) AND (pick < 10))
        FAIL_UNLESS(g = ral_grid_new(real, M, N))
    else 
        FAIL_UNLESS(g = ral_grid_new(gd->type, M, N));

    /* calculate cell_size, world.min.x ...? no because new cell_size_x is not necessarily cell_size_y */

    ral_grid_set_all(g, ral_undef, e);
    if (!pick) {
        if (gd->type == integer) {
            FOR(c, g) {
                p.y = ROUND(tr[0]+tr[1]*c.y+tr[2]*c.x);
                p.x = ROUND(tr[3]+tr[4]*c.y+tr[5]*c.x);
                if (GCin(gd, p) AND GCD(gd,p)) GCiv(g, c) = GCiv(gd, p);
            }
        } else {
            FOR(c, g) {
                p.y = ROUND(tr[0]+tr[1]*c.y+tr[2]*c.x);
                p.x = ROUND(tr[3]+tr[4]*c.y+tr[5]*c.x);
                if (GCin(gd, p) AND GCD(gd,p)) GCnv(g, c) = GCnv(gd, p);
            }
        }
    } else {
        int bbox[4];
        ral_point_t src[4];
        ral_polygon_t P;
        P.nodes = src;
        P.n = 4;
        if (gd->type == integer) {
            FOR(c, g) {
                int n = 0;
                double mean = 0, variance = 0;
                int f = 1;
                ral_rect_in_src(c, tr, P, bbox);
                for (p.y = MAX(bbox[0],0); p.y < MIN(bbox[2]+1,gd->height) ; p.y++) {
                    for (p.x = MAX(bbox[1],0); p.x < MIN(bbox[3]+1,gd->width) ; p.x++) {
                        ral_point_t q;
                        q.x = p.x;
                        q.y = p.y;
                        if (GCivD(gd,p) AND ral_pnpoly(q, P)) {
                            switch (pick) {
                            case 1:{ 
                                n++;
                                mean += (GCiv(gd, p) - mean) / n;
                                break;
                            }
                            case 2:{
                                double oldmean = mean;
                                n++;
                                mean += (GCiv(gd, p) - mean) / n;
                                variance += (GCiv(gd, p) - oldmean) * (GCiv(gd, p) - mean);
                                break; 
                            }
                            case 10:{
                                if (f) {
                                    GCiv(g, c) = GCiv(gd, p);
                                    f = 0;
                                } else {
                                    GCiv(g, c) = MIN(GCiv(g, c),GCiv(gd, p));
                                }
                                break; 
                            }
                            case 11:{ 
                                if (f) {
                                    GCiv(g, c) = GCiv(gd, p);  
                                    f = 0;
                                } else {
                                    GCiv(g, c) = MAX(GCiv(g, c),GCiv(gd, p));  
                                }
                                break; 
                            }
                            case 20:{
                                if (f) {                                    
                                    GCiv(g, c) = 0;
                                    f = 0;
                                } 
                                if (GCiv(gd, p) == value) GCiv(g, c)++;
                                break; 
                            }
                            }
                        }
                    }
                }
                switch (pick) {
                case 1:{
                    if (n)
                        GCnv(g, c) = mean;
                    break;
                }
                case 2:{ 
                    if (n)
                        GCnv(g, c) = variance/(n-1);
                    break; 
                }
                }
            }
        } else {
            FOR(c, g) {
                int n = 0;
                double mean = 0, variance = 0;
                int f = 1;
                ral_rect_in_src(c, tr, P, bbox);
                for (p.y = MAX(bbox[0],0); p.y < MIN(bbox[2]+1,gd->height) ; p.y++) {
                    for (p.x = MAX(bbox[1],0); p.x < MIN(bbox[3]+1,gd->width) ; p.x++) {
                        ral_point_t q;
                        q.x = p.x;
                        q.y = p.y;
                        if (GCnvD(gd,p) AND ral_pnpoly(q, P)) {
                            switch (pick) {
                            case 1:{ 
                                n++;
                                mean += (GCnv(gd, p) - mean) / n;
                                break;
                            }
                            case 2:{
                                double oldmean = mean;
                                n++;
                                mean += (GCnv(gd, p) - mean) / n;
                                variance += (GCnv(gd, p) - oldmean) * (GCnv(gd, p) - mean);
                                break; 
                            }
                            case 10:{
                                if (f) {
                                    GCnv(g, c) = GCnv(gd, p);
                                    f = 0;
                                } else {
                                    GCnv(g, c) = MIN(GCnv(g, c),GCnv(gd, p));
                                }
                                break; 
                            }
                            case 11:{ 
                                if (f) {
                                    GCnv(g, c) = GCnv(gd, p);  
                                    f = 0;
                                } else {
                                    GCnv(g, c) = MAX(GCnv(g, c),GCnv(gd, p));  
                                }
                                break; 
                            }
                            }
                        }
                    }
                }
                switch (pick) {
                case 1:{
                    if (n)
                        GCnv(g, c) = mean;
                    break;
                }
                case 2:{ 
                    if (n)
                        GCnv(g, c) = variance/(n-1);
                    break; 
                }
                }
            }
        }
    }
    return g;
 fail:
    ral_delete(g);
    return NULL;
}
