#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *Grid_new(ral_object_t *_self)
{
    ral_grid_t *self = _self;
    self->type = integer;
    self->width = 0;
    self->height = 0;
    self->cell_size = 1;
    self->world.min.x = 0;
    self->world.min.y = 0;
    self->world.max.x = 0;
    self->world.max.y = 0;
    self->has_no_data_value = 0;
    self->integer_no_data_value = -999;
    self->real_no_data_value = -999.0;
    self->data = NULL;
    return self;
}

void Grid_delete(ral_object_t *_self)
{
    ral_grid_t *self = _self;
    free(self->data);
}

const char *Grid_cname()
{
    return "Grid";
}

static const struct Class _Grid = {
    sizeof(struct ral_grid),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Grid_new,
    /*  2 */ NULL,
    /*  3 */ Grid_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Grid_cname,
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

ral_class ralGrid = &_Grid;

/** Construct a new grid object. The cell_size, world, and
 * no_data_value are left to default values.
 */
ral_grid_t *ral_grid_new(ral_grid_type_t type, int width, int height)
{
    ral_grid_t *gd = ral_new(ralGrid);
    size_t n = height*width ;
    TEST(gd);
    gd->type = type;
    gd->width = width;
    gd->height = height;
    gd->cell_size = 1;
    gd->world.max.x = width;
    gd->world.max.y = height;
    if (gd->type == integer)
        gd->data = CALLOC(n, RAL_INTEGER);
    else
        gd->data = CALLOC(n, RAL_REAL);
    TEST(gd->data);
    return gd;
 fail:
    return NULL;
}

/** Construct a new grid object that is like another grid object.
*/
ral_grid_t *ral_grid_new_like(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e)
{
    ral_grid_t *g;
    FAIL_UNLESS(g = ral_grid_new(type, gd->width, gd->height));
    g->cell_size = gd->cell_size;
    g->world = gd->world;
    if (gd->has_no_data_value) ral_grid_set_no_data_value(g, ral_grid_get_no_data_value(gd), e);
    if (e && *e) (*e)->level = 1; /* it is not worth failing if the no_data was not set */
    return g;
fail:
    return NULL;
}

/** Construct a new grid object that is a clone of another grid object.
*/
ral_grid_t *ral_grid_new_copy(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e)
{
    ral_grid_t *g;
    ral_cell_t c;
    FAIL_UNLESS(g = ral_grid_new_like(gd, type, e));
    if (gd->type == integer) {
        if (g->type == integer) {
            FOR(c, gd)
                GCiv(g, c) = GCiv(gd, c);
        } else {
            FOR(c, gd)
                GCnv(g, c) = GCiv(gd, c);
        }
    } else {
        if (g->type == integer) {
            FOR(c, gd)
                ROUND_TEST(GCnv(gd, c), GCiv(g, c), e);
        } else {
            FOR(c, gd)
                GCnv(g, c) = GCnv(gd, c);
        }
    }
    return g;
 fail:
    return NULL;
}

void ral_grid_delete(ral_grid_t *gd)
{
    ral_delete(gd);
}

void *ral_grid_data(ral_grid_t *gd)
{
    return gd->data;
}

char *ral_grid_datatype_name(ral_grid_t *gd)
{
    if (gd->type == integer)
        return RAL_INTEGER_TYPE_NAME;
    else
        return RAL_REAL_TYPE_NAME;
}

int ral_grid_get_height(ral_grid_t *gd) 
{
    return gd->height;
}

int ral_grid_get_width(ral_grid_t *gd) 
{
    return gd->width;
}

ral_grid_type_t ral_grid_get_type(ral_grid_t *gd) 
{
    return gd->type;
}

double ral_grid_get_cell_size(ral_grid_t *gd)
{  
    return gd->cell_size;
}

ral_wbox_t *ral_grid_get_world(ral_grid_t *gd)
{
    return ral_wbox_new(gd->world.min.x, gd->world.min.y, gd->world.max.x, gd->world.max.y);
}

void *ral_grid_get_no_data_value(ral_grid_t *gd)
{
    if (!gd->has_no_data_value)
        return NULL;
    if (gd->type == integer) {
        return ral_integer_new((long)gd->integer_no_data_value);
    } else {
        return ral_real_new(gd->real_no_data_value);
    }
}

int ral_grid_set_no_data_value(ral_grid_t *gd, void *value, ral_error_t **e)
{
    if (!value || value == ral_undef)
        gd->has_no_data_value = 0;
    if (gd->type == integer) {
        long nd = ral_as_int(value);
        if (nd < RAL_INTEGER_MIN || nd > RAL_INTEGER_MAX)
            ERROR("Integer out of bounds when setting no_data value for an integer grid.");
        else {
            gd->integer_no_data_value = nd;
            gd->has_no_data_value = 1;
        }
    } else {
        gd->real_no_data_value = ral_as_real(value);
        gd->has_no_data_value = 1;
    }
    return 1;
fail:
    return 0;
}

void ral_grid_flip_horizontal(ral_grid_t *gd)
{
    if (gd->type == integer) {
        ral_cell_t c;
        RAL_INTEGER x;
        for (c.y = 0; c.y < gd->height; c.y++) 
            for (c.x = 0; c.x < gd->width/2; c.x++) {
                ral_cell_t d = c;
                d.x = gd->width - 1 - c.x;
                x = GCiv(gd, d);
                GCiv(gd, d) = GCiv(gd, c);
                GCiv(gd, c) = x;
            }
    } else {
        ral_cell_t c;
        RAL_REAL x;
        for (c.y = 0; c.y < gd->height; c.y++) 
            for (c.x = 0; c.x < gd->width/2; c.x++) {
                ral_cell_t d = c;
                d.x = gd->width - 1 - c.x;
                x = GCnv(gd, d);
                GCnv(gd, d) = GCnv(gd, c);
                GCnv(gd, c) = x;
            } 
    }
}

void ral_grid_flip_vertical(ral_grid_t *gd)
{
    if (gd->type == integer) {
        ral_cell_t c;
        RAL_INTEGER x;
        for (c.y = 0; c.y < gd->height/2; c.y++) 
            for (c.x = 0; c.x < gd->width; c.x++) {
                ral_cell_t d = c;
                d.y = gd->height - 1 - c.y;
                x = GCiv(gd, d);
                GCiv(gd, d) = GCiv(gd, c);
                GCiv(gd, c) = x;
            }
    } else {
        ral_cell_t c;
        RAL_REAL x;
        for (c.y = 0; c.y < gd->height/2; c.y++) 
            for (c.x = 0; c.x < gd->width; c.x++) {
                ral_cell_t d = c;
                d.y = gd->height - 1 - c.y;
                x = GCnv(gd, d);
                GCnv(gd, d) = GCnv(gd, c);
                GCnv(gd, c) = x;
            } 
    } 
} 

int ral_grid_coerce(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e)
{
    if (gd->type == real AND type == integer) {
        ral_grid_t *g = ral_grid_new_copy(gd, integer, e);
        FAIL_UNLESS(g);
        swap_grids(gd, g);
    } else if (gd->type == integer AND type == real) {
        ral_grid_t *g = ral_grid_new_copy(gd, real, e);
        FAIL_UNLESS(g);
        swap_grids(gd, g);
    }
    return 1;
 fail:
    return 0;
}

int ral_grid_overlayable(ral_grid_t *g1, ral_grid_t *g2)
{
    return (g1->height == g2->height AND g1->width == g2->width AND 
            fabs(g1->cell_size - g2->cell_size) < EPSILON AND
            fabs(g1->world.min.x - g2->world.min.x) < EPSILON AND
            fabs(g1->world.min.y - g2->world.min.y) < EPSILON);
}

void ral_grid_set_bounds(ral_grid_t *gd, double cell_size, double minX, double minY)
{
    gd->cell_size = cell_size;
    gd->world.min.x = minX;
    gd->world.min.y = minY;
    gd->world.max.x = minX+gd->width*cell_size;
    gd->world.max.y = minY+gd->height*cell_size;
}

void ral_grid_copy_bounds(ral_grid_t *from,ral_grid_t *to)
{
    to->cell_size = from->cell_size;
    to->world = from->world;
}

ral_rcoords_t * ral_grid_point2cell(ral_grid_t *gd, ral_wcoords_t * p)
{
    ral_rcoords_t * c = ral_new(ralRCoords);
    p->x -= gd->world.min.x;
    p->x /= gd->cell_size;
    p->y = gd->world.max.y - p->y;
    p->y /= gd->cell_size;
    c->x = floor(p->x);
    c->y = floor(p->y);
    return c;
}

ral_wcoords_t * ral_grid_cell2point(ral_grid_t *gd, ral_rcoords_t * c)
{
    ral_wcoords_t * p = ral_new(ralWCoords);
    p->x = gd->world.min.x + (RAL_REAL)c->x*gd->cell_size + gd->cell_size/2;
    p->y = gd->world.min.y + (RAL_REAL)(gd->height-c->y)*gd->cell_size - gd->cell_size/2;
    return p;
}

ral_wcoords_t * ral_grid_cell2point_upleft(ral_grid_t *gd, ral_rcoords_t * c)
{
    ral_wcoords_t * p = ral_new(ralWCoords);
    p->x = gd->world.min.x + (double)c->x*gd->cell_size;
    p->y = gd->world.min.y + (double)(gd->height-c->y)*gd->cell_size;
    return p;
}

void *ral_grid_get(ral_grid_t *gd, ral_rcoords_t * rc)
{
    ral_cell_t c = {rc->y, rc->x};
    FAIL_UNLESS(GCin(gd, c));
    if (gd->type == integer) {
        long i = (long)GCiv(gd, c);
        if (gd->has_no_data_value && i == (long)gd->integer_no_data_value)
            return ral_undef;
        else
            return ral_integer_new(i);
    } else {
        double x = GCnv(gd, c);
        if (gd->has_no_data_value && x == (double)gd->real_no_data_value)
            return ral_undef;
        else
            return ral_real_new(x);
    }
fail:
    return NULL;
}

void ral_grid_set(ral_grid_t *gd, ral_rcoords_t * rc, void *value, ral_error_t **e)
{
    ral_cell_t c = {rc->y, rc->x};
    if (!value) return;
    if (!GCin(gd, c)) {
        if (e)
            *e = ral_error_new(0, "%i, %i: cell out of bounds when setting data value to a grid.", c.y, c.x);
        goto fail;
    }
    if (value != ral_undef) {
        if (gd->type == integer)
            GCiv(gd, c) = ral_as_int(value);
        else
            GCnv(gd, c) = ral_as_real(value);
    } else {
        if (!gd->has_no_data_value) {
            if (e)
                *e = ral_error_new(1, "Warning: Setting no data value to -999.");
            gd->has_no_data_value = 1;
            if (gd->type == integer)
                gd->integer_no_data_value = -999;
            else
                gd->real_no_data_value = -999.0;
        }
        if (gd->type == integer)
            GCiv(gd, c) = gd->integer_no_data_value;
        else
            GCnv(gd, c) = gd->real_no_data_value;
    }
fail:
    return;
}

int ral_grid_is_defined(ral_grid_t *gd)
{
    ral_cell_t c;
    if (gd->type == integer) {
        FOR(c, gd) {
            if (GCivD(gd, c)) 
                GCiv(gd, c) = 1;
            else
                GCiv(gd, c) = 0;
        }
    } else {
        ral_grid_t *g = ral_grid_new_like(gd, integer, NULL);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c)) 
                GCiv(g, c) = 1;
            else
                GCiv(g, c) = 0;
        }
        swap_grids(gd, g);
    }
    ral_grid_set_no_data_value(gd, NULL, NULL);
    return 1;
 fail:
    return 0;
}

int ral_grid_not(ral_grid_t *gd)
{
    ral_cell_t c;
    ASSERT(gd->type == integer, BAD);
    FOR(c, gd)
        if (GCivD(gd, c)) 
            GCiv(gd, c) = !GCiv(gd, c);
    return 1;
 fail:
    return 0;
}

int ral_grid_and_grid(ral_grid_t *gd1, ral_grid_t *gd2)
{
    ral_cell_t c;
    ASSERT(gd1->height == gd2->height AND gd1->width == gd2->width, BAD);
    ASSERT(gd1->type == integer AND gd2->type == integer, BAD);
    FOR(c, gd1) {
        if (GCivD(gd1, c)) {
            if (GCiv(gd1, c)) {
                if (!GCivD(gd2, c))
                    ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
                else
                    GCiv(gd1, c) = 1 AND GCiv(gd2, c);
            } else
                GCiv(gd1, c) = 0;
        } else if (GCivD(gd2, c) AND !GCiv(gd2, c))
            GCiv(gd1, c) = 0;
        else
            ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
    }
    return 1;
fail:
    return 0;
}

int ral_grid_or_grid(ral_grid_t *gd1, ral_grid_t *gd2) 
{  
    ral_cell_t c;
    ASSERT(gd1->height == gd2->height AND gd1->width == gd2->width, BAD);
    ASSERT(gd1->type == integer AND gd2->type == integer, BAD);
    FOR(c, gd1) {
        if (GCivD(gd1, c)) {
            if (GCiv(gd1, c))
                GCiv(gd1, c) = 1;
            else {
                if (!GCivD(gd2, c))
                    ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
                else 
                    GCiv(gd1, c) = 1 AND GCiv(gd2, c);
            }
        } else if (GCivD(gd2, c) AND GCiv(gd2, c))
            GCiv(gd1, c) = 1;
        else
            ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
    }
    return 1;
 fail:
    return 0;
}

void ral_grid_add(ral_grid_t *gd, void *x)
{
    ral_cell_t c;
    if (gd->type == integer) {
        RAL_INTEGER i = ral_as_int(x);
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) += i;
    
    } else {
        RAL_REAL r = ral_as_real(x);
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) +=  r;
    }
}

int ral_grid_add_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) += GCnv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) += GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c)) {
                ROUND_TEST(GCnv(gd2, c)+GCiv(gd1, c), GCiv(gd1, c), e);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) += GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_sub_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) -= GCnv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) -= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c)) {
                long a = (long)GCiv(gd1, c) - ROUND(GCnv(gd2, c));
                ASSERT(a > RAL_INTEGER_MIN AND a < RAL_INTEGER_MAX, IOB);
                GCiv(gd1, c) = a;
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) -= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

void ral_grid_mult(ral_grid_t *gd, void *x)
{
    ral_cell_t c;
    if (gd->type == integer) {
        RAL_INTEGER i = ral_as_int(x);
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) *= i;
    
    } else {
        RAL_REAL r = ral_as_real(x);
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) *= r;
    }
}

int ral_grid_mult_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) *= GCnv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) *= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c)) {
                ROUND_TEST(GCnv(gd2, c), GCiv(gd1, c), e);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) *= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

void ral_grid_div(ral_grid_t *gd, void *x, int reversed, ral_error_t **e) 
{
    ral_cell_t c;
    if (!reversed) {
        if (gd->type == integer) {
            RAL_INTEGER i = ral_as_int(x);
            FOR(c, gd)
                if (GCivD(gd, c))
                    GCiv(gd, c) /= i;
        } else {
            RAL_REAL r = ral_as_real(x);
            FOR(c, gd)
                if (GCnvD(gd, c))
                    GCnv(gd, c) /= r;
        }
    } else {
        double r = ral_as_real(x);
        if (gd->type == integer) {
            FOR(c, gd) {
                if (GCivD(gd, c)) {
                    ASSERT(GCiv(gd, c) != 0, DBZ);
                    ROUND_TEST(r/(double)GCiv(gd, c), GCiv(gd, c), e);
                }
            }
        } else {
            FOR(c, gd) {
                ASSERT(GCnv(gd, c) != 0, DBZ);
                if (GCnvD(gd, c))
                    GCnv(gd, c) = r / GCnv(gd, c);
            }
        }
    }
fail:
    return;
}

int ral_grid_div_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c)) {
                ASSERT(GCnv(gd2, c) != 0, DBZ);
                GCnv(gd1, c) /= GCnv(gd2, c);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c)) {
                ASSERT(GCiv(gd2, c) != 0, DBZ);
                GCnv(gd1, c) /= GCiv(gd2, c);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c)) {
                ASSERT(GCnv(gd2, c) != 0, DBZ);
                ROUND_TEST((double)GCiv(gd1, c)/GCnv(gd2, c), GCiv(gd1, c), e);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c)) {
                ASSERT(GCiv(gd2, c) != 0, DBZ);
                ROUND_TEST((double)GCiv(gd1, c)/(double)GCiv(gd2, c), GCiv(gd1, c), e);
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

void ral_grid_modulus(ral_grid_t *gd, int x, int reversed) 
{
    ral_cell_t c;
    if (!reversed) {
        ASSERT(x != 0, DBZ);
        ASSERT(gd->type == integer, BAD);
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) %= x;
    } else {
        ASSERT(gd->type == integer, BAD);
        FOR(c, gd) {
            if (GCivD(gd, c)) {
                ASSERT(GCiv(gd, c) != 0, DBZ);
                GCiv(gd, c) = x % GCiv(gd, c);
            }
        }
    }
fail:
    return;
}

int ral_grid_modulus_grid(ral_grid_t *gd1, ral_grid_t *gd2) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2) &&
           gd1->type == integer &&
           gd2->type == integer, BAD);
    FOR(c, gd1) {
        if (GCivD(gd1, c) AND GCivD(gd2, c)) {
            ASSERT(GCiv(gd2, c) != 0, DBZ);
            GCiv(gd1, c) %= GCiv(gd2, c);
        } else
            ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
    }
    return 1;
 fail:
    return 0;
}

void ral_grid_power(ral_grid_t *gd, double x, int reversed) 
{
    ral_cell_t c;
    if (!reversed) {
        if (gd->type == integer) {
            FOR(c, gd)
                if (GCivD(gd, c))
                    GCiv(gd, c) = ROUND(pow((double)GCiv(gd, c), x));
        } else {
            FOR(c, gd)
                if (GCnvD(gd, c))
                    GCnv(gd, c) = pow(GCnv(gd, c), x);
        }
    } else {
        if (gd->type == integer) {
            FOR(c, gd) {
                if (GCivD(gd, c))
                    GCiv(gd, c) = ROUND(pow(x, (RAL_REAL)GCiv(gd, c)));
            }
        } else {
            FOR(c, gd) {
                if (GCnvD(gd, c))
                    GCnv(gd, c) = pow(x, GCnv(gd, c));
            }
        } 
    }
}

int ral_grid_power_grid(ral_grid_t *gd1, ral_grid_t *gd2) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) = pow(GCnv(gd1, c), GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) = pow(GCnv(gd1, c), (double)GCiv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c)) {
                GCiv(gd1, c) = ROUND(pow((double)GCiv(gd1, c), GCnv(gd2, c)));
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c)) {
                GCiv(gd1, c) = ROUND(pow((double)GCiv(gd1, c), (double)GCiv(gd2, c)));
            } else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

ral_grid_t *ral_grid_round(ral_grid_t *gd, ral_error_t **e)
{
    ral_cell_t c;
    ral_grid_t *g = NULL;
    FAIL_UNLESS(g = ral_grid_new_like(gd, integer, e));
    if (gd->type == integer) {
        FOR(c, gd)
            GCiv(g, c) = GCiv(gd, c);
    } else {
        FOR(c, gd)
            GCiv(g, c) = ROUND(GCnv(gd, c));
    }
    return g;
fail:
    return NULL;
}

int ral_grid_abs(ral_grid_t *gd) 
{
    ral_cell_t c;
    if (gd->type == integer) {
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = abs(GCiv(gd, c));
    } else {
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) = fabs(GCnv(gd, c));
    }
    return 1;
}

int ral_grid_acos(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = acos(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_atan(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = atan(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_atan2(ral_grid_t *gd1, ral_grid_t *gd2) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    FAIL_UNLESS(ral_grid_coerce(gd1, real, NULL));
    FAIL_UNLESS(ral_grid_coerce(gd2, real, NULL));
    FOR(c, gd1) {
        if (GCnvD(gd1, c) AND GCnvD(gd2, c))
            GCnv(gd1, c) = atan2(GCnv(gd1, c),GCnv(gd2, c));
        else
            ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
    }
    return 1;
 fail:
    return 0;
}

int ral_grid_ceil(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = ceil(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_cos(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = cos(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_cosh(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = cosh(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_exp(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = exp(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_floor(ral_grid_t *gd) 
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = floor(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_log(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = log(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_log10(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c)) {
            double x = GCnv(gd, c);
            ASSERT(x > 0, NLG);
            GCnv(gd, c) = log10(x);
        }
    return 1;
 fail:
    return 0;
}

int ral_grid_pow(ral_grid_t *gd, void *value)
{
    ral_cell_t c;
    if (class_of(value) == ralInteger && gd->type == integer) {
        int b = ((ral_integer_t *)value)->value;
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = ROUND(pow(GCiv(gd, c), b));
    } else {
        double b = ral_as_real(value);
        FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) = pow(GCnv(gd, c), b);
    }
    return 1;
fail:
    return 0;
}

int ral_grid_sin(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = sin(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_sinh(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = sinh(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_sqrt(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = sqrt(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_tan(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = tan(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_tanh(ral_grid_t *gd)
{
    ral_cell_t c;
    FAIL_UNLESS(ral_grid_coerce(gd, real, NULL));
    FOR(c, gd)
        if (GCnvD(gd, c))
            GCnv(gd, c) = tanh(GCnv(gd, c));
    return 1;
 fail:
    return 0;
}

int ral_grid_lt(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) < ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) < i;
    }
    return 1;
 fail:
    return 0;
}

int ral_grid_gt(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) > ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd) {
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) > i;
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_le(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) <= ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) <= i;

    }
    return 1;
 fail:
    return 0;
}

int ral_grid_ge(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) >= ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) >= i;

    }
    return 1;
 fail:
    return 0;
}

int ral_grid_eq(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) == ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) == i;

    }
    return 1;
 fail:
    return 0;
}

int ral_grid_ne(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) != ral_as_real(x);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd) {
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) != i;
        }

    }
    return 1;
 fail:
    return 0;
}

int ral_grid_cmp(ral_grid_t *gd, void *x, ral_error_t **e)
{
    ral_cell_t c;
    if (gd->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd) {
            if (GCnvD(gd, c))
                GCiv(g, c) = GCnv(gd, c) > ral_as_real(x) ? 1 : (GCnv(gd, c) == ral_as_real(x) ? 0 : -1);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd, g);
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd) {
            if (GCivD(gd, c))
                GCiv(gd, c) = GCiv(gd, c) > i ? 1 : (GCiv(gd, c) == i ? 0 : -1);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_lt_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) < GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) < GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) < ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) < GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_gt_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) > GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) > GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) > ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) > GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_le_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) <= GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) <= GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) <= ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) <= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_ge_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) >= GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) >= GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) >= ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) >= GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_eq_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) == GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) == GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) == ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) == GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_ne_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) != GCnv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) != GCiv(gd2, c);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) != ROUND(GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) != GCiv(gd2, c);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_cmp_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) > GCnv(gd2, c) ? 
                    1 : (GCnv(gd1, c) == GCnv(gd2, c) ? 0 : -1);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == real AND gd2->type == integer) {
        ral_grid_t *g = ral_grid_new_like(gd1, integer, e);
        FAIL_UNLESS(g);
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCiv(g, c) = GCnv(gd1, c) > GCiv(gd2, c) ? 
                    1 : (GCnv(gd1, c) == GCiv(gd2, c) ? 0 : -1);
            else
                ral_grid_set(g, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
        swap_grids(gd1, g);
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) > ROUND(GCnv(gd2, c)) ? 
                    1 : (GCiv(gd1, c) == ROUND(GCnv(gd2, c)) ? 0 : -1);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = GCiv(gd1, c) > GCiv(gd2, c) ? 
                    1 : (GCiv(gd1, c) == GCiv(gd2, c) ? 0 : -1);
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

void ral_grid_min(ral_grid_t *gd, void *x)
{
    ral_cell_t c;
    if (gd->type == real) {
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) = MIN(GCnv(gd, c), ral_as_real(x));
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = MIN(GCiv(gd, c), i);
    } 
}

void ral_grid_max(ral_grid_t *gd, void *x)
{
    ral_cell_t c;
    if (gd->type == real) {
        FOR(c, gd)
            if (GCnvD(gd, c))
                GCnv(gd, c) = MAX(GCnv(gd, c), ral_as_real(x));
    } else {
        RAL_INTEGER i = ROUND(ral_as_real(x));
        FOR(c, gd)
            if (GCivD(gd, c))
                GCiv(gd, c) = MAX(GCiv(gd, c), i);
    }
}

int ral_grid_min_grid(ral_grid_t *gd1, ral_grid_t *gd2)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) = MIN(GCnv(gd1, c),GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) = MIN(GCnv(gd1, c),GCiv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = MIN(GCiv(gd1, c),ROUND(GCnv(gd2, c)));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = MIN(GCiv(gd1, c),GCiv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

int ral_grid_max_grid(ral_grid_t *gd1, ral_grid_t *gd2)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd1, gd2), BAD);
    if (gd1->type == real AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCnvD(gd2, c))
                GCnv(gd1, c) = MAX(GCnv(gd1, c),GCnv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == real AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCnvD(gd1, c) AND GCivD(gd2, c))
                GCnv(gd1, c) = MAX(GCnv(gd1, c),GCiv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == real) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCnvD(gd2, c))
                GCiv(gd1, c) = MAX(GCiv(gd1, c),ROUND(GCnv(gd2, c)));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } else if (gd1->type == integer AND gd2->type == integer) {
        FOR(c, gd1) {
            if (GCivD(gd1, c) AND GCivD(gd2, c))
                GCiv(gd1, c) = MAX(GCiv(gd1, c),GCiv(gd2, c));
            else
                ral_grid_set(gd1, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
        }
    } 
    return 1;
 fail:
    return 0;
}

void ral_grid_random(ral_grid_t *gd)
{
    if (gd->type == integer) {
        ral_cell_t c;
        FOR(c, gd) {
            /* from i randomly to 0..y */
            double p = rand()/((double)RAND_MAX+1);
            GCiv(gd, c) = (int)(p*(GCiv(gd, c)+1));
        }
    } else {
        ral_cell_t c;
        FOR(c, gd) {
            /* from x randomly to [0..x] */
            double p = rand()/((double)RAND_MAX);
            GCnv(gd, c) = p*GCnv(gd, c);
        }
    }
}

/** Focal methods */

ral_array_t * ral_grid_get_focal(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_cell_t c = {rc->y, rc->x};
    
    ral_cell_t c2;
    ral_array_t * a = ral_new(ralArray);
    int d = (ral_number_of_elements(focus)-1)/2;
    ral_integer_t * i = ral_integer_new(0);
    ral_integer_t * j = ral_integer_new(0);
    for (c2.y = c.y - d; c2.y <= c.y + d; c2.y++) {
        ral_array_t * row = ral_new(ralArray);
        i->value = c2.y - c.y + d;
        void *r = ral_lookup(focus, i);
        for (c2.x = c.x - d; c2.x <= c.x + d; c2.x++) {
            j->value = c2.x - c.x + d;
            ral_integer_t * mask = ral_lookup(r, j);
            void *x;
            if (mask->value && GCin(gd, c2)) {
                if (gd->type == integer)
                    x = GCivD(gd, c2) ? ral_integer_new((long)GCiv(gd, c2)) : ral_undef;
                else
                    x = GCnvD(gd, c2) ? ral_real_new(GCnv(gd, c2)) : ral_undef;
            } else
                x = ral_undef;
            ral_insert(row, x, j);
        }
        ral_insert(a, row, i);
    }
    ral_delete(i);
    ral_delete(j);
    return a;
}

void *ral_grid_focal_sum(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    void *ret = ral_array_sum(a);
    ral_delete(a);
    return ret;
}

void *ral_grid_focal_mean(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    void *ret = ral_array_mean(a);
    ral_delete(a);
    return ret;
}

void *ral_grid_focal_variance(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    void *ret = ral_array_variance(a);
    ral_delete(a);
    return ret;
}

void *ral_grid_focal_count(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    void *ret = ral_array_count(a);
    ral_delete(a);
    return ret;
}

void *ral_grid_focal_count_of(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus, RAL_INTEGER value)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    ral_integer_t * i = ral_integer_new(0);
    i->value = value;
    void *ret = ral_array_count_of(a, i);
    ral_delete(i);
    ral_delete(a);
    return ret;
}

ral_interval_t *ral_grid_focal_range(ral_grid_t *gd, ral_rcoords_t * rc, ral_array_t * focus)
{
    ral_array_t * a = ral_grid_get_focal(gd, rc, focus);
    ral_interval_t *ret = ral_array_range(a);
    ral_delete(a);
    return ret;
}

ral_grid_t *ral_grid_focal_sum_grid(ral_grid_t *grid, ral_array_t * focus)
{
    ral_grid_t *ret = NULL;
    ral_cell_t c;
    FAIL_UNLESS(ret = ral_grid_new_like(grid, grid->type, NULL));
    FOR(c, ret) {
        void *f = ral_grid_focal_sum(grid, ral_rcoords_new(c.x, c.y), focus);
        ral_grid_set(ret, ral_rcoords_new(c.x, c.y), f, NULL);
        ral_delete(f);
    }
    return ret;
 fail:
    return NULL;
}

ral_grid_t *ral_grid_focal_mean_grid(ral_grid_t *grid, ral_array_t * focus)
{
    ral_grid_t *ret = NULL;
    ral_cell_t c;
    FAIL_UNLESS(ret = ral_grid_new_like(grid, real, NULL));
    FOR(c, ret) {
        void *f = ral_grid_focal_mean(grid, ral_rcoords_new(c.x, c.y), focus);
        ral_grid_set(ret, ral_rcoords_new(c.x, c.y), f, NULL);
        ral_delete(f);
    }
    return ret;
 fail:
    return NULL;
}

ral_grid_t *ral_grid_focal_variance_grid(ral_grid_t *grid, ral_array_t * focus)
{
    ral_grid_t *ret = NULL;
    ral_cell_t c;
    FAIL_UNLESS(ret = ral_grid_new_like(grid, real, NULL));
    FOR(c, ret) {
        void *f = ral_grid_focal_variance(grid, ral_rcoords_new(c.x, c.y), focus);
        ral_grid_set(ret, ral_rcoords_new(c.x, c.y), f, NULL);
        ral_delete(f);
    }
    return ret;
 fail:
    return NULL;
}

ral_grid_t *ral_grid_focal_count_grid(ral_grid_t *grid, ral_array_t * focus, ral_error_t **e)
{
    ral_grid_t *ret = NULL;
    ral_cell_t c;
    FAIL_UNLESS(ret = ral_grid_new_like(grid, integer, e));
    FOR(c, ret) {
        void *f = ral_grid_focal_count(grid, ral_rcoords_new(c.x, c.y), focus);
        ral_grid_set(ret, ral_rcoords_new(c.x, c.y), f, NULL);
        ral_delete(f);
    }
    return ret;
fail:
    return NULL;
}

ral_grid_t *ral_grid_focal_count_of_grid(ral_grid_t *grid, ral_array_t * focus, RAL_INTEGER value, ral_error_t **e)
{
    ral_grid_t *ret = NULL;
    ral_cell_t c;
    ASSERT(grid->type == integer, EIG);
    FAIL_UNLESS(ret = ral_grid_new_like(grid, integer, e));
    FOR(c, ret) {
        void *f = ral_grid_focal_count_of(grid, ral_rcoords_new(c.x, c.y), focus, value);
        ral_grid_set(ret, ral_rcoords_new(c.x, c.y), f, NULL);
        ral_delete(f);
    }
    return ret;
fail:
    return NULL;
}

/** Zonal methods */

ral_hash_t * ral_grid_zonal_count(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * counts = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        ral_integer_t * count = ral_lookup(counts, key);
        if (count)
            count->value++;
        else
            ral_insert(counts, ral_integer_new(1), key);
    }
    ral_delete(key);
    return counts;
 fail:
    ral_delete(counts);
    return NULL;
}

ral_hash_t * ral_grid_zonal_count_of(ral_grid_t *gd, ral_grid_t *zones, RAL_INTEGER value)
{
    ral_cell_t c;
    ral_hash_t * counts = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && gd->type == integer &&
           zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCivND(gd, c) OR GCiv(gd, c) != value)  continue;
        key->value = (long)GCiv(zones, c);
        ral_integer_t * count = ral_lookup(counts, key);
        if (count)
            count->value++;
        else
            ral_insert(counts, ral_integer_new(1), key);
    }
    ral_delete(key);
    return counts;
 fail:
    ral_delete(counts);
    return NULL;
}

ral_hash_t * ral_grid_zonal_sum(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * sums = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        ral_real_t *sum = ral_lookup(sums, key);
        if (sum)
            sum->value += GCv(gd, c);
        else
            ral_insert(sums, ral_real_new(GCv(gd, c)), key);
    }
    ral_delete(key);
    return sums;
fail:
    ral_delete(sums);
    return NULL;
}

ral_hash_t * ral_grid_zonal_range(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * ranges = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    if (gd->type == integer) {
        ral_integer_t * key = ral_integer_new(0);
        FOR(c, gd) {
            if (GCivND(zones, c) OR GCivND(gd, c)) continue;
            key->value = (long)GCiv(zones, c);
            ral_interval_t *range = ral_lookup(ranges, key);
            if (range) {
                ((ral_integer_t *)(range->min))->value = MIN((long)GCiv(gd, c), ((ral_integer_t *)(range->min))->value);
                ((ral_integer_t *)(range->max))->value = MAX((long)GCiv(gd, c), ((ral_integer_t *)(range->max))->value);
            } else {
                long x = (long)GCiv(gd, c);
                ral_insert(ranges, ral_interval_new(ral_integer_new(x), ral_integer_new(0), x), key);
            }
        }
        ral_delete(key);
    } else {
        ral_integer_t * key = ral_integer_new(0);
        FOR(c, gd) {
            if (GCnvND(zones, c) OR GCnvND(gd, c)) continue;
            key->value = (long)GCiv(zones, c);
            ral_interval_t *range = ral_lookup(ranges, key);
            if (range) {
                ((ral_real_t *)(range->min))->value = MIN(GCnv(gd, c), ((ral_real_t *)(range->min))->value);
                ((ral_real_t *)(range->max))->value = MAX(GCnv(gd, c), ((ral_real_t *)(range->max))->value);
            } else {
                double x = GCnv(gd, c);
                ral_insert(ranges, ral_interval_new(ral_real_new(x), ral_real_new(x), 3), key);
            }
        }
        ral_delete(key);
    }
    return ranges;
fail:
    ral_delete(ranges);
    return NULL;
}

ral_hash_t * ral_grid_zonal_min(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * mins = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        ral_real_t * min = ral_lookup(mins, key);
        if (min) {
            if (GCv(gd, c) < min->value) min->value = GCv(gd, c);
        } else
            ral_insert(mins, ral_real_new(GCv(gd, c)), key);
    }
    ral_delete(key);
    return mins;
fail:
    ral_delete(mins);
    return NULL;
}

ral_hash_t * ral_grid_zonal_max(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * maxs = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        ral_real_t * max = ral_lookup(maxs, key);
        if (max) {
            if (GCv(gd, c) > max->value) max->value = GCv(gd, c);
        } else
            ral_insert(maxs, ral_real_new(GCv(gd, c)), key);
    }
    ral_delete(key);
    return maxs;
 fail:
    ral_delete(maxs);
    return NULL;
}

/*
  mean and variance are calculated recursively, the algorithm is as in Statistics::Descriptive.pm
  division by n-1 is used for variance
*/
ral_hash_t * ral_grid_zonal_mean(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * counts = ral_new(ralHash);
    ral_hash_t * means = ral_new(ralHash);
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        ral_real_t * mean = ral_lookup(means, key);
        if (mean) {
            ral_integer_t * count = ral_lookup(counts, key);
            count->value += 1;
            mean->value += (GCv(gd, c) - mean->value) / count->value;
        } else {
            ral_insert(means, ral_real_new(GCv(gd, c)), key);
            ral_insert(counts, ral_integer_new(1), key);
        }
    }
    ral_delete(key);
    ral_delete(counts);
    return means;
 fail:
    ral_delete(counts);
    ral_delete(means);
    return NULL;
}

ral_hash_t * ral_grid_zonal_variance(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ral_hash_t * counts = ral_new(ralHash);
    ral_hash_t * means = ral_new(ralHash);
    ral_hash_t * variances = ral_new(ralHash);
    int i;
    ASSERT(ral_grid_overlayable(gd, zones) && zones->type == integer, BAD);
    ral_integer_t * key = ral_integer_new(0);
    FOR(c, gd) {
        double oldmean;
        if (GCivND(zones, c) OR GCND(gd, c)) continue;
        key->value = (long)GCiv(zones, c);
        double data_value = GCv(gd, c);
        ral_real_t * mean = ral_lookup(means, key);
        if (mean) {
            ral_integer_t * count = ral_lookup(means, key);
            ral_real_t * variance = ral_lookup(means, key);
            count->value += 1;
            oldmean = mean->value;
            mean->value += (data_value - oldmean) / count->value;
            variance->value += (data_value - oldmean) * (data_value - mean->value);
        } else {
            ral_insert(variances, ral_real_new(0), key);
            ral_insert(means, ral_real_new(data_value), key);
            ral_insert(counts, ral_integer_new(1), key);
        }
    }
    ral_delete(key);
    for (i = 0; i < counts->size_of_table; i++)
        if (counts->table[i]) {
            ral_kvp_t * a;
            for (a = counts->table[i]; a; a = a->next) {
                ral_real_t * variance = ral_hash_lookup(variances, a->key);
                ral_integer_t * count = (ral_integer_t *)a->value;
                if (count->value > 1) variance->value /= (count->value - 1);
            }
        }
    ral_delete(counts);
    ral_delete(means);
    return variances;
 fail:
    ral_delete(counts);
    ral_delete(means);
    ral_delete(variances);
    return NULL;
}

ral_hash_t * ral_grid_zonal_contents(ral_grid_t *gd, ral_grid_t *zones)
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(gd, zones) && 
           gd->type == integer && 
           zones->type == integer, BAD);
    ral_hash_t * zonal_contents = ral_new(ralHash);
    ral_integer_t * zkey = ral_integer_new(0);
    ral_integer_t * ckey = ral_integer_new(0);
    FOR(c, zones) {
        if (GCivND(zones, c) || GCivND(gd, c)) continue;
        zkey->value = (long)GCiv(zones, c);
        ckey->value = (long)GCiv(gd, c);
        ral_hash_t * contents = ral_lookup(zonal_contents, zkey);
        if (contents) {
            ral_integer_t * i = ral_lookup(contents, ckey);
            if (i)
                i->value++;
            else
                ral_insert(contents, ral_integer_new(1), ckey);
        } else {
            ral_hash_t * contents = ral_new(ralHash);
            ral_insert(contents, ral_integer_new(1), ckey);
            ral_insert(zonal_contents, contents, zkey);
        }
    }
    ral_delete(zkey);
    ral_delete(ckey);
    return zonal_contents;
fail:
    return NULL;
}


/** Global methods */

void ral_grid_set_all(ral_grid_t *gd, void *value, ral_error_t **e)
{
    if (value && value != ral_undef) {
        if (gd->type == integer) {
            ral_cell_t c;
            RAL_INTEGER i = ral_as_int(value);
            FOR(c, gd)
                GCiv(gd, c) = i;
        } else {
            ral_cell_t c;
            RAL_REAL r = ral_as_real(value);
            FOR(c, gd)
                GCnv(gd, c) = r;
        }
    } else {
        if (!gd->has_no_data_value) {
            if (e)
                *e = ral_error_new(1, "Warning: Setting no data value to -999.");
            gd->has_no_data_value = 1;
            if (gd->type == integer)
                gd->integer_no_data_value = -999;
            else
                gd->real_no_data_value = -999;
        }
        if (gd->type == integer) {
            ral_cell_t c;
            FOR(c, gd)
                GCiv(gd, c) = gd->integer_no_data_value;
        } else {
            ral_cell_t c;
            FOR(c, gd)
                GCnv(gd, c) = gd->real_no_data_value;
        }
    }
}

ral_interval_t *ral_grid_get_value_range(ral_grid_t *gd)
{
    ral_cell_t c;
    ral_interval_t *ab = ral_interval_new(ral_undef, ral_undef, 3);
    int f = 0;
    if (gd->type == integer) {
        FOR(c, gd) {
            if (GCivD(gd, c)) {
                long a = (long)GCiv(gd, c);
                if (!f) {
                    ab->min = ral_integer_new(a);
                    ab->max = ral_integer_new(a);
                    f = 1;
                } else {
                    ((ral_integer_t *)(ab->min))->value = MIN(((ral_integer_t *)(ab->min))->value, a);
                    ((ral_integer_t *)(ab->max))->value = MAX(((ral_integer_t *)(ab->max))->value, a);
                }
            }
        }
    } else {
        FOR(c, gd) {
            if (GCnvD(gd, c)) {
                double a = GCnv(gd, c);
                if (!f) {
                    ab->min = ral_real_new(a);
                    ab->max = ral_real_new(a);
                    f = 1;
                } else {
                    ((ral_real_t *)(ab->min))->value = MIN(((ral_real_t *)(ab->min))->value, a);
                    ((ral_real_t *)(ab->max))->value = MAX(((ral_real_t *)(ab->max))->value, a);
                }
            }
        }
    }
    return ab;
}

long ral_grid_count(ral_grid_t *gd)
{
    ral_cell_t c;
    long count = 0;
    if (gd->type == integer) {
        FOR(c, gd)
            if (GCivD(gd, c))
                count++;
    } else {
        FOR(c, gd)
            if (GCnvD(gd, c))
                count++;
    }
    return count;
}

long ral_grid_count_of(ral_grid_t *gd, RAL_INTEGER value)
{
    ral_cell_t c;
    long count = 0;
    ASSERT(gd->type == integer, BAD);
    FOR(c, gd)
        if (GCivD(gd, c) AND GCiv(gd, c) == value)
            count++;
 fail:
    return count;
}

double ral_grid_sum(ral_grid_t *gd)
{
    ral_cell_t c;
    double sum = 0;
    if (gd->type == integer) {
        FOR(c, gd)
            if (GCivD(gd, c))
                sum += GCiv(gd, c);
    } else if (gd->type == real) {
        FOR(c, gd)
            if (GCnvD(gd, c))
                sum += GCnv(gd, c);
    }
    return sum;
}

double ral_grid_mean(ral_grid_t *gd)
{
    ral_cell_t c;
    double mean = 0;
    int n = 0;
    if (gd->type == integer) {
        FOR(c, gd)
            if (GCivD(gd, c)) {
                n++;
                mean += (GCiv(gd, c) - mean) / n;
            }
    } else if (gd->type == real) {
        FOR(c, gd)
            if (GCnvD(gd, c)) {
                n++;
                mean += (GCnv(gd, c) - mean) / n;
            }
    } 
    return mean;
}

double ral_grid_variance(ral_grid_t *gd)
{
    ral_cell_t c;
    double variance = 0, mean = 0;
    int n = 0;
    if (gd->type == integer) {
        FOR(c, gd)
            if (GCivD(gd, c)) {
                double oldmean = mean;
                n++;
                mean += (GCiv(gd, c) - oldmean) / n;
                variance += (GCiv(gd, c) - oldmean) * (GCiv(gd, c) - mean);
            }
    } else if (gd->type == real) {
        FOR(c, gd)
            if (GCnvD(gd, c)) {
                double oldmean = mean;
                n++;
                mean += (GCnv(gd, c) - oldmean) / n;
                variance += (GCnv(gd, c) - oldmean) * (GCnv(gd, c) - mean);
            }
    } 
    if (n > 1) variance /= (n - 1);
    return variance;
}

ral_bin_t * ral_grid_get_histogram(ral_grid_t *gd, ral_array_t * bins)
{
    ral_cell_t c;
    ral_bin_t * histogram = ral_new(ralBin);
    int i, n = ral_number_of_elements(bins);
    for (i = 0; i < n; i++)
        histogram->bins[i] = ral_clone(bins->array[i]);
    if (gd->type == integer) {
        ral_integer_t *value = ral_integer_new(0);
        FOR(c, gd) {
            if (GCivND(gd, c)) continue;
            value->value = (long)GCiv(gd, c);
            ral_integer_t * count = ral_lookup(histogram, value);
            if (count)
                count->value++;
            else
                ral_insert(histogram, ral_integer_new(1), value);
        }
        ral_delete(value);
    } else {
        ral_real_t * value = ral_real_new(0);
        FOR(c, gd) {
            if (GCnvD(gd, c)) {
                value->value = GCnv(gd, c);
                ral_integer_t * count = ral_lookup(histogram, value);
                if (count)
                    count->value++;
                else
                    ral_insert(histogram, ral_integer_new(1), value);
            }
        }
        ral_delete(value);
    }
    return histogram;
}

ral_hash_t * ral_grid_get_contents(ral_grid_t *gd) 
{
    ral_cell_t c;
    ral_hash_t * hash = ral_new(ralHash);
    if (gd->type == integer) {
        ral_integer_t * key = ral_integer_new(0);
        FOR(c, gd) {
            if (GCivND(gd, c)) continue;
            key->value = (long)GCiv(gd, c);
            ral_integer_t * count = ral_lookup(hash, key);
            if (count)
                count->value++;
            else
                ral_insert(hash, ral_integer_new(1), key);
        }
        ral_delete(key);
    } else {
        /* refuse to list contents of a grid with real number values */
        ral_delete(hash);
        hash = NULL;
    }
    return hash;
}

ral_grid_t *ral_grid_cross(ral_grid_t *a, ral_grid_t *b, ral_error_t **e)
{
    ral_grid_t *c = NULL;
    ral_cell_t p;
    ral_hash_t * atable = NULL, *btable = NULL;
    ral_array_t * ca = NULL, *cb = NULL;
    ASSERT((a->type == integer) AND (b->type == integer), BAD);
    ASSERT(ral_grid_overlayable(a, b), BAD);
    FAIL_UNLESS(atable = ral_grid_get_contents(a));
    FAIL_UNLESS(btable = ral_grid_get_contents(b));
    FAIL_UNLESS(ca = ral_keys(atable));
    FAIL_UNLESS(cb = ral_keys(btable));
    FAIL_UNLESS(c = ral_grid_new_like(a, integer, e));
    ral_array_sort(ca);
    ral_array_sort(cb);
    int na = ral_number_of_elements(ca);
    int nb = ral_number_of_elements(cb);
    FOR(p, a) {
        if (GCivD(a, p) AND GCivD(b, p)) {
            ral_integer_t * ia = ral_integer_new(0); ia->value = 0;
            ral_integer_t * ib = ral_integer_new(0); ib->value = 0;
            while (ia->value < na AND ral_as_int(ral_lookup(ca, ia)) != GCiv(a, p)) ia->value++;
            while (ib->value < nb AND ral_as_int(ral_lookup(cb, ib)) != GCiv(b, p)) ib->value++;
            GCiv(c, p) = ib->value + ia->value*na + 1;
            ral_delete(ia);
            ral_delete(ib);
        } else 
            ral_grid_set(c, ral_rcoords_new(p.x, p.y), ral_undef, NULL);
    }
 fail:
    ral_delete(atable);
    ral_delete(btable);
    ral_delete(ca);
    ral_delete(cb);
    return c;
}

/* if a then b = c */
void ral_grid_if_then(ral_grid_t *a, ral_grid_t *b, void *c)
{
    ral_cell_t p;
    ASSERT(a->type == integer &&
           ral_grid_overlayable(a, b), BAD);
    if (b->type == integer) {
        RAL_INTEGER i = ral_as_int(c);
        FOR(p, a)
            if (GCivD(a, p) AND GCiv(a, p)) GCiv(b, p) = i;
    } else if (b->type == real) {
        FOR(p, a)
            if (GCivD(a, p) AND GCiv(a, p)) GCnv(b, p) = ral_as_real(c);
    
    }
 fail:
    return;
}

/* if a then b = c else b = d */
void ral_grid_if_then_else(ral_grid_t *a, ral_grid_t *b, void *c, void *d)
{
    ral_cell_t p;
    ASSERT(a->type == integer &&
           ral_grid_overlayable(a, b), BAD);
    if (b->type == integer) {
        RAL_INTEGER x = ral_as_int(c), y = ral_as_int(d);
        FOR(p, a) {
            if (GCivD(a, p) AND GCiv(a, p)) {
                GCiv(b, p) = x;
            } else {
                GCiv(b, p) = y;
            }
        }
    } else if (b->type == real) {
        FOR(p, a) {
            if (GCivD(a, p) AND GCiv(a, p)) {
                GCnv(b, p) = ral_as_real(c);
            } else {
                GCnv(b, p) = ral_as_real(d);
            }
        }
    
    }
 fail:
    return;
}

/* if a then b = c */
void ral_grid_if_then_grid(ral_grid_t *a, ral_grid_t *b, ral_grid_t *c)
{
    ral_cell_t p;
    ASSERT(a->type == integer &&
           ral_grid_overlayable(a, b) &&
           ral_grid_overlayable(a, c), BAD);
    if (b->type == integer) {
        if (c->type == integer) {
            FOR(p, a)
                if (GCivD(a, p) AND GCivD(c, p) AND GCiv(a, p)) 
                    GCiv(b, p) = GCiv(c, p);
        } else {
            FOR(p, a)
                if (GCivD(a, p) AND GCnvD(c, p) AND GCiv(a, p))
                    GCiv(b, p) = ROUND(GCnv(c, p));
        }
    } else {
        if (c->type == integer) {
            FOR(p, a)
                if (GCivD(a, p) AND GCivD(c, p) AND GCiv(a, p)) 
                    GCnv(b, p) = GCiv(c, p);
        } else {
            FOR(p, a)
                if (GCivD(a, p) AND GCnvD(c, p) AND GCiv(a, p)) 
                    GCnv(b, p) = GCnv(c, p);
        }
    }
fail:
    return;
}

/* hash: reclassify only those explicitly indicated, however '*' is a
 * default mapping. bin: by default reclassifies everything, however,
 * 'null' means do not classify. */
void ral_grid_reclassify(ral_grid_t *gd, ral_classifier_t *classifier, ral_error_t **e)
{
    /* things to assert: gd, classifier, classifier type, classifier result type */
    assert(classifier && " must not be NULL");
    ral_class ret_type = ral_get_output_type(classifier);
    /*fprintf(stderr, "data type in entry %s\n", ral_data_element_type(gd));*/
    /*fprintf(stderr, "ret=%s, %p classifier=%s\n", ral_name_of_class(ret_type), ret_type, as_string(classifier));*/
    ral_grid_t *ret = gd;
    ral_cell_t c;
    ral_string_t * s = ral_new(ralString);
    ral_string_set(s, "*");
    void *def = ral_lookup(classifier, s);
    ral_delete(s);
    if (gd->type == integer) {        
        if (ret_type == ralReal) ret = ral_grid_new_like(gd, real, NULL);
        ral_integer_t * key = ral_integer_new(0);
        FOR(c, gd) {
            if (GCivD(gd, c)) {
                key->value = (long)GCiv(gd, c);
                void *y = ral_lookup(classifier, key);
                /*fprintf(stderr, "int classified: %li=>%s(%s)\n", key->value, as_string(y), ral_cname(y));*/
                if (!y) y = def;
                if (!y);
                else if (class_of(y) != ralString) /* all strings mean: do not reclassify */
                    ral_grid_set(ret, ral_rcoords_new(c.x, c.y), y, NULL);
                else if (ret != gd)
                    GCnv(ret, c) = GCiv(gd, c);
            }
        }
        ral_delete(key);
    } else {
        if (class_of(classifier) == ralHash) {
            *e = ral_error_new(0, "Real valued grids can't be reclassified with hashes.");
            goto fail;
        }
        if (ret_type == ralInteger) ret = ral_grid_new_like(gd, integer, e);
        ral_real_t * key = ral_real_new(0.0);
        FOR(c, gd) {
            if (GCnvD(gd, c)) {
                key->value = GCnv(gd, c);
                void *y = ral_lookup(classifier, key);
                /*fprintf(stderr, "real classified: %f=>%s(%s)\n", key->value, as_string(y), ral_cname(y));*/
                if (!y) y = def;
                if (!y);
                else if (class_of(y) != ralString) /* all strings mean: do not reclassify */
                    ral_grid_set(ret, ral_rcoords_new(c.x, c.y), y, NULL);
                else if (ret != gd)
                    GCiv(ret, c) = round(GCnv(gd, c));
            }
        }
        ral_delete(key);
    }
    if (ret != gd) swap_grids(gd, ret);
    /*fprintf(stderr, "data type in return %s\n", ral_data_element_type(gd));*/
fail:;
}
