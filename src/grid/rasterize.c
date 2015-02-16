#include "ral2/ral.h"
#include "priv/priv.h"

/** Drawing methods */

void ral_grid_line(ral_grid_t *gd, ral_rcoords_t * rc1, ral_rcoords_t * rc2, void *pen)
{
    ral_cell_t c1 = {rc1->y, rc1->x};
    ral_cell_t c2 = {rc2->y, rc2->x};
    if (gd->type == integer) {
        RAL_INTEGER p = ral_as_int(pen);
        LINE(gd, c1, c2, p, INTEGER_GRID_SET_CELL);
    } else {
        RAL_REAL p = ral_as_real(pen);
        LINE(gd, c1, c2, p, REAL_GRID_SET_CELL);
    }
}

void ral_grid_filled_rect(ral_grid_t *gd, ral_rcoords_t * rc1, ral_rcoords_t * rc2, void *pen)
{
    ral_cell_t c1 = {rc1->y, rc1->x};
    ral_cell_t c2 = {rc2->y, rc2->x};
    ral_cell_t c;
    if (c2.y < c1.y) swap(c1.y, c2.y, c.y);
    if (c2.x < c1.x) swap(c1.x, c2.x, c.x);
    if (gd->type == integer) {
        RAL_INTEGER p = ral_as_int(pen);
        for (c.y = MAX(0,c1.y); c.y < MIN(gd->height, c2.y+1); c.y++) {
            for (c.x = MAX(0,c1.x); c.x < MIN(gd->width, c2.x+1); c.x++) {
                GCiv(gd, c) = p;
            }
        }
    } else {
        RAL_REAL p = ral_as_real(pen);
        for (c.y = MAX(0,c1.y); c.y < MIN(gd->height, c2.y+1); c.y++) {
            for (c.x = MAX(0,c1.x); c.x < MIN(gd->width, c2.x+1); c.x++) {
                GCnv(gd, c) = p;
            }
        }
    }
}

int ral_grid_filled_polygon(ral_grid_t *gd, ral_geometry_t * g, void *pen)
{
    ral_active_edge_table_t *aet_list = NULL;
    ral_cell_t c;
    double y = gd->world.min.y + 0.5*gd->cell_size;
    FAIL_UNLESS(aet_list = ral_get_active_edge_table_array(g->parts, g->n_parts));
    if (gd->type == integer) {
        RAL_INTEGER p = ral_as_int(pen);
        for (c.y = gd->height - 1; c.y >= 0; c.y--) {
            double *x;
            int n;            
            ral_scanline_at(aet_list, g->n_parts, y, &x, &n);
            if (x) {
                int draw = 0;
                int begin = 0;
                int k;
                while ((begin < n) AND (x[begin] < gd->world.min.x)) {
                    begin++;
                    draw = !draw;
                }
                c.x = 0;
                for (k = begin; k < n; k++) {
                    int jmax = floor((x[k] - gd->world.min.x)/gd->cell_size+0.5);
                    while ((c.x < gd->width) AND (c.x < jmax)) {
                        if (draw) GCiv(gd, c) = p;
                        c.x++;
                    }
                    if (c.x == gd->width) break;
                    draw = !draw;
                }
                ral_delete_scanline(&x);
            }
            y += gd->cell_size;
        }
    } else {
        RAL_REAL p = ral_as_real(pen);
        for (c.y = gd->height - 1; c.y >= 0; c.y--) {
            double *x;
            int n;
            ral_scanline_at(aet_list, g->n_parts, y, &x, &n);
            if (x) {
                int draw = 0;
                int begin = 0;
                int k;
                while ((begin < n) AND (x[begin] < gd->world.min.x)) {
                    begin++;
                    draw = !draw;
                }
                c.x = 0;
                for (k = begin; k < n; k++) {
                    int jmax = floor((x[k] - gd->world.min.x)/gd->cell_size+0.5);
                    while ((c.x < gd->width) AND (c.x < jmax)) {
                        if (draw) GCnv(gd, c) = p;
                        c.x++;
                    }
                    if (c.x == gd->width) break;
                    draw = !draw;
                }
                ral_delete_scanline(&x);
            }
            y += gd->cell_size;
        }
    }
    ral_active_edge_table_array_destroy(&aet_list, g->n_parts);
    return 1;
 fail:
    ral_active_edge_table_array_destroy(&aet_list, g->n_parts);
    return 0;
}

void ral_grid_filled_circle(ral_grid_t *gd, ral_rcoords_t * rc, int r, void *pen)
{
    ral_cell_t c = {rc->y, rc->x};
    if (gd->type == integer) {
        FILLED_CIRCLE(gd, c, r, ral_as_int(pen), INTEGER_GRID_SET_CELL);
    } else {
        FILLED_CIRCLE(gd, c, r, ral_as_real(pen), REAL_GRID_SET_CELL);
    }
}

int ral_grid_rasterize_feature(ral_grid_t *gd, OGRFeatureH f, int value_field, OGRFieldType ft)
{
    ral_geometry_t * g = ral_new(ralGeometry);
    RAL_INTEGER i_value = 1;
    RAL_REAL d_value = 1;
    void *value = NULL;

    FAIL_UNLESS(g && ral_geometry_set_from_ogr_geometry(g, OGR_F_GetGeometryRef(f)));

    if (value_field > -1) {
        if (ft == OFTInteger) {
            i_value = OGR_F_GetFieldAsInteger(f, value_field);
            value = ral_integer_new((long)i_value);
        } else {
            d_value = OGR_F_GetFieldAsDouble(f, value_field);
            value = ral_real_new(d_value);
        }
    }
    if (!value) value = ral_integer_new(1);

    if (g->type == wkbPoint OR g->type == wkbMultiPoint OR 
        g->type == wkbPoint25D OR g->type == wkbMultiPoint25D)
    {
        int i;
        for (i = 0; i < g->n_parts; i++) {
            int j;
            for (j = 0; j < g->parts[i].n; j++) {
                ral_point_t p = g->parts[i].nodes[j];
                if (POINT_IN_RECTANGLE(p, gd->world)) {
                    ral_rcoords_t * rc = ral_grid_point2cell(gd, ral_wcoords_new(p.x, p.y));
                    ral_cell_t c = {rc->y, rc->x};
                    ral_delete(rc);
                    if (ft == OFTInteger)
                        GCiv(gd, c) = i_value;
                    else
                        GCnv(gd, c) = d_value;
                }
            }
        }
    } else if (g->type == wkbLineString OR g->type == wkbMultiLineString OR 
               g->type == wkbLineString25D OR g->type == wkbMultiLineString25D)
    {
        int i;
        for (i = 0; i < g->n_parts; i++) {
            int j;
            for (j = 0; j < g->parts[i].n - 1; j++) {
                /* draw line from g->parts[i].nodes[j] to g->parts[i].nodes[j+1] */
                /* clip */
                ral_line_t l;
                l.begin = g->parts[i].nodes[j];
                l.end = g->parts[i].nodes[j+1];
                if (clip_line_to_rect(&l,gd->world)) {
                    ral_rcoords_t * rc1 = ral_grid_point2cell(gd, ral_wcoords_new(l.begin.x, l.begin.y));
                    ral_rcoords_t * rc2 = ral_grid_point2cell(gd, ral_wcoords_new(l.end.x, l.end.y));
                    ral_grid_line(gd, rc1, rc2, value);
                }
            }
        }
    } else if (g->type == wkbPolygon OR g->type == wkbMultiPolygon OR 
               g->type == wkbPolygon25D OR g->type == wkbMultiPolygon25D)
    {
        ral_grid_filled_polygon(gd, g, value);
    }

    ral_delete(g);
    return 1;
fail:
    ral_delete(g);
    return 0;
}

int ral_grid_rasterize_layer(ral_grid_t *gd, OGRLayerH l, int value_field)
{
    OGRFieldType ft = OFTInteger;
    OGRFeatureH f;

    if (value_field > -1)
        ft = OGR_Fld_GetType(OGR_FD_GetFieldDefn(OGR_L_GetLayerDefn(l), value_field));

    OGR_L_SetSpatialFilterRect(l, 
                               gd->world.min.x, gd->world.min.y, 
                               gd->world.max.x, gd->world.max.y);

    OGR_L_ResetReading(l);
    while ((f = OGR_L_GetNextFeature(l))) {
        FAIL_UNLESS(ral_grid_rasterize_feature(gd, f, value_field, ft));
        OGR_F_Destroy(f);
    }

    return 1;
fail:
    return 0;
}

void ral_array_add_cell(ral_grid_t *gd, ral_cell_t c, ral_array_t * a)
{
}

ral_array_t * ral_grid_get_line(ral_grid_t *gd, ral_rcoords_t * rc1, ral_rcoords_t * rc2)
{
    ral_cell_t c1 = {rc1->y, rc1->x};
    ral_cell_t c2 = {rc2->y, rc2->x};
    ral_array_t * a = NULL;
    LINE(gd, c1, c2, a, ral_array_add_cell);
    return a;
}

ral_array_t * ral_grid_get_rect(ral_grid_t *gd, ral_rcoords_t * rc1, ral_rcoords_t * rc2)
{
    ral_cell_t c1 = {rc1->y, rc1->x};
    ral_cell_t c2 = {rc2->y, rc2->x};
    ral_cell_t c;
    ral_array_t * a = NULL;
    if (c2.y < c1.y) swap(c1.y, c2.y, c.y);
    if (c2.x < c1.x) swap(c1.x, c2.x, c.x);
    for (c.y = MAX(0,c1.y); c.y < MIN(gd->height, c2.y+1); c.y++)
        for (c.x = MAX(0,c1.x); c.x < MIN(gd->width, c2.x+1); c.x++)
            ral_array_add_cell(gd, c, a);
    return a;
}

ral_array_t * ral_grid_get_circle(ral_grid_t *gd, ral_rcoords_t * rc, int r)
{
    ral_cell_t c = {rc->y, rc->x};
    ral_array_t * array = NULL;
    FILLED_CIRCLE(gd, c, r, array, ral_array_add_cell);
    return array;
}

/* after gdImageFill in gd.c of http://www.boutell.com/gd/ */
void ral_grid_floodfill(ral_grid_t *gd, ral_grid_t *done, ral_rcoords_t * rc, void *pen, int connectivity)
{
    int lastBorder;
    void *old;
    int leftLimit, rightLimit;
    ral_rcoords_t * rc2 = ral_new(ralRCoords);
    ral_integer_t * one = ral_integer_new(0);
    one->value = 1;
    
    old = ral_grid_get(gd, rc);
    if (ral_cmp(old, pen) == 0) {
        /* Nothing to be done */
        if (done) ral_grid_set(done, rc, one, NULL);
        goto done;
    }
    /* Seek up */
    leftLimit = (-1);
    rc2->x = rc->x;
    for (rc2->y = rc->y; (rc2->y >= 0); rc2->y--) {
        if (ral_grid_get(gd, rc2) != old) {
            break;
        }
        ral_grid_set(gd, rc2, pen, NULL);
        if (done) ral_grid_set(done, rc2, one, NULL);
        if (connectivity == 8)
            leftLimit = MAX(0, rc2->y-1);
        else
            leftLimit = rc2->y;
    }
    if (leftLimit == (-1)) {
        goto done;
    }
    /* Seek down */
    if (connectivity == 8)
        rightLimit = MIN(gd->height-1, rc->y+1);
    else
        rightLimit = rc->y;
    for (rc2->y = (rc->y+1); (rc2->y < gd->height); rc2->y++) {        
        if (ral_cmp(ral_grid_get(gd, rc2), old) != 0) {
            break;
        }
        ral_grid_set(gd, rc2, pen, NULL);
        if (done) ral_grid_set(done, rc2, one, NULL);
        if (connectivity == 8)
            rightLimit = MIN(gd->height-1, rc2->y+1);
        else
            rightLimit = rc2->y;
    }
    /* Look at columns right and left and start paints */
    /* right */
    if (rc->x > 0) {
        rc2->x = rc->x-1;
        lastBorder = 1;
        for (rc2->y = leftLimit; (rc2->y <= rightLimit); rc2->y++) {
            void *a = ral_grid_get(gd, rc2);
            if (lastBorder) {
                if (ral_cmp(a, old) == 0) {        
                    ral_grid_floodfill(gd, done, rc2, pen, connectivity);
                    lastBorder = 0;
                }
            } else if (ral_cmp(a, old) != 0) {
                lastBorder = 1;
            }
            ral_delete(a);
        }
    }
    /* left */
    if (rc->x < ((gd->width) - 1)) {
        rc2->x = rc->x+1;
        lastBorder = 1;
        for (rc2->y = leftLimit; (rc2->y <= rightLimit); rc2->y++) {
            void *a = ral_grid_get(gd, rc2);
            if (lastBorder) {
                if (ral_cmp(a, old) == 0) {
                    ral_grid_floodfill(gd, done, rc2, pen, connectivity);
                    lastBorder = 0;
                }
            } else if (ral_cmp(a, old) != 0) {
                lastBorder = 1;
            }
            ral_delete(a);
        }
    }
done:
    ral_delete(rc2);
    ral_delete(one);
    ral_delete(old);
}

