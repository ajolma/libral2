#include "ral2/ral.h"
#include "priv/priv.h"

/* 
   a border cell of an area is a grid cell which has at least one cell
   in its 8-neighborhood which has a different value than the area
   value or the cell is on the border of the grid
   
   this algorithm walks the border of an area and tests each
   bordercell with a given test function
*/

/* 
   a is the place where to start walking
   out is a direction which is away from the area whose border is to be walked
*/
int borderwalk(ral_cell_t a, int out, void *param, ral_cell_fct is_area, ral_cell_fct test)
{
    ral_cell_t c = a;
    int da = 0;
    int d = out;
    /* search for the first direction which is 
       clockwise an area cell just after direction out */
    ral_cell_t t = ral_cell_move(c, out);

    /* is a on the area? */
    ASSERT(!is_area(c, param), "c is not on the area");

    /* is out really off the area? */
    ASSERT(is_area(t, param), "out is on the area");

    while (!is_area(t, param)) {
	d = RAL_NEXT_DIR(d);
	if (d == out) /* a is the area */
	    break;
	t = ral_cell_move(c, d);
    }
    FAIL_UNLESS(test(c, param));

    if (d == out) /* a is the area */
	return 1;
    while (1) {
	t = ral_cell_move(c, d);
	while (!is_area(t, param)) {
	    d = RAL_NEXT_DIR(d);
	    /* there _IS_ a 8-neighbor which is on the area */
	    t = ral_cell_move(c, d);
	}
	/* this is a ghost by feng shui definition, 
	   i.e. this prefers walking to dir 1, 3, 5, or 7 */
	if (EVEN(d)) {
	    int od = RAL_NEXT_DIR(d);
	    t = ral_cell_move(c, od);
	    if (is_area(t, param))
		d = od;
	}
	/* test if we are done */
	if (SAME_CELL(c, a)) {
	    if (d == da)
		return 1;
	    else if (!da)
		da = d;
	}
	c = ral_cell_move(c, d);

	FAIL_UNLESS(test(c, param));
	d += 6;
	if (d > 8) d -= 8;
    }
    /* should never be here */
 fail:
    return 0;
}

/*

  Fits a 9-term quadratic polynomial 

  NOTE: assumes dx = dy as in grid

z = A * x^2y^2 + B * x^2y + C * xy^2 + D * x^2 + E * y^2 + F * xy + G * x + H *y + I

a[1] = A
a[2] = B
a[3] = C
a[4] = D
a[5] = E
a[6] = F
a[7] = G
a[8] = H
a[9] = I

  to a 3*3 square grid centered at the center point of cell c

  z's:   1 2 3
         4 5 6
         7 8 9

  ^
  |
y |
  |
   ---> 
    x

z1 =  1 * A +  1 * B + -1 * C + 1 * D + 1 * E + -1 * F + -1 * G +  1 * H + I
z2 =  0 * A +  0 * B +  0 * C + 0 * D + 1 * E +  0 * F +  0 * G +  1 * H + I
z3 =  1 * A +  1 * B +  1 * C + 1 * D + 1 * E +  1 * F +  1 * G +  1 * H + I
z4 =  0 * A +  0 * B +  0 * C + 1 * D + 0 * E +  0 * F + -1 * G +  0 * H + I
z5 =  0 * A +  0 * B +  0 * C + 0 * D + 0 * E +  0 * F +  0 * G +  0 * H + I
z6 =  0 * A +  0 * B +  0 * C + 1 * D + 0 * E +  0 * F +  1 * G +  0 * H + I
z7 =  1 * A + -1 * B + -1 * C + 1 * D + 1 * E +  1 * F + -1 * G + -1 * H + I
z8 =  0 * A +  0 * B +  0 * C + 0 * D + 1 * E +  0 * F +  0 * G + -1 * H + I
z9 =  1 * A + -1 * B +  1 * C + 1 * D + 1 * E + -1 * F +  1 * G + -1 * H + I

A =  0.25 * z1 +  0.5 * z2 + 0.25 * z3 + -0.5 * z4 +  1 * z5 + -0.5 * z6 +  0.25 * z7 + -0.5 * z8 +  0.25 * z9
B =  0.25 * z1 + -0.5 * z2 + 0.25 * z3 +  0   * z4 +  0 * z5 +  0   * z6 + -0.25 * z7 +  0.5 * z8 + -0.25 * z9
C = -0.25 * z1 +  0   * z2 + 0.25 * z3 +  0.5 * z4 +  0 * z5 + -0.5 * z6 + -0.25 * z7 +  0   * z8 +  0.25 * z9
D =  0    * z1 +  0   * z2 + 0    * z3 +  0.5 * z4 + -1 * z5 +  0.5 * z6 +  0    * z7 +  0   * z8 +  0    * z9
E =  0    * z1 +  0.5 * z2 + 0    * z3 +  0   * z4 + -1 * z5 +  0   * z6 +  0    * z7 +  0.5 * z8 +  0    * z9
F = -0.25 * z1 +  0   * z2 + 0.25 * z3 +  0   * z4 +  0 * z5 +  0   * z6 +  0.25 * z7 +  0   * z8 + -0.25 * z9
G =  0    * z1 +  0   * z2 + 0    * z3 + -0.5 * z4 +  0 * z5 +  0.5 * z6 +  0    * z7 +  0   * z8 +  0    * z9
H =  0    * z1 +  0.5 * z2 + 0    * z3 +  0   * z4 +  0 * z5 +  0   * z6 +  0    * z7 + -0.5 * z8 +  0    * z9
I =  0    * z1 +  0   * z2 + 0    * z3 +  0   * z4 +  1 * z5 +  0   * z6 +  0    * z7 +  0   * z8 +  0    * z9

z_factor is the unit of z divided by the unit of x and y

*/

int ral_fitpoly(ral_grid_t *dem, double a[], ral_cell_t c, double z_factor) 
{
    int ix = 1;
    int ii, jj;
    double z[10];
    z[0] = 0;
    for (ii = c.y-1; ii <= c.y+1; ii++) {
	for (jj = c.x-1; jj <= c.x+1; jj++) {
	    ral_cell_t c2;
	    if (ii < 0) {
		c2.y = 0;
	    } else if (ii > dem->height-1) {
		c2.y = dem->height-1;
	    } else {
		c2.y = ii;
	    }
	    if (jj < 0) {
		c2.x = 0;
	    } else if (jj > dem->width-1) {
		c2.x = dem->width-1;
	    } else {
		c2.x = jj;
	    }
	    if (GCD(dem, c2)) 
		z[ix] =  GCv(dem, c2);
	    else
		z[ix] = GCv(dem, c);
	    z[ix] *= (z_factor / dem->cell_size);
	    ix++;
	}
    }
    a[0] = 0;
    a[1] =  0.25 * z[1] +  0.5 * z[2] + 0.25 * z[3] -  0.5 * z[4] + z[5] -  0.5 * z[6] +  0.25 * z[7] -  0.5 * z[8] +  0.25 * z[9];
    a[2] =  0.25 * z[1] -  0.5 * z[2] + 0.25 * z[3]                                    -  0.25 * z[7] +  0.5 * z[8] -  0.25 * z[9];
    a[3] = -0.25 * z[1]               + 0.25 * z[3] +  0.5 * z[4]        -  0.5 * z[6] -  0.25 * z[7]               +  0.25 * z[9];
    a[4] =                                             0.5 * z[4] - z[5] +  0.5 * z[6]                                            ;
    a[5] =                 0.5 * z[2]                             - z[5]                              +  0.5 * z[8]               ;
    a[6] = -0.25 * z[1]               + 0.25 * z[3]                                    +  0.25 * z[7]               -  0.25 * z[9];
    a[7] =                                            -0.5 * z[4]        +  0.5 * z[6]                                            ;
    a[8] =                 0.5 * z[2]                                                                 -  0.5 * z[8]               ;
    a[9] =                                                          z[5]                                                          ;
    return 1;
}

/*
        j
        ->      ^
     i |        | 0  -> Pi/2   | Pi   <- 3*Pi/2
       V                       V
  
*/
double ral_aspect(double a[]) 
{
    double asp;
    if (fabs(a[8]) < EPSILON) {
	if (fabs(a[7]) < EPSILON) return -1;
	if (a[7] < 0) return M_PI/2.0;
	return 3.0/2.0 * M_PI;
    }
    if (fabs(a[7]) < EPSILON) {
	if (a[8] > 0) return M_PI;
	if (a[7] >= 0) return 2.0 * M_PI;
	return 0;
    }
    asp = M_PI - atan2(a[8]/a[7],1) + M_PI/2.0 * (a[7]/fabs(a[7]));
    if (asp<0) return 0;
    if (asp>2*M_PI) return 2*M_PI;
    return asp;
}

ral_grid_t *ral_dem_aspect(ral_grid_t *dem)
{
    ral_grid_t *ag = NULL;
    ral_cell_t c;
    FAIL_UNLESS(ag = ral_grid_new_like(dem, real, NULL));
    FOR(c, dem) {
	if (GCD(dem, c)) {
	    double a[10];
	    ral_fitpoly(dem, a, c, 1);
	    GCnv(ag, c) = ral_aspect(a);
	} else {
            ral_rcoords_t *rc = ral_new(ralRCoords);
            rc->x = c.x;
            rc->y = c.y;
	    ral_grid_set(ag, rc, ral_undef, NULL);
            ral_delete(rc);
	}
    }
    return ag;
 fail:
    return NULL;
}

ral_grid_t *ral_dem_slope(ral_grid_t *dem, double z_factor)
{
    ral_grid_t *sg = NULL;
    ral_cell_t c;
    FAIL_UNLESS(sg = ral_grid_new_like(dem, real, NULL));
    FOR(c, dem) {
	if (GCD(dem, c)) {
	    double a[10];
	    ral_fitpoly(dem, a, c, z_factor);
	    GCnv(sg, c) = atan(sqrt(a[7]*a[7]+a[8]*a[8]));
	} else {
            ral_rcoords_t *rc = ral_new(ralRCoords);
            rc->x = c.x;
            rc->y = c.y;
	    ral_grid_set(sg, rc, ral_undef, NULL);
            ral_delete(rc);
	}
    }
    return sg;
 fail:
    return NULL;
}

double ral_flatness_threshold(ral_grid_t *dem)
{
    if (dem->type == integer)
	return 0;
    else
	return 0.0; /* there is a problem draining flat areas if this is left non-zero */
}

ral_grid_t *ral_dem_fdg(ral_grid_t *dem, int method, ral_error_t **e) 
{
    ral_grid_t *fdg = NULL;
    ral_cell_t c;
    double dz = ral_flatness_threshold(dem);
    FAIL_UNLESS(fdg = ral_grid_new_like(dem, integer, e));
    ral_grid_set_all(fdg, ral_undef, NULL);
    FOR(c, dem) {
	if (GCD(dem, c)) {
	    /* 
	       test for all eight-neighbors of c and 
	       remember the direction to the one with steepest descent to 
	       if a nodata available flow there
	       if on the border of the grid, flow outside
	    */
	    int dir, found = 0, dir_t = 0, has_non_higher = 0, many = 0;
	    double zc = GCv(dem, c), descent = 0;

	    RAL_DIRECTIONS(dir) {
		ral_cell_t t = ral_cell_move(c, dir);
		if (GCin(dem, t) AND GCD(dem, t)) {
		    double zt = GCv(dem, t);
		    if (zt < zc - dz) {
			double coeff = 1;
			if (EVEN(dir)) {
			    if (method == RAL_RHO8) {
				double rho = ((double)rand()) / RAND_MAX;
				coeff = 1.0/(2.0-rho);
			    } else {
				coeff = sqrt(1.0/2.0);
			    }
			}
			if (!found OR coeff*(zc-zt) > descent) {
			    found = 1;
			    descent = coeff*(zc-zt);
			    dir_t = dir;
			}
			many |= 1 << (dir-1); /* bits in the byte tell downslope cells */
		    } else if (zt <= zc + dz) {
			has_non_higher = 1;
		    }
		} else {
		    found = 1;
		    dir_t = dir;
		    many = 1 << (dir-1);
		    break;
		}
	    }
	    if (found) {
		if (method == RAL_MANY8)
		    dir = many;
		else
		    dir = dir_t;
	    } else if (has_non_higher)
		dir = RAL_FLAT_AREA;
	    else 
		dir = RAL_PIT_CELL;
	    GCiv(fdg, c) = dir;
	}
    }
    return fdg;
 fail:
    return NULL;
}

int fdg_is_outlet(ral_grid_t *fdg, ral_grid_t *streams, ral_cell_t c)
{
    if (GCin(fdg, c) AND GCD(fdg, c) AND GCv(fdg, c) > 0 AND GCv(streams, c)) {
	c = FLOW(fdg, c);
	if (GCout(fdg, c) OR GCND(fdg, c) OR GCND(streams, c))
	    return 1;
    }
    return 0;
}

ral_rcoords_t * ral_fdg_outlet(ral_grid_t *fdg, ral_grid_t *streams, ral_rcoords_t * rc)
{
    ral_cell_t c = {rc->y, rc->x};
    ral_cell_t previous = c;
    while (GCin(fdg, c) AND GCD(fdg, c) AND GCv(fdg, c) > 0 AND GCD(streams, c)) {
	previous = c;
	c = FLOW(fdg, c);
    }
    return ral_rcoords_new(previous.x, previous.y);
}

ral_grid_t *ral_dem_ucg(ral_grid_t *dem, ral_error_t **e) 
{
    ral_grid_t *ucg = NULL;
    ral_cell_t c;
    double dz = ral_flatness_threshold(dem);
    FAIL_UNLESS(ucg = ral_grid_new_like(dem, integer, e));
    ral_grid_set_all(ucg, ral_undef, NULL);
    FOR(c, dem) {
	if (GCD(dem, c)) {
	    int dir, many = 0;
	    double zc = GCv(dem, c);
	    RAL_DIRECTIONS(dir) {
		ral_cell_t t = ral_cell_move(c, dir);
		if (GCin(dem, t) AND GCD(dem, t)) {
		    double zt = GCv(dem, t);
		    if (zt > zc + dz) {
			many |= 1 << (dir-1); /* bits in the byte tell upslope cells */
		    } 
		}
	    }
	    GCiv(ucg, c) = many;
	}
    }
    return ucg;
 fail:
    return NULL;
}

int ral_init_pour_point_struct(pour_point_t *pp, ral_grid_t *fdg, ral_grid_t *dem, ral_grid_t *mark)
{
    ASSERT(mark && mark->type == integer &&
           (!dem || ral_grid_overlayable(fdg, dem)) &&
           ral_grid_overlayable(fdg, mark) &&
           fdg->type == integer, BAD);
    pp->fdg = fdg;
    pp->dem = dem;
    pp->mark = mark;
    if (dem) pp->dz = ral_flatness_threshold(dem);
    pp->bc_found = 0;
    pp->test_inner = 1;
    pp->pp_found = 0;
    pp->pour_to_no_data = 0;
    return 1;
fail:
    return 0;
}

#define IS_FLAT_AND_NOT_MARKED(pp,t) (GCin((pp)->fdg, (t)) AND \
                       GCivD((pp)->fdg, (t)) AND \
                       !GCiv((pp)->mark, (t)) AND \
                       (GCiv((pp)->fdg, (t)) == RAL_FLAT_AREA))

void ral_markflat(pour_point_t *pp, ral_cell_t c)
{
    int upLimit,downLimit;
    ral_cell_t t = c;

    if (!IS_FLAT_AND_NOT_MARKED(pp, c)) return;

    /* Seek up for the last flat cell */
    
    for (t.y = c.y; t.y >= 0; t.y--) {
	if (!IS_FLAT_AND_NOT_MARKED(pp, t)) break;
	GCiv(pp->mark, t) = 1;
	pp->counter++;
    }
    if (!pp->bc_found) {
	pp->bc_found = 1;
	pp->dir_out = 1;	
	pp->bc = t;
	if (pp->bc.y < 0) pp->bc.y++;
	if (!GCiv(pp->mark, pp->bc)) pp->bc.y++;
    }
    upLimit = MAX(0,t.y);

    /* Seek down and mark and count */
    for (t.y = c.y+1; t.y < pp->mark->height; t.y++) {    
	if (!IS_FLAT_AND_NOT_MARKED(pp, t)) break;
	GCiv(pp->mark, t) = 1;
	pp->counter++;
    }
    downLimit = MIN(pp->mark->height-1,t.y);

    /* Look at columns right and left */
    /* left */
    if (c.x > 0) {
	int lastBorder = 1;
	t.x = c.x-1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a = IS_FLAT_AND_NOT_MARKED(pp, t);
	    if (lastBorder) {
		if (a) {
		    ral_markflat(pp, t);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }

    /* right */
    if (c.x < (pp->mark->width - 1)) {
	int lastBorder = 1;
	t.x = c.x+1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a = IS_FLAT_AND_NOT_MARKED(pp, t);
	    if (lastBorder) {
		if (a) {
		    ral_markflat(pp, t);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }
}

/* this is called for each candidate inner pour point (border cell) */
int test_pour_point(ral_cell_t c, void *param) {
    pour_point_t *pp = (pour_point_t *)param;
    int d, d_in;
    double zc, slope_in = 0;
    ral_cell_t c_in;

    zc = GCv(pp->dem, c);
    d_in = GCiv(pp->fdg, c);
    c_in = ral_cell_move(c, d_in);

    /* this border cell might also be a pour to no_data cell */
    if (d_in AND GCin(pp->fdg, c_in) AND GCivD(pp->fdg, c_in)) 
	slope_in = (zc - GCv(pp->dem, c_in))/RAL_DISTANCE_UNIT(d_in);

    if (pp->pour_to_no_data) {
	if (zc < pp->z_ipp)
	    pp->z_ipp = zc;
    } else {
	for (d = 1; d < 9; d++) {
	    ral_cell_t t = ral_cell_move(c, d);
	    double zt, slope_out;
	    if (GCout(pp->fdg, t) OR GCivND(pp->fdg, t)) {
		pp->pp_found = 1;
		pp->ipp = c;
		pp->z_ipp = zc;
		pp->in2out = d;
		pp->pour_to_no_data = 1;
		break;
	    }
	    if (GCiv(pp->mark, t)) continue;
	    zt = GCv(pp->dem, t);
	    slope_out = (zc - zt)/RAL_DISTANCE_UNIT(d);
	    if (!pp->pp_found OR 
		(pp->test_inner AND 
		 (zc < pp->z_ipp OR 
		  (zc == pp->z_ipp AND slope_out > pp->slope_out) OR
		  (zc == pp->z_ipp AND slope_out == pp->slope_out AND d_in AND slope_in > pp->slope_in))) OR
		(!pp->test_inner AND 
		 (zt < pp->z_opp OR 
		  (zt == pp->z_opp AND RAL_DISTANCE_UNIT(d) < pp->dio)))) {
		pp->pp_found = 1;
		pp->ipp = c;
		pp->z_ipp = zc;
		pp->slope_in = slope_in;
		pp->slope_out = slope_out;
		pp->dio = RAL_DISTANCE_UNIT(d);
		pp->in2out = d;
		pp->opp = t;
		pp->z_opp = zt;
	    }
	}
    }
    return 1;
}

int ral_is_marked(ral_cell_t c, void *param) {
    return GCin(((pour_point_t *)param)->mark, c) AND 
	GCiv(((pour_point_t *)param)->mark, c);
}

void drain_flat_area_to_pour_point(pour_point_t *pp, ral_cell_t c, int dir_for_c) 
{
    int upLimit,downLimit;
    int at_up_4_or_6 = 0;
    int at_down_2_or_8 = 0;
    ral_cell_t t = c;
    
    if (!GCiv(pp->mark, c)) return;

    if (dir_for_c > 0) GCiv(pp->fdg, c) = dir_for_c;

    /* Seek up */
    for (t.y = c.y; t.y >= 0; t.y--) {
	if (!GCiv(pp->mark, t)) {
	    at_up_4_or_6 = 1;
	    break;	    
	}
	GCiv(pp->mark, t) = 0;
	if (t.y < c.y) GCiv(pp->fdg, t) = 5; /* down */
    }
    upLimit = MAX(0,t.y);

    /* Seek down */
    for (t.y = c.y+1; t.y < pp->mark->height; t.y++) {    
	if (!GCiv(pp->mark, t)) {
	    at_down_2_or_8 = 1;
	    break;
	}
	GCiv(pp->mark, t) = 0;
	GCiv(pp->fdg, t) = 1; /* up */
    }
    downLimit = MIN(pp->mark->height-1,t.y);

    /* Look at columns right and left */
    /* left */
    if (c.x > 0) {
	int lastBorder = 1;
	t.x = c.x-1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a;
	    if ((t.y == upLimit) AND at_up_4_or_6) {
		dir_for_c = 4;
	    } else if ((t.y == downLimit) AND at_down_2_or_8) {
		dir_for_c = 2;
	    } else {
		dir_for_c = 3;
	    }
	    a = GCiv(pp->mark, t);
	    if (lastBorder) {
		if (a) {
		    drain_flat_area_to_pour_point(pp, t, dir_for_c);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }

    /* right */
    if (c.x < (pp->mark->width - 1)) {
	int lastBorder = 1;
	t.x = c.x+1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a;
	    if ((t.y == upLimit) AND at_up_4_or_6) {
		dir_for_c = 6;
	    } else if ((t.y == downLimit) AND at_down_2_or_8) {
		dir_for_c = 8;
	    } else {
		dir_for_c = 7;
	    }
	    a = GCiv(pp->mark, t);
	    if (lastBorder) {
		if (a) {
		    drain_flat_area_to_pour_point(pp, t, dir_for_c);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }
}

void raise_area(pour_point_t *pp, ral_cell_t c, double z)
{
    int lastBorder;
    int leftLimit, rightLimit;
    ral_cell_t t;
    /* Seek up */
    leftLimit = (-1);
    t.x = c.x;
    for (t.y = c.y; (t.y >= 0); t.y--) {
	if (!(GCivD(pp->fdg, t) AND GCiv(pp->mark, t))) {
	    break;
	}
	GCiv(pp->mark, t) = 0;
	ral_grid_set(pp->dem, ral_rcoords_new(t.x, t.y), ral_real_new(z), NULL); /* dem may be int or float */
	leftLimit = MAX(0,t.y-1);
    }
    if (leftLimit == (-1)) {
	return;
    }
    /* Seek down */
    rightLimit = MIN(pp->dem->height-1,c.y+1);
    for (t.y = (c.y+1); (t.y < pp->dem->height); t.y++) {	
	if (!(GCivD(pp->fdg, t) AND GCiv(pp->mark, t))) {
	    break;
	}
	GCiv(pp->mark, t) = 0;
	ral_grid_set(pp->dem, ral_rcoords_new(t.x, t.y), ral_real_new(z), NULL); /* dem may be int or float */
	rightLimit = MIN(pp->dem->height-1,t.y+1);
    }
    /* Look at columns right and left and start paints */
    /* right */
    if (c.x > 0) {
	t.x = c.x-1;
	lastBorder = 1;
	for (t.y = leftLimit; (t.y <= rightLimit); t.y++) {
	    int a = (GCivD(pp->fdg, t) AND GCiv(pp->mark, t));
	    if (lastBorder) {
		if (a) {	
		    raise_area(pp, t, z);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }
    /* left */
    if (c.x < ((pp->dem->width) - 1)) {
	t.x = c.x+1;
	lastBorder = 1;
	for (t.y = leftLimit; (t.y <= rightLimit); t.y++) {
	    int a = (GCivD(pp->fdg, t) AND GCiv(pp->mark, t));
	    if (lastBorder) {
		if (a) {
		    raise_area(pp, t, z);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }
}

int ral_flat_and_pit_cells(ral_grid_t *fdg)
{
    ral_cell_t c;
    int count = 0;
    FOR(c, fdg)
	if (GCiv(fdg, c) == RAL_FLAT_AREA OR GCiv(fdg, c) == RAL_PIT_CELL) count++;
    return count;
}

int ral_fdg_drain_flat_areas1(ral_grid_t *fdg, ral_grid_t *dem)
{
    double dz = ral_flatness_threshold(dem);
    double zf; /* elevation of the flat area */
    ral_cell_t c;
    int done = -1;
    int fixed_flats = 0;
    ASSERT(ral_grid_overlayable(fdg, dem) && fdg->type == integer, BAD);
    while (done != fixed_flats) {
	done = fixed_flats;
	FOR(c, fdg) {
	    if (GCiv(fdg, c) == RAL_FLAT_AREA OR GCiv(fdg, c) == RAL_PIT_CELL) {	    
		/* 
		   are there non-higher neigbors with dge resolved?
		   or are there outside cells in the neighborhood?
		*/
		double zl = 0, zt;
		int dir = 0, d, invdir = 5;
		int is_on_border = 0;
		zf = GCv(dem, c);
		for (d = 1; d < 9; d++) {
		    int dt;
		    ral_cell_t t = ral_cell_move(c, d);
		    if (GCout(fdg, t) OR GCivND(fdg, t)) {
			is_on_border = d;
			continue;
		    }
		    zt = GCv(dem, t);
		    dt = GCiv(fdg, t);
		    if (dt > 0 AND dt != invdir AND
			zt <= zf + dz AND
			(!dir OR zt < zl)) {
			zl = zt;
			dir = d;
		    }
		    invdir++;
		    if (invdir > 8) invdir = 1;
		}
		if (dir) {
		    GCiv(fdg, c) = dir;
		    fixed_flats++;
		} else if (is_on_border) {
		    GCiv(fdg, c) = is_on_border;
		    fixed_flats++;
		}
	    }
	}
    }
    return fixed_flats;
fail:
    return -1;
}

int ral_fdg_drain_flat_areas2(ral_grid_t *fdg, ral_grid_t *dem, ral_error_t **e) 
{
    ral_cell_t c;
    int fixed_flats = 0;
    pour_point_t pp;
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, dem, ral_grid_new_like(fdg, integer, e)));
    pp.dz = -1; /* do not go beyond flat areas defined by the FDG */
    FOR(c, fdg) {
	if (GCiv(fdg, c) != RAL_FLAT_AREA) continue;

	pp.zf = GCv(pp.dem, c);

	/* mark the flat area and find a definitive border cell */
	pp.bc_found = 0;
	pp.counter = 0;
	ral_markflat(&pp, c);

	/* walk the border */
	pp.test_inner = 0;
	pp.pp_found = 0;
	pp.pour_to_no_data = 0;
	FAIL_UNLESS(borderwalk(pp.bc, pp.dir_out, &pp, &ral_is_marked, &test_pour_point));

	/* if the elevation of the outer pour point is not higher than inner pour point */
	/* or the outer pour point is no_data */
	/* drain there, else make inner pour point a pit cell */
	if (pp.pour_to_no_data OR pp.z_opp <= GCv(pp.dem, pp.ipp))
	    GCiv(pp.fdg, pp.ipp) = pp.in2out;
	else
	    GCiv(pp.fdg, pp.ipp) = RAL_PIT_CELL;

	/* starting from the inner pour point make the whole flat area
	   drain into it, this unmarks the flat area along the way */

	drain_flat_area_to_pour_point(&pp, pp.ipp, 0); 

	fixed_flats++;
    }
    ral_delete(pp.mark);
    return fixed_flats;
 fail:
    ral_delete(pp.mark);
    return -1;
}

int ral_dem_raise_pits(ral_grid_t *dem, double z_limit)
{
    ral_cell_t c;
    int pits_filled = 0;
    FOR(c, dem) {
	int d, f = 0;
	double zc, z_lowest_nbor = 0;
	if (GCND(dem, c)) continue;
	zc = GCv(dem, c);
	RAL_DIRECTIONS(d) {
	    ral_cell_t t = ral_cell_move(c, d);
	    if (GCin(dem, t) AND GCD(dem, t)) {
		double zt = GCv(dem, t);
		if (!f OR zt < z_lowest_nbor) {
		    f = 1;
		    z_lowest_nbor = zt;
		    if (z_lowest_nbor < zc + z_limit) {
			f = 0;
			break;
		    }
		}
	    }
	}
	if (f) {
	    ral_grid_set(dem, ral_rcoords_new(c.x, c.y), ral_real_new(z_lowest_nbor), NULL);
	    pits_filled++;
	}
    }
    return pits_filled;
}

int ral_dem_lower_peaks(ral_grid_t *dem, double z_limit)
{
    ral_cell_t c;
    int peaks_cut = 0;
    FOR(c, dem) {
	int d, f = 0;
	double zc, z_highest_nbor = 0;
	if (GCND(dem, c)) continue;
	zc = GCv(dem, c);
	RAL_DIRECTIONS(d) {
	    ral_cell_t t = ral_cell_move(c, d);
	    if (GCin(dem, t) AND GCD(dem, t)) {
		double zt = GCv(dem, t);
		if (!f OR zt > z_highest_nbor) {
		    f = 1;
		    z_highest_nbor = zt;
		    if (z_highest_nbor > zc - z_limit) {
			f = 0;
			break;
		    }
		}
	    }
	}
	if (f) {
	    ral_grid_set(dem, ral_rcoords_new(c.x, c.y), ral_real_new(z_highest_nbor), NULL);
	    peaks_cut++;
	}
    }
    return peaks_cut;
}

/* mark upslope cells with number m to pp->mark, non recursive, this fct is used a lot */
long ral_mark_upslope_cells(pour_point_t *pp, ral_rcoords_t * rc, int m)
{
    long size = 0;
    ral_cell_t c = {rc->y, rc->x};
    ral_cell_t root = c;
    while (1) {
	ral_cell_t up;
	int go_up = 0;
	int dir;
	RAL_DIRECTIONS(dir) {
	    up = ral_cell_move(c, dir);
	    if (GCin(pp->fdg, up) AND 
		GCivD(pp->fdg, up) AND 
		GCiv(pp->fdg, up) == RAL_INV_DIR(dir)) {
		if (!GCiv(pp->mark, up)) {
		    go_up = 1;
		    break;
		}
	    }
	}
	if (go_up)
	    c = up;
	else {
	    GCiv(pp->mark, c) = m;

	    /* remember the topmost cell (min i) so that direction is away from the area */
	    if (!pp->bc_found OR c.y < pp->bc.y) {
		pp->bc_found = 1;
		pp->bc = c;
		pp->dir_out = RAL_N;
	    }
    
	    size++;
	    if SAME_CELL(c, root) return size;
	    c = ral_cell_move(c, GCiv(pp->fdg, c));
	}
    }
}

int ral_fdg_catchment(ral_grid_t *fdg, ral_grid_t *mark, ral_rcoords_t * rc, int m)
{
    ral_cell_t c = {rc->y, rc->x};
    pour_point_t pp;
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, NULL, mark));
    if (!(GCin(fdg, c))) return 0;
    ral_mark_upslope_cells(&pp, ral_rcoords_new(c.x, c.y), m);
    return 1;
fail:
    return 0;
}

ral_grid_t *ral_fdg_depressions(ral_grid_t *fdg, int inc_m, ral_error_t **e)
{
    long m = 1;
    ral_cell_t c;
    ral_grid_t *mark = NULL;
    pour_point_t pp;
    FAIL_UNLESS(mark = ral_grid_new_like(fdg, integer, e));
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, NULL, mark));
    FOR(c, fdg) {
	if (GCiv(fdg, c) == RAL_PIT_CELL) {
	    ral_mark_upslope_cells(&pp, ral_rcoords_new(c.x, c.y), m);
	    if (inc_m) m++;
	    ASSERT(m < RAL_INTEGER_MAX, IOB);
	}
    }
    return mark;
fail:
    ral_delete(mark);
    return NULL;
}

int fillpit(pour_point_t *pp, ral_cell_t c, double zmax)
{
    int size = 1;
    int dir, invdir = 5;
    ral_grid_set(pp->dem, ral_rcoords_new(c.x, c.y), ral_real_new(zmax), NULL);
    RAL_DIRECTIONS(dir) {
	ral_cell_t t = ral_cell_move(c, dir);
	double zt;
	if (!GCin(pp->dem, t) OR !GCivD(pp->fdg, t))
	    continue;
	zt = GCv(pp->dem, t);
	if (GCiv(pp->fdg, t) == invdir AND zt < zmax)
	    size += fillpit(pp, t, zmax);
	invdir++;
	if (invdir > 8) invdir = 1;
    }
    return size;
}

int ral_dem_fill_depressions(ral_grid_t *dem, ral_grid_t *fdg, ral_error_t **e)
{
    ral_cell_t c;
    int pits_filled = 0;
    pour_point_t pp;
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, dem, ral_grid_new_like(fdg, integer, e)));
    ral_integer_t * fill = ral_integer_new(0);
    FOR(c, fdg) {
	if (GCiv(fdg, c) == RAL_PIT_CELL) {

	    double zmax;

	    pp.bc_found = 0;
	    ral_mark_upslope_cells(&pp, ral_rcoords_new(c.x, c.y), 1);

	    pp.pp_found = 0;
	    pp.pour_to_no_data = 0;
	    FAIL_UNLESS(borderwalk(pp.bc, pp.dir_out, &pp, &ral_is_marked, &test_pour_point));

	    ral_grid_floodfill(pp.mark, NULL, ral_rcoords_new(c.x, c.y), fill, 8);

	    zmax = GCv(dem, pp.ipp);
	    if (!pp.pour_to_no_data) zmax = MAX(zmax, GCv(dem, pp.opp));
	    fillpit(&pp, c, zmax);
	    
	    ASSERT(pits_filled < INT_MAX, IOB);
            pits_filled++;
	} 
    }
    ral_delete(fill);
    ral_delete(pp.mark);
    return pits_filled;
 fail:
    ral_delete(pp.mark);
    return -1;
}

/* fdg should have only pits and valid dirs */
int ral_dem_breach(ral_grid_t *dem, ral_grid_t *fdg, int limit, ral_error_t **e)
{
    ral_cell_t c;
    pour_point_t pp;
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, dem, ral_grid_new_like(fdg, integer, e)));
    ral_integer_t * fill = ral_integer_new(0);
    FOR(c, fdg) {
	if (GCiv(fdg, c) == RAL_PIT_CELL) {

	    double z1 = GCv(dem, c), z2, l_in, l_out, l;
	    ral_cell_t flow;
	    int dir, b_count = 0;

	    pp.bc_found = 0;
	    ral_mark_upslope_cells(&pp, ral_rcoords_new(c.x, c.y), 1);

	    pp.pp_found = 0;
	    pp.pour_to_no_data = 0;
	    FAIL_UNLESS(borderwalk(pp.bc, pp.dir_out, &pp, &ral_is_marked, &test_pour_point));
	    ral_grid_floodfill(pp.mark, NULL, ral_rcoords_new(c.x, c.y), fill, 8);

	    /* The breach should now be done from ipp to the pit cell
	       (elev = z1) and from opp to the border of data or to
	       cell with elev same or lower than pit elev (elev =
	       z2). Take z1 and z2 and interpolate z's along the
	       breach. */

	    flow = pp.ipp;
	    l_in = 0;
	    while (!SAME_CELL(flow, c)) {
		if (GCv(dem, flow) > z1 + pp.dz) b_count++;
		l_in += RAL_DISTANCE_UNIT(GCiv(fdg, flow));
		flow = FLOW(fdg, flow);
	    }
	    
	    if (pp.pour_to_no_data) {
		z2 = GCv(dem, pp.ipp);
		l_out = 0;
	    } else {
		l_out = RAL_DISTANCE_UNIT(pp.in2out);
		flow = pp.opp;
		b_count++;
		while (1) {
		    z2 = GCv(dem, flow);
		    if (z2 <= z1 + pp.dz) break;
		    flow = FLOW(fdg, flow);
		    b_count++;
		    /* stop searching for the end of breaching if */
		    if (GCout(fdg, flow) OR GCND(fdg, flow) OR GCiv(fdg, flow) < 1) break;
		    l_out += RAL_DISTANCE_UNIT(GCiv(fdg, flow));
		}
	    }

	    /* give up if breaching would require too many cells to be
	       breached or z2 > z1 and not drain to no_data */
	    if (limit > 0 AND b_count > limit) continue;
	    if (z1 < z2-pp.dz AND GCin(fdg, flow) AND GCD(fdg, flow)) continue;
	    
	    flow = pp.ipp;
	    dir = pp.in2out;
	    l = 0;
	    if (SAME_CELL(flow, c)) {
		GCiv(fdg, flow) = dir;
	    } else {
		while (!SAME_CELL(flow, c)) {
		    int tmp = GCiv(fdg, flow);
		    ral_cell_t down = FLOW(fdg, flow);
		    
		    double z = z1 < z2-pp.dz ? z1 : z1 - (z1-z2)*(l_in-l)/(l_in+l_out);
		    ral_grid_set(dem, ral_rcoords_new(flow.x, flow.y), ral_real_new(z), NULL);
		    
		    /* invert the path */
		    GCiv(fdg, flow) = dir;
		    dir = RAL_INV_DIR(tmp);
		    
		    l += RAL_DISTANCE_UNIT(GCiv(fdg, flow));
		    flow = down;
		}
	    }
		     
	    if (!pp.pour_to_no_data) {
		l = RAL_DISTANCE_UNIT(pp.in2out);
		flow = pp.opp;
		while (1) {
		    double old_z = GCv(dem, flow), z;
		    if (old_z <= z1 + pp.dz) break;
		    z = z1 < z2-pp.dz ? z1 : z1 - (z1-z2)*(l_in+l)/(l_in+l_out);
		    ral_grid_set(dem, ral_rcoords_new(flow.x, flow.y), ral_real_new(z), NULL);
		    flow = FLOW(fdg, flow);
		    /* stop breaching if */
		    if (GCout(fdg, flow) OR GCND(fdg, flow) OR GCiv(fdg, flow) < 1) break;
		    l += RAL_DISTANCE_UNIT(GCiv(fdg, flow));
		}
	    }

	} 
    }
    ral_delete(fill);
    ral_delete(pp.mark);
    return 1;
 fail:
    ral_delete(pp.mark);
    return 0;
}

int ral_fdg_drain_depressions(ral_grid_t *fdg, ral_grid_t *dem, ral_error_t **e) 
{
    ral_cell_t c;
    int fixed_pits = 0;
    pour_point_t pp;
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, dem, ral_grid_new_like(fdg, integer, e)));
    ral_integer_t * fill = ral_integer_new(0);
    FOR(c, fdg) {
	if (GCiv(fdg, c) != RAL_PIT_CELL) continue;
	
	pp.bc_found = 0;
	ral_mark_upslope_cells(&pp, ral_rcoords_new(c.x, c.y), 1);
	
	/* walk the border, start from border set by mark_upslope_cells (min i) */
	pp.pp_found = 0;
	pp.pour_to_no_data = 0;
	FAIL_UNLESS(borderwalk(pp.bc, pp.dir_out, &pp, &ral_is_marked, &test_pour_point));

	if (!SAME_CELL(pp.ipp, c)) {
	    /* invert the path from the pour point to the pit point */
	    /* from pour_point to pit */
	    ral_cell_t a = pp.ipp;
	    int da = GCiv(pp.fdg, a);
	    ral_cell_t b = ral_cell_move(a, da);
	    int db = GCiv(pp.fdg, b);
	    while (db) {
		GCiv(pp.fdg, b) = RAL_INV_DIR(da);
		a = b;
		da = db;
		b = ral_cell_move(a, da);
		db = GCiv(pp.fdg, b); 
	    }
	    GCiv(pp.fdg, b) = RAL_INV_DIR(da);
	}

	GCiv(pp.fdg, pp.ipp) = pp.in2out;
	ral_grid_floodfill(pp.mark, NULL, ral_rcoords_new(c.x, c.y), fill, 8);
	fixed_pits++;
    }
    ral_delete(fill);
    ral_delete(pp.mark);
    return fixed_pits;
 fail:
    ral_delete(pp.mark);
    return -1;
}

#define RAL_ON_PATH(fdg, c, stop)               \
    (GCin((fdg), (c)) AND			\
     GCivD((fdg), (c)) AND                      \
     GCiv((fdg), (c)) > 0 AND                   \
     GCiv((fdg), (c)) < 9 AND                   \
     (!(stop) OR (GCD((stop), (c)) AND          \
		  GCv((stop), (c)) <= 0)))

ral_grid_t *ral_fdg_path(ral_grid_t *fdg, ral_rcoords_t * rc, ral_grid_t *stop, ral_error_t **e)
{
    ral_cell_t c = {rc->y, rc->x};
    ral_grid_t *path = NULL;
    ASSERT((!stop || ral_grid_overlayable(fdg, stop)) &&
           fdg->type == integer, BAD);
    FAIL_UNLESS(path = ral_grid_new_like(fdg, integer, e));
    ral_grid_set_all(path, ral_undef, NULL);
    while (RAL_ON_PATH(fdg, c, stop)) {	
	GCiv(path, c) = 1;
	c = FLOW(fdg, c);        
    }
    return path;
fail:
    ral_delete(path);
    return NULL;
}

ral_grid_t *ral_fdg_path_length(ral_grid_t *fdg, ral_grid_t *stop, ral_grid_t *op)
{
    ral_grid_t *path_length = NULL;
    ral_cell_t c;
    ASSERT((!stop || ral_grid_overlayable(fdg, stop)) &&
           (!op || ral_grid_overlayable(fdg, op)) &&
           fdg->type == integer, BAD);
    FAIL_UNLESS(path_length = ral_grid_new_like(fdg, real, NULL));
    ral_grid_set_all(path_length, ral_undef, NULL);

    FOR(c, fdg) {

        int prev_dir = -1;
        double length = 0;
        ral_cell_t d = c;

        if (GCivND(fdg, c) OR
            GCiv(fdg, c) < 1 OR
            GCiv(fdg, c) > 8)
            continue;
	
        while (RAL_ON_PATH(fdg, d, stop)) {

            /* within d but two directions */
            if (!op OR (GCD(op, d))) {
		    
                if (prev_dir > 0)
                    length += RAL_DISTANCE_UNIT(prev_dir) / 2.0;
	    
                length += RAL_DISTANCE_UNIT(GCiv(fdg, d)) / 2.0;

            }

            prev_dir = GCiv(fdg, d);
            d = FLOW(fdg, d);

        }

        if (!op OR (GCin(op, d) AND GCD(op, d)))
	    
            length += RAL_DISTANCE_UNIT(prev_dir) / 2.0;

        GCnv(path_length, c) = length * fdg->cell_size;

    }

    return path_length;
fail:
    ral_delete(path_length);
    return NULL;
}

ral_grid_t *ral_fdg_path_sum(ral_grid_t *fdg, ral_grid_t *stop, ral_grid_t *op)
{
    ral_grid_t *path_sum = NULL;
    ral_cell_t c;
    ASSERT((!stop || ral_grid_overlayable(fdg, stop)) &&
           ral_grid_overlayable(fdg, op) &&
           fdg->type == integer, BAD);
    FAIL_UNLESS(path_sum = ral_grid_new_like(fdg, real, NULL));
    ral_grid_set_all(path_sum, ral_undef, NULL);

    FOR(c, fdg) {

	int prev_dir = -1;
	double sum = 0;
	ral_cell_t d = c;

	if (GCivND(fdg, c) OR
	    GCiv(fdg, c) < 1 OR
	    GCiv(fdg, c) > 8)
	    continue;
	
	while (RAL_ON_PATH(fdg, d, stop)) {
	    
	    /* within d but two directions */
	    if (GCD(op, d)) {
		
		if (prev_dir > 0)
		    sum += GCv(op, d) * RAL_DISTANCE_UNIT(prev_dir) / 2.0;
	    
		sum += GCv(op, d) * RAL_DISTANCE_UNIT(GCiv(fdg, d)) / 2.0;

	    }
	    
	    prev_dir = GCiv(fdg, d);
	    d = FLOW(fdg, d);
	    
	}

	if (GCin(op, d) AND GCD(op, d))
	    
	    sum += GCv(op, d) * RAL_DISTANCE_UNIT(prev_dir) / 2.0;

	GCnv(path_sum, c) = sum;

    }

    return path_sum;
 fail:
    ral_delete(path_sum);
    return NULL;
}

ral_grid_t *ral_fdg_upslope_sum(ral_grid_t *fdg, ral_grid_t *op, int include_self)
{
    ral_grid_t *sum_grid = NULL;
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(fdg, op) &&
           fdg->type == integer, BAD);
    FAIL_UNLESS(sum_grid = ral_grid_new_like(fdg, real, NULL));
    ral_grid_set_all(sum_grid, ral_undef, NULL);
    FOR(c, fdg) {
	ral_cell_t d;
	/* no_data or already computed? */
	if (!GCivD(fdg, c) OR GCnvD(sum_grid, c))
	    continue;
	/* visit all upslope cells with d and compute the sum */
	d = c;
	while (1) {
	    ral_cell_t up;
	    double sum = 0;
	    /* can we compute the sum from immediately upstream cells? */
	    int go_up = 0;
	    int dir;
	    RAL_DIRECTIONS(dir) {
		up = ral_cell_move(d, dir);
		if (GCin(fdg, up) AND 
		    GCivD(fdg, up) AND 
		    GCiv(fdg, up) == RAL_INV_DIR(dir)) {
		    if (GCnvND(sum_grid, up)) {
			/* at least one upslope cell has non-resolved upslope sum */
			go_up = 1;
			break;
		    } else {
			sum += GCnv(sum_grid, up);
			if (!include_self AND GCD(op, up))
			    sum += GCv(op, up);
		    }
		}
	    }
	    if (go_up)
		d = up;
	    else {
		if (include_self AND GCD(op, d))
		    sum += GCv(op, d);
		GCnv(sum_grid, d) = sum;
		if SAME_CELL(d, c) break;
		d = FLOW(fdg, d);
	    }
	}
    }
    return sum_grid;
 fail:
    ral_delete(sum_grid);
    return NULL;
}

ral_grid_t *ral_fdg_upslope_count(ral_grid_t *fdg, ral_grid_t *op, int include_self)
{
    ral_grid_t *count_grid = NULL;
    ral_cell_t c;
    ASSERT((!op || ral_grid_overlayable(fdg, op)) && fdg->type == integer, BAD);
    FAIL_UNLESS(count_grid = ral_grid_new_like(fdg, real, NULL));
    ral_grid_set_all(count_grid, ral_undef, NULL);
    FOR(c, fdg) {
	ral_cell_t d;
	/* no_data or already computed? */
	if (!GCivD(fdg, c) OR GCnvD(count_grid, c))
	    continue;
	/* visit all upslope cells with d and compute the sum */
	d = c;
	while (1) {
	    ral_cell_t up;
	    double count = 0;
	    /* can we compute the count from immediately upstream cells? */
	    int go_up = 0;
	    int dir;
	    RAL_DIRECTIONS(dir) {
		up = ral_cell_move(d, dir);
		if (GCin(fdg, up) AND 
		    GCivD(fdg, up) AND 
		    GCiv(fdg, up) == RAL_INV_DIR(dir)) {
		    if (GCnvND(count_grid, up)) {
			/* at least one upslope cell has non-resolved upslope count */
			go_up = 1;
			break;
		    } else {
			count += GCnv(count_grid, up);
			if (!include_self AND (!op OR GCD(op, up)))
			    count += 1;
		    }
		}
	    }
	    if (go_up)
		d = up;
	    else {
		if (include_self) {
		    if (!op OR GCD(op, d)) 
			count += 1;
		}
		GCnv(count_grid, d) = count;
		if SAME_CELL(d, c) break;
		d = FLOW(fdg, d);
	    }
	}
    }
    return count_grid;
 fail:
    ral_delete(count_grid);
    return NULL;
}

ral_grid_t *ral_water_route(ral_grid_t *water, ral_grid_t *dem, ral_grid_t *fdg, ral_grid_t *k, double r)
{
    ral_cell_t c;
    ral_grid_t *f = NULL;
    ASSERT(ral_grid_overlayable(water, dem) AND 
           (!fdg OR ral_grid_overlayable(water, fdg)) AND 
           ral_grid_overlayable(water, k) && 
           water->type == real AND 
           (!fdg OR (fdg->type == integer)) AND 
           (k->type == real), BAD);
    FAIL_UNLESS(f = ral_grid_new_like(water, real, NULL));

    FOR(c, water) {

	double S = GCnv(water, c);

	if (fdg AND GCivD(fdg, c)) {
	    
	    ral_cell_t down = ral_cell_move(c, GCiv(fdg, c));

	    if (GCout(fdg, down) OR GCivND(fdg, down))
		GCnv(f, c) -= S;
	    else {
		double u = RAL_DISTANCE_UNIT(GCiv(fdg, c));
		double slope = r * (GCv(dem, c) - GCv(dem, down)) / (dem->cell_size * u);
		double dS = GCnv(k, c) * sqrt(slope) * S;
		GCnv(f, c) -= dS;
		GCnv(f, down) += dS;
	    }

	} else if (GCD(dem, c)) {

	    int dir;
	    for (dir = 1; dir < 9; dir += 2) { /* N, E, S, W */
		ral_cell_t down = ral_cell_move(c, dir);
		if (GCout(water, down) OR GCivND(water, down))
		    GCnv(f, c) -= S;
		else {
		    double slope = r * (GCv(dem, c) - GCv(dem, down)) / dem->cell_size;
		    double dS = GCnv(k, c) * sqrt(slope) * S;
		    if (slope > 0) {
			GCnv(f, c) -= dS;
			GCnv(f, down) += dS;
		    }
		}
	    }
	}
    }
    return f;
 fail:
    if (f) ral_delete(f);
    return NULL;
}

int ral_find_flats(slopes_t *pp) 
{
    ral_cell_t c;
    FOR(c, pp->fdg) {
	if (GCD(pp->fdg, c)) {
	    double zc = GCv(pp->dem, c);
	    int dir, is_flat = 1;
	    RAL_DIRECTIONS(dir) {
		ral_cell_t t = ral_cell_move(c, dir);
		double zt = GCv(pp->dem, t);
		/* this marks pits as flat but there shouldn't be any pits */
		if (GCin(pp->fdg, t) AND GCD(pp->fdg, t) AND zt < zc - pp->dz) {
		    is_flat = 0;
		    break;
		}
	    }
	    GCiv(pp->flat, c) = is_flat;
	} else {
	    ral_grid_set(pp->flat, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
	}
    }
    return 1;
}

int ral_slope_sums(slopes_t *pp)
{
    ral_cell_t c;
    FOR(c, pp->fdg) {
	if (GCD(pp->fdg, c)) {
	    double zc = GCv(pp->dem, c);
	    double slope_sum = 0;
	    int dir;
	    RAL_DIRECTIONS(dir) {
		ral_cell_t t = ral_cell_move(c, dir);
		double zt;
		if (GCin(pp->fdg, t) AND 
		    GCivD(pp->fdg, t) AND
		    (zt = GCv(pp->dem, t)) < zc - pp->dz)
		    slope_sum += (zc - zt)/RAL_DISTANCE_UNIT(dir);
	    }
	    GCnv(pp->ss, c) = slope_sum;
	} else {
	    ral_grid_set(pp->ss, ral_rcoords_new(c.x, c.y), ral_undef, NULL);
	}
    }
    return 1;
}

int cuag2(slopes_t *pp, ral_cell_t c, int recursion) 
{
    int dir, invdir = 5;
    double ua = 1; 
    /* upslope area is 1 + that part of higher lying cells that comes
       to this cell

       if this cell is on flat area (i.e. the lowest 8-neighbor has
       the same z as this) then use the fdg to find the upslope cells
       and portion is 1 if the upslope cell is flat otherwise use the
       portion coeff */
    double zc = GCv(pp->dem, c);
    RAL_DIRECTIONS(dir) {
	ral_cell_t t = ral_cell_move(c, dir);
	if (GCin(pp->fdg, t) AND GCivD(pp->fdg, t)) {
	    double zt = GCv(pp->dem, t);
	    if (fabs(zc - zt) <= pp->dz AND GCiv(pp->flat, t) AND GCiv(pp->fdg, t) == invdir) {
		if (GCnv(pp->uag, t) > 0 OR !recursion) {
		    ua += GCnv(pp->uag, t);
		} else {
		    ua += GCnv(pp->uag, t) = cuag2(pp, t, 1);
		}
	    } else if (zt > zc + pp->dz) {
		if (GCnv(pp->uag, t) > 0 OR !recursion)
		    ua += ((zt - zc)/RAL_DISTANCE_UNIT(dir))/GCnv(pp->ss, t)*GCnv(pp->uag, t);
		else {
		    GCnv(pp->uag, t) = cuag2(pp, t, 1);
		    ua += ((zt - zc)/RAL_DISTANCE_UNIT(dir))/GCnv(pp->ss, t)*GCnv(pp->uag, t);
		}
	    }
	}
	invdir++;
	if (invdir > 8) invdir = 1;
    }
    return ua;
}

void mark_basin(pour_point_t *pp, ral_grid_t *streams, ral_cell_t c, int m) 
{
    ral_cell_t c0 = c;
    int d = GCiv(pp->fdg, c); /* direction at current cell */
    int n = 1;                /* nr. of possible directions to go */
    ral_cell_t c2 = c;              /* moving cursor */
    int d2 = d; 
    /* seek to the end of this stream */
    while (n == 1) {
	int dt = d2; /* test direction */
	dt = RAL_NEXT_DIR(dt);
	n = 0;
	c = c2;
	d = d2;
	while (dt != d) { /* test all directions */
	    ral_cell_t t = c; /* test cell */
	    int di = RAL_INV_DIR(dt); /* upstream cell has this fd */
	    t = ral_cell_move(t, dt);
	    if (GCin(pp->fdg, t) AND 
		GCivD(streams, t) AND GCiv(streams, t) AND GCiv(pp->fdg, t) == di) {
		n++;
		c2 = t;
		d2 = di;
	    }
	    dt++;
	    if (dt > 8) {
		dt = 1;
		if (d == 0) break;
	    }
	}
    }
    if (n > 1) { /* junction, recurse to all upstream streams */
	int dt = RAL_NEXT_DIR(d);
	while (dt != d) { /* test all directions */
	    ral_cell_t t = c; /* test cell */
	    int di = RAL_INV_DIR(dt); /* upstream cell has this fd */
	    t = ral_cell_move(t, dt);
	    if (GCin(pp->fdg, t) AND 
		GCivD(streams, t) AND GCiv(streams, t) AND 
		GCiv(pp->fdg, t) == di) {
		mark_basin(pp, streams, t, m);
	    }
	    dt++;
	    if (dt > 8) {
		dt = 1;
		if (d == 0) break;
	    }
	}
    }
    /* now mark this subcatchment and we are done */
    ral_mark_upslope_cells(pp, ral_rcoords_new(c0.x, c0.y), m);
}

ral_grid_t *ral_streams_subcatchments(ral_grid_t *streams, ral_grid_t *fdg, ral_rcoords_t * rc, ral_error_t **e) 
{
    ral_cell_t c = {rc->y, rc->x};
    pour_point_t pp;
    ASSERT(streams->type == integer &&
           fdg->type == integer &&
           GCin(streams, c) &&
           GCiv(streams, c) != 0, BAD);
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, NULL, ral_grid_new_like(fdg, integer, e)));
    mark_basin(&pp, streams, c, 1);
    return pp.mark;
fail:
    return NULL;
}

ral_grid_t *ral_streams_subcatchments2(ral_grid_t *streams, ral_grid_t *fdg, ral_error_t **e)
{
    int m = 1;
    ral_cell_t c;
    pour_point_t pp;
    ASSERT(streams->type == integer &&
           fdg->type == integer, BAD);
    FAIL_UNLESS(ral_init_pour_point_struct(&pp, fdg, NULL, ral_grid_new_like(fdg, integer, e)));
    FOR(c, fdg) {
	if (GCivD(streams, c) AND fdg_is_outlet(fdg, streams, c)) {
	    mark_basin(&pp, streams, c, m);
            m++;
        }
    }
    return pp.mark;
 fail:
    return NULL;
}

int upstream_cells_of(ral_grid_t *streams, ral_grid_t *fdg, ral_cell_t c, ral_cell_t u[]) 
{
    int n = 0;
    int dir;
    RAL_DIRECTIONS(dir) {
	ral_cell_t t = ral_cell_move(c, dir);
	if (GCin(streams, t) AND GCivD(streams, t) AND 
	    GCiv(streams, t) AND GCiv(fdg, t) == RAL_INV_DIR(dir)) {
	    u[n] = t;
	    n++;
	}
    }
    return n;
}

int number_tree(ral_grid_t *streams, ral_grid_t *fdg, ral_cell_t c, int k) 
{
    while (1) {
	/* move along the stream from b onwards */    
	int n;
	ral_cell_t u[8];  /* upstream cells */
	GCiv(streams, c) = k;
	n = upstream_cells_of(streams, fdg, c, u);

	if (n == 0) break; /* b is the end cell of the stream */

	if (n == 1) { /* b is a stream cell */
	    c = u[0];

	} else { /* a cell where two or more streams join */
	    int i;
	    for (i = 0; i < n; i++)
		k = number_tree(streams, fdg, u[i], k+1);
	    break;
	}

    }
    return k;
}

void mark_tree(ral_grid_t *streams, ral_grid_t *fdg, ral_cell_t c, int index) 
{
  while (1) {
    /* move up the stream from c onwards */    
    int n;
    ral_cell_t u[8];  /* upstream cells */
    GCiv(streams, c) = index;
    n = upstream_cells_of(streams, fdg, c, u);
    
    if (n == 0) break; /* c is the end cell of the stream */
    
    if (n == 1) { /* c is a stream cell */
      c = u[0];
      
    } else { /* a cell where two or more streams join */
      int i;
      for (i = 0; i < n; i++)
	mark_tree(streams, fdg, u[i], index);
      break;
    }
  }
}

int ral_streams_number(ral_grid_t *streams, ral_grid_t *fdg, ral_rcoords_t * rc, int sid0) 
{
    ral_cell_t c = {rc->y, rc->x};
    ASSERT(ral_grid_overlayable(streams, fdg) &&
           fdg->type == integer &&
           streams->type == integer &&
           GCin(streams, c), BAD);
    number_tree(streams, fdg, c, sid0);
    return 1;
 fail:
    return 0;
}

int ral_streams_number2(ral_grid_t *streams, ral_grid_t *fdg, int sid0) 
{
    ral_cell_t c;
    ASSERT(ral_grid_overlayable(streams, fdg) &&
           fdg->type == integer &&
           streams->type == integer, BAD);
    FOR(c, fdg) {
	if (fdg_is_outlet(fdg, streams, c))
	    sid0 = number_tree(streams, fdg, c, sid0)+1;
    }
    return 1;
 fail:
    return 0;
}

int new_streams_vector(ral_grid_t *streams, ral_grid_t *fdg, ral_cell_t c, int k,
                          OGRFeatureH hFeat, OGRGeometryH hGeom, OGRLayerH hLayer) 
{
    int n;
    double dist, maxdist = 1000000000000.0;
    ral_wcoords_t * p;
    ral_cell_t d, u[8];  /* upstream cells */

    dist = RAL_DISTANCE_UNIT(GCiv(fdg, c))*ral_grid_get_cell_size(streams);  
    while (1) {
        /* move along the stream from c onwards */    
        p = ral_grid_cell2point(streams, ral_rcoords_new(c.x, c.y));
        OGR_G_AddPoint_2D(hGeom, p->x, p->y);
        dist += RAL_DISTANCE_UNIT(GCiv(fdg, c))*ral_grid_get_cell_size(streams);
        GCiv(streams, c) = k;
        n = upstream_cells_of(streams, fdg, c, u);
    
        if (n == 0) { /* b is the end cell of the stream */
            OGR_F_SetGeometry(hFeat, hGeom); 
            OGR_L_CreateFeature(hLayer, hFeat);  
            OGR_G_DestroyGeometry(hGeom);  
            OGR_F_Destroy(hFeat);
            break; 
        }    

        if (n == 1) { /* b is a stream cell */
            c = u[0];
            if (dist > maxdist) {
                k++;
                OGR_F_SetGeometry(hFeat, hGeom); 
                OGR_L_CreateFeature(hLayer, hFeat);  
                OGR_G_DestroyGeometry(hGeom);  
                OGR_F_Destroy(hFeat);
                hFeat = OGR_F_Create(OGR_L_GetLayerDefn(hLayer)); 
                OGR_F_SetFieldInteger(hFeat, 0, k);
                d = FLOW(fdg, c);
                OGR_F_SetFieldInteger(hFeat, 1, GCiv(streams, d));
                hGeom = OGR_G_CreateGeometry(wkbLineString);
                OGR_G_AddPoint_2D(hGeom, p->x, p->y);
                dist = RAL_DISTANCE_UNIT(GCiv(fdg, c))*ral_grid_get_cell_size(streams);
            }
      
        } else { /* a cell where two or more streams join */
            int i;
            OGR_F_SetGeometry(hFeat, hGeom); 
            OGR_L_CreateFeature(hLayer, hFeat);  
            OGR_G_DestroyGeometry(hGeom);  
            OGR_F_Destroy(hFeat);
            for (i = 0; i < n; i++) {
                hFeat = OGR_F_Create(OGR_L_GetLayerDefn(hLayer)); 
                OGR_F_SetFieldInteger(hFeat, 0, k+1);
                d = FLOW(fdg, c);
                OGR_F_SetFieldInteger(hFeat, 1, GCiv(streams, d)); 
                hGeom = OGR_G_CreateGeometry(wkbLineString);
                OGR_G_AddPoint_2D(hGeom, p->x, p->y);
                k = new_streams_vector(streams, fdg, u[i], k+1, hFeat, hGeom, hLayer);
            }
            break;
        }
        ral_delete(p);
    }
    return k;
}

int ral_streams_vectorize(ral_grid_t *streams, ral_grid_t *fdg, int row, int col)
{
  const char *pszDriverName = "ESRI Shapefile";
  ral_cell_t c;
  ral_grid_t *str;
  OGRSFDriverH hDr;
  OGRDataSourceH hDS;
  OGRLayerH hLayer;
  OGRFieldDefnH hFldDefn;
  OGRFeatureH hFeat;
  OGRGeometryH hGeom;

  ASSERT(ral_grid_overlayable(streams, fdg), BAD);
  str = ral_grid_new_copy(streams, integer, NULL);
  c.y = row;
  c.x = col;
  OGRRegisterAll();
  hDr = OGRGetDriverByName(pszDriverName);
  /* CHECK IF DRIVER IS NULL */
  hDS = OGR_Dr_CreateDataSource(hDr, "c:\\testOGR.shp", NULL);
  /*CHECK IF DATASOURCE IS NULL*/
  hLayer = OGR_DS_CreateLayer(hDS, "test", NULL, wkbUnknown, NULL);
  /*CHECK IF LAYER IS NULL*/
  hFldDefn = OGR_Fld_Create("StreamID", OFTInteger);
  OGR_Fld_SetWidth(hFldDefn, 32);
  OGR_L_CreateField(hLayer, hFldDefn, TRUE);
  hFldDefn = OGR_Fld_Create("FlowsTo", OFTInteger);
  OGR_Fld_SetWidth(hFldDefn, 32);
  OGR_L_CreateField(hLayer, hFldDefn, TRUE);
  /*CHECK IF  RETURNED OGREEROR TELLS SOMETHINGI*/
  hFeat = OGR_F_Create(OGR_L_GetLayerDefn(hLayer));
  OGR_F_SetFieldInteger(hFeat, 0, 1); 
  OGR_F_SetFieldInteger(hFeat, 1, 0); 
  hGeom = OGR_G_CreateGeometry(wkbLineString);
  new_streams_vector(str, fdg, c, 1, hFeat, hGeom, hLayer);

  ral_delete(str);
  OGR_DS_Destroy(hDS);
  return 1;

 fail:
  /* if (str) ral_gddestroy(str);*/
  return 0;
}

int rprune(lakedata_t *ld, ral_cell_t a, double from_origin_to_a, int *was_pruned);

/* borderwalk do-your-thing functions for lakedata struct: */

int ral_prunelake(ral_cell_t c, void *param) 
{
    lakedata_t *ld = (lakedata_t *)param;
    int dir;

    if (GCiv(ld->mark, c)) return 1;
    GCiv(ld->mark, c) = 1;
    
    /* neighboring non-lake stream cells */
    RAL_DIRECTIONS(dir) {

	ral_cell_t t = ral_cell_move(c, dir);

	if (GCout(ld->lakes, t) OR (GCiv(ld->lakes, t) == ld->lid)) continue;

	if (GCivD(ld->streams, t) AND GCiv(ld->streams, t)) {

	    /* does the stream cell flow into this cell? */
	    
	    if (SAME_CELL(c, FLOW(ld->fdg, t))) {
		
		/* the test cell flows into the lake, prune this stream */
		FAIL_UNLESS(rprune(ld, t, RAL_DISTANCE_UNIT(GCiv(ld->fdg, t)) * ld->fdg->cell_size, NULL));
                
	    }
	    
	}
    }
    return 1;
 fail:
    return 0;
}

int ral_lakearea(ral_cell_t c, void *param)
{
   return GCin(((lakedata_t *)param)->lakes, c) AND 
	GCivD(((lakedata_t *)param)->lakes, c) AND 
	GCiv(((lakedata_t *)param)->lakes, c) == ((lakedata_t *)param)->lid;
}

/* 
   renumber the stream upstream from cell c with id id 
   stop at a lake if it is given 
*/
void renumber_stream(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, ral_cell_t c, int id) 
{
    while (1) {
	/* move along the arc from c onwards */    
	int k;
	ral_cell_t u[8]; /* upstream cells */
	GCiv(streams, c) = id;
	k = upstream_cells_of(streams, fdg, c, u);
	if (k == 0) break; /* c is an end cell of a stream */
	if (k == 1) {      /* c is a stream cell */
	    if (lakes AND GCivD(lakes, u[0]) AND GCiv(lakes, u[0])) 
		break;
	    c = u[0];
	} else             /* a cell where two or more streams join */
	    break;
    }
}

int rprune(lakedata_t *ld, ral_cell_t a, double from_origin_to_a, int *was_pruned) 
{
    ral_cell_t b = a;
    double l = from_origin_to_a;
    if (was_pruned) *was_pruned = 0;
    if (ld->lakes AND GCivD(ld->lakes, a) AND GCiv(ld->lakes, a)) { /* on a lake */
	int lid = ld->lid;
	ld->lid = GCiv(ld->lakes, a);
	/* look for streams that flow into the lake  */
	if (!borderwalk(a, GCiv(ld->fdg, a), ld, &ral_lakearea, &ral_prunelake)) goto fail;
	ld->lid = lid;
	return 1;
    }
    while (1) {

	/* move along the stream from a onwards */
	int n;

	ral_cell_t u[8];  /* upstream cells */

	ASSERT(!GCiv(ld->mark, b), "Loop detected when pruning a lake. Fail.");

	GCiv(ld->mark, b) = 1;

	n = upstream_cells_of(ld->streams, ld->fdg, b, u);

	if (n == 0) { /* b is the end cell of the stream */

	    if (l < ld->min_l) { /* this stream is too short, prune it */
		ral_cell_t x = b;
		while (1) {
		    GCiv(ld->streams, x) = 0;
		    if (SAME_CELL(x, a)) break;
		    x = FLOW(ld->fdg, x);
		}
		if (was_pruned) *was_pruned = 1;
	    }
	    break;

	} else if (n == 1) { /* b is a regular stream cell */

	    if (ld->lakes AND GCivD(ld->lakes, u[0]) AND GCiv(ld->lakes, u[0])) { 
		/* with a lake cell just upstream */	
		/* backup lid, there may be lakes behind the lakes */
		int lid = ld->lid;
		ld->lid = GCiv(ld->lakes, u[0]);
		/* look for streams that flow into the lake  */
		FAIL_UNLESS(borderwalk(u[0], GCiv(ld->fdg, u[0]), ld, &ral_lakearea, &ral_prunelake));
		ld->lid = lid;
		break;
	    } else {
		l += RAL_DISTANCE_UNIT(GCiv(ld->fdg, u[0])) * ld->fdg->cell_size;
		b = u[0];
	    }

	} else { /* b is a cell where two or more streams join */

	    int i;
	    for (i = 0; i < n; i++) {
		if (ld->lakes AND GCivD(ld->lakes, u[i]) AND GCiv(ld->lakes, u[i])) {
		    /* backup lid, there may be lakes behind the lakes */
		    int lid = ld->lid;
		    ld->lid = GCiv(ld->lakes, u[i]);
		    FAIL_UNLESS(borderwalk(u[i], GCiv(ld->fdg, u[0]), ld, &ral_lakearea, &ral_prunelake));
		    ld->lid = lid;
		} else {
		    double l_zero = RAL_DISTANCE_UNIT(GCiv(ld->fdg, u[i]))*ld->fdg->cell_size;
		    /* join the remaining upstream stream if there is now no junction */
		    int pruned;
		    FAIL_UNLESS(rprune(ld, u[i], l_zero, &pruned));
		    if (pruned AND n == 2) {
			int j = i == 0 ? 1 : 0;
			renumber_stream(ld->streams, ld->fdg, NULL, u[j], GCiv(ld->streams, b));
		    }
		}
	    }
	    break;

	}

    }
    return 1;
 fail:
    return 0;
}

int ral_streams_prune(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, ral_rcoords_t * rc, double min_l, ral_error_t **e) 
{
    ral_cell_t c = {rc->y, rc->x};
    lakedata_t ld;
    ld.lakes = lakes;
    ld.streams = streams;
    ld.fdg = fdg;
    ld.mark = NULL;
    ld.min_l = min_l;
    ASSERT(ral_grid_overlayable(streams, fdg) &&
           (!lakes || ral_grid_overlayable(streams, lakes)) &&
           fdg->type == integer &&
           streams->type == integer &&
           (!lakes || lakes->type == integer) &&
           GCin(streams, c), BAD);
    FAIL_UNLESS(ld.mark = ral_grid_new_like(fdg, integer, e));
    FAIL_UNLESS(rprune(&ld, c, 0, NULL));
    ral_delete(ld.mark);
    return 1;
fail:
    ral_delete(ld.mark);
    return 0;
}

int ral_streams_prune2(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, double min_l, ral_error_t **e)
{
    ral_cell_t c;
    lakedata_t ld;
    ld.lakes = lakes;
    ld.streams = streams;
    ld.fdg = fdg;
    ld.mark = NULL;
    ld.min_l = min_l;
    ASSERT(ral_grid_overlayable(streams, fdg) &&
           (!lakes OR ral_grid_overlayable(streams, lakes)) &&
           fdg->type == integer &&
           streams->type == integer &&
           (!lakes OR lakes->type == integer), BAD);
    FAIL_UNLESS(ld.mark = ral_grid_new_like(fdg, integer, e));
    FOR(c, fdg) {
	if (fdg_is_outlet(fdg, streams, c))
	    FAIL_UNLESS(rprune(&ld, c, 0, NULL));
    }
    ral_delete(ld.mark);
    return 1;
 fail:
    ral_delete(ld.mark);
    return 0;
}

int ral_testlake(ral_cell_t c, void *param)
{
    lakedata_t *ld = (lakedata_t *)param;
    int dir;
    
    /* neighboring non-lake stream cells */
    RAL_DIRECTIONS(dir) {

	ral_cell_t t = ral_cell_move(c, dir);

	if (GCout(ld->lakes, t) OR (GCiv(ld->lakes, t) == ld->lid)) continue;

	if (GCiv(ld->streams, t) == ld->sid) {

	    /* it is a stream cell with same id, 
	       does the stream flow into this lake? */
	    
	    if (SAME_CELL(c, FLOW(ld->fdg, t))) {
		
		/* the test cell flows into the lake, renumber this stream */

		renumber_stream(ld->streams, ld->fdg, NULL, t, ld->nsid);
		ld->nsid++;
		
	    }
	}
    }
    return 1;
}

int ral_streams_break(ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, int nsid) 
{
    ral_cell_t a;
    lakedata_t ld;

    ld.lakes = lakes;
    ld.streams = streams;
    ld.fdg = fdg;
    ld.nsid = nsid;

    ASSERT(ral_grid_overlayable(streams, fdg) &&
           ral_grid_overlayable(streams, lakes) &&
           fdg->type == integer &&
           streams->type == integer &&
           lakes->type == integer, BAD);

    FOR(a, fdg) {
	if (GCivD(lakes, a) AND GCiv(lakes, a) AND 
	    GCivD(streams, a) AND GCiv(streams, a)) {
		
	    /* cells of interest: a stream leaving a lake */

	    ral_cell_t f = FLOW(fdg, a);                   /* the cell into which the stream flows to */
	    if (GCout(fdg, a) OR SAME_CELL(f, a)) 
	      continue;                              /* no flow */
	    ld.lid = GCiv(lakes, a);             /* lake id */
	    ld.sid = GCiv(streams, a);           /* stream id */
	    if (GCiv(lakes, f) == ld.lid) continue; /* the stream is not an outflow */

	    FAIL_UNLESS(borderwalk(a, GCiv(fdg, a), &ld, &ral_lakearea, &ral_testlake));
	}
    }
    return 1;
 fail:
    return 0;
}

int outlet(lakedata_t *ld, ral_cell_t c) 
{
    ral_cell_t f = FLOW(ld->fdg, c);          
    if (GCout(ld->fdg, f))
	return 1;
    return GCiv(ld->lakes, f) != ld->lid;
}

int findoutlet(ral_cell_t c, void *param)
{
    lakedata_t *ld = (lakedata_t *)param;
    if (outlet(ld, c)) {
	double ua = GCv(ld->uag, c);
	if (!ld->outlet_found OR ua > ld->max_ua) {
	    ld->outlet_found = 1;
	    ld->max_ua = ua;
	    ld->outlet = c;
	}
    }
    return 1;
}

void drain_lake_to_outlet(lakedata_t *ld, ral_cell_t c, int dir_for_c) 
{
    int upLimit,downLimit;
    int at_up_4_or_6 = 0;
    int at_down_2_or_8 = 0;
    ral_cell_t t = c;
    
    if (GCiv(ld->lakes, t) != ld->lid) return;

    if (dir_for_c > 0) GCiv(ld->fdg, c) = dir_for_c;

    /* Seek up */
    for (t.y = c.y; t.y >= 0; t.y--) {
	if (GCiv(ld->lakes, t) != ld->lid) {
	    at_up_4_or_6 = 1;
	    break;	    
	}
	GCiv(ld->mark, t) = 1;
	if (t.y < c.y) GCiv(ld->fdg, t) = 5; /* down */
    }
    upLimit = MAX(0,t.y);

    /* Seek down */
    for (t.y = c.y+1; t.y < ld->lakes->height; t.y++) {    
	if (GCiv(ld->lakes, t) != ld->lid) {
	    at_down_2_or_8 = 1;
	    break;
	}
	GCiv(ld->mark, t) = 1;
	GCiv(ld->fdg, t) = 1; /* up */
    }
    downLimit = MIN(ld->lakes->height-1,t.y);

    /* Look at columns right and left */
    /* left */
    if (c.x > 0) {
	int lastBorder = 1;
	t.x = c.x-1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a;
	    if ((t.y == upLimit) AND at_up_4_or_6) {
		dir_for_c = 4;
	    } else if ((t.y == downLimit) AND at_down_2_or_8) {
		dir_for_c = 2;
	    } else {
		dir_for_c = 3;
	    }
	    a = GCiv(ld->lakes, t) == ld->lid AND !GCiv(ld->mark, t);
	    if (lastBorder) {
		if (a) {
		    drain_lake_to_outlet(ld, t, dir_for_c);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }

    /* right */
    if (c.x < (ld->lakes->width - 1)) {
	int lastBorder = 1;
	t.x = c.x+1;
	for (t.y = upLimit; t.y <= downLimit; t.y++) {
	    int a;
	    if ((t.y == upLimit) AND at_up_4_or_6) {
		dir_for_c = 6;
	    } else if ((t.y == downLimit) AND at_down_2_or_8) {
		dir_for_c = 8;
	    } else {
		dir_for_c = 7;
	    }
	    a = GCiv(ld->lakes, t) == ld->lid AND !GCiv(ld->mark, t);
	    if (lastBorder) {
		if (a) {
		    drain_lake_to_outlet(ld, t, dir_for_c);
		    lastBorder = 0;
		}
	    } else if (!a) {
		lastBorder = 1;
	    }
	}
    }
}

int ral_fdg_kill_extra_outlets(ral_grid_t *fdg, ral_grid_t *lakes, ral_grid_t *uag, ral_error_t **e) 
{
    ral_cell_t c;
    lakedata_t ld;

    ld.lakes = lakes;
    ld.fdg = fdg;
    ld.uag = uag;
    ld.mark = NULL;

    ASSERT(ral_grid_overlayable(lakes, fdg) &&
           ral_grid_overlayable(lakes, uag) &&
           fdg->type == integer &&
           lakes->type == integer, BAD);
    FAIL_UNLESS(ld.mark = ral_grid_new_like(fdg, integer, e));

    FOR(c, fdg) {

	/* check all lakes once */
	if (GCivD(lakes, c) AND GCiv(lakes, c) AND !GCiv(ld.mark, c)) {
	    
	    /* cells of interest: flow out of a lake */
	    /* find the one and only outlet: 
	       the one with largest upslope area */

	    ld.lid = GCiv(lakes, c);

	    /* set border cell (bc) */
	    ld.bc = c;
	    while (GCin(lakes, ld.bc) AND 
		   GCivD(lakes, ld.bc) AND 
		   GCiv(lakes, ld.bc) == ld.lid)
		ld.bc.y--;
	    ld.bc.y++;
	    ld.dir_out = 1;

	    ld.outlet_found = 0;
	    FAIL_UNLESS(borderwalk(ld.bc, ld.dir_out, &ld, &ral_lakearea, &findoutlet));
	    WARN_UNLESS(ld.outlet_found, "No outlet found in a lake.");
            
	    /* kill other outlets and mark */
	    drain_lake_to_outlet(&ld, ld.outlet, 0);
	}

    }
    ral_delete(ld.mark);
    return 1;
fail:
    ral_delete(ld.mark);
    return 0;
}

void ral_catchment_destroy(catchment_t **c)
{
    if (*c) {
	if ((*c)->outlet) free((*c)->outlet);
	if ((*c)->down) free((*c)->down);
	free(*c);
	*c = NULL;
    }
}

ral_cell_t ral_catchment_down(catchment_t *c, ral_cell_t outlet)
{
    int i;
    ral_cell_t down = {-1, -1};
    for (i = 0; i < c->n; i++) {
	if (SAME_CELL(outlet, c->outlet[i])) return c->down[i];
    }
    return down;
}

int ral_catchment_add(catchment_t *c, ral_cell_t outlet, ral_cell_t down) 
{
    ral_cell_t test;
    ASSERT(c, BAD);
    if (c->size == 0) {
	c->size = c->delta;
	TEST(c->outlet = CALLOC(c->size, ral_cell_t));
	TEST(c->down = CALLOC(c->size, ral_cell_t));
	c->n = 0;
    } else if (c->n >= c->size) {
	ral_cell_t *tmp;
	c->size += c->delta;
	TEST(tmp = REALLOC(c->outlet, c->size, ral_cell_t));
	c->outlet = tmp;
	TEST(tmp = REALLOC(c->down, c->size, ral_cell_t));
	c->down = tmp;
    }
    /* testing for loops down -> -> outlet */
    if (!SAME_CELL(down, outlet)) {
	do {
	    test = ral_catchment_down(c, down);
	    ASSERT(!SAME_CELL(test, outlet), "Loop detected when adding a catchment. Fail.");
	} while (test.y > 0);
    }
    c->outlet[c->n] = outlet;
    c->down[c->n] = down;
    c->n++;
    return 1;
 fail:
    return 0;
}

int tree(treedata_t *td, ral_cell_t a);
int lake(treedata_t *td, ral_cell_t a);

catchment_t *ral_catchment_new(ral_grid_t *subs, ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, ral_rcoords_t * routlet, int headwaters)
{
    ral_cell_t outlet = {routlet->y, routlet->x};
    treedata_t td = {{NULL, NULL, NULL, 0, 0, 
                      {-1, -1}, 0, 0, 1, 0, {-1, -1}, 0.0, 0.0, 0.0, 0, 0, 0, {-1, -1}, 0}, 
                     NULL, NULL, NULL, NULL, 1, 0};
    catchment_t *catchment = MALLOC(catchment_t);
    TEST(catchment);
    catchment->size = 0;
    catchment->delta = 50;	
    catchment->outlet = NULL;
    catchment->down = NULL;
    td.pp.fdg = fdg;
    td.pp.mark = subs;
    td.catchment = catchment;
    td.subs = subs;
    td.lakes = lakes;
    td.headwaters = headwaters;
    ASSERT(ral_grid_overlayable(subs, streams) &&
           ral_grid_overlayable(subs, fdg) &&
           ral_grid_overlayable(subs, lakes) &&
           subs->type == integer AND 
           streams->type == integer AND
           fdg->type == integer AND
           lakes->type == integer &&
           GCin(subs, outlet), BAD);
    FAIL_UNLESS(streams = ral_grid_new_copy(streams, streams->type, NULL));
    td.streams = streams;
    td.last_stream_section_end = outlet;
    FAIL_UNLESS(tree(&td, outlet));
    ral_mark_upslope_cells(&(td.pp), ral_rcoords_new(outlet.x, outlet.y), td.k);
    FAIL_UNLESS(ral_catchment_add(td.catchment, outlet, outlet));
    ral_delete(streams);
    return catchment;
 fail:
    ral_delete(streams);
    ral_catchment_destroy(&catchment);
    return NULL;
}

catchment_t *ral_catchment_new_complete(ral_grid_t *subs, ral_grid_t *streams, ral_grid_t *fdg, ral_grid_t *lakes, int headwaters)
{
    ral_cell_t c;
    treedata_t td = {{NULL, NULL, NULL, 0, 0, 
                      {-1, -1}, 0, 0, 1, 0, {-1, -1}, 0.0, 0.0, 0.0, 0, 0, 0, {-1, -1}, 0}, 
                     NULL, NULL, NULL, NULL, 1, 0};
    catchment_t *catchment = MALLOC(catchment_t);
    TEST(catchment);
    catchment->size = 0;
    catchment->delta = 50;	
    catchment->outlet = NULL;
    catchment->down = NULL;
    td.pp.fdg = fdg;
    td.pp.mark = subs;
    td.catchment = catchment;
    td.subs = subs;
    td.lakes = lakes;
    td.headwaters = headwaters;
    ASSERT(ral_grid_overlayable(subs, streams) AND 
           ral_grid_overlayable(subs, fdg) AND 
           ral_grid_overlayable(subs, lakes) &&
           subs->type == integer AND 
           streams->type == integer AND
           fdg->type == integer AND
           lakes->type == integer, BAD);
    FAIL_UNLESS(streams = ral_grid_new_copy(streams, streams->type, NULL));
    td.streams = streams;
    FOR(c, fdg) {
	if (fdg_is_outlet(fdg, streams, c)) {
	    td.last_stream_section_end = c;
	    FAIL_UNLESS(tree(&td, c));
	    ral_mark_upslope_cells(&(td.pp), ral_rcoords_new(c.x, c.y), td.k);
	    td.k++;
	    FAIL_UNLESS(ral_catchment_add(td.catchment, c, c));
	}
    }
    ral_delete(streams);
    return catchment;
 fail:
    ral_delete(streams);
    ral_catchment_destroy(&catchment);
    return NULL;
    
}

/* callback for the treedata struct */

int ral_testlake2(ral_cell_t c, void *param)
{
    treedata_t *td = (treedata_t *)param;
    int d;
    
    /* neighboring non-lake stream cells */
    for (d = 1; d < 9; d++) {

	ral_cell_t t = ral_cell_move(c, d);

	if (GCout(td->lakes, t) OR (GCiv(td->lakes, t) == td->lid)) continue;

	if (GCivD(td->streams, t) AND GCiv(td->streams, t) > 0) { 

	    /* it is an unvisited stream cell, 
	       does the stream flow into this lake? */
	    
	    if (SAME_CELL(FLOW(td->pp.fdg, t), c)) {

		/* the test cell flows into the lake */
		
		td->last_stream_section_end = t;
		FAIL_UNLESS(tree(td, t));
		ral_mark_upslope_cells(&(td->pp), ral_rcoords_new(t.x, t.y), td->k);
		td->k++;
		FAIL_UNLESS(ral_catchment_add(td->catchment, t, td->a));
	    }
	    
	}
    }
    return 1;
 fail:
    return 0;
}

int ral_lakearea2(ral_cell_t c, void *param)
{
    treedata_t *td = (treedata_t *)param;
    if (GCout(td->lakes, c) OR GCND(td->lakes, c))
	return 0;
    return GCiv(td->lakes, c) == td->lid;
}

int tree(treedata_t *td, ral_cell_t a) 
{
    /* arc a..b , k is a running number for the subcatchments
       this is a recursive function
       let a = b = root point when calling from outside
    */
    ral_cell_t b = a;
    if (GCivD(td->lakes, a) AND GCiv(td->lakes, a)) { /* on a lake */
	FAIL_UNLESS(lake(td, a));
	ral_mark_upslope_cells(&(td->pp), ral_rcoords_new(a.x, a.y), td->k);
	td->k++;
	return 1;
    }
    while (1) {
	/* move along the arc from b onwards */    
	int c;
	ral_cell_t u[8]; /* upstream cells of b */
    
	/* check for loops, visited stream cells are marked by
	   multiplying them by -1 this has no effect to the original
	   streams grid since we use a copy here */

	if (GCivD(td->streams, b)) {
	    ASSERT(GCiv(td->streams, b) >= 0, "Loop detected when recursing a catchment. Fail.");
	    GCiv(td->streams, b) *= -1;
	}

	c = upstream_cells_of(td->streams, td->pp.fdg, b, u);

	if (c == 0) { /* b is an end cell of an arc */
	    if (td->headwaters) {
		ral_mark_upslope_cells(&(td->pp), ral_rcoords_new(b.x, b.y), td->k);
		td->k++;
		FAIL_UNLESS(ral_catchment_add(td->catchment, b, td->last_stream_section_end));
	    }
	    break;
	} else if (c == 1) {      /* b is an arc cell */
	    if (GCivD(td->lakes, u[0]) AND GCiv(td->lakes, u[0])) { /* but on a lake */
		FAIL_UNLESS(lake(td, u[0]));
		ral_mark_upslope_cells(&(td->pp), ral_rcoords_new(u[0].x, u[0].y), td->k);
		td->k++;
		FAIL_UNLESS(ral_catchment_add(td->catchment, u[0], a));
		break;
	    } else { /* regular arc point */
		b = u[0];
	    }
	} else {           /* b is a cell where two or more arcs join */
	    int l;
	    for (l = 0; l < c; l++) {
		if (GCivD(td->lakes, u[l]) AND GCiv(td->lakes, u[l])) { /* a lake cell */
		    FAIL_UNLESS(lake(td, u[l]));
		} else {
		    td->last_stream_section_end = u[l];
		    FAIL_UNLESS(tree(td, u[l]));
		}
		ral_mark_upslope_cells(&(td->pp), ral_rcoords_new(u[l].x, u[l].y), td->k);
		td->k++;
		FAIL_UNLESS(ral_catchment_add(td->catchment, u[l], a));
	    }
	    break;
	}
    }

    return 1;
 fail:
    return 0;
}

int lake(treedata_t *td, ral_cell_t a) 
{
    /* backup the old values: */
    int lid = td->lid; 
    ral_cell_t a_copy = td->a;

    td->lid = GCiv(td->lakes, a);  /* lake id */    
    td->a = a;

    FAIL_UNLESS(borderwalk(a, GCiv(td->pp.fdg, a), td, &ral_lakearea2, &ral_testlake2));

    /* return the old values */
    td->lid = lid; 
    td->a = a_copy;
    return 1;
 fail:
    return 0;
}
