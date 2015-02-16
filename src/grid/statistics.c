#include "ral2/ral.h"
#include "priv/priv.h"

ral_array_t *ral_grid_variogram(ral_grid_t *gd, double max_lag, int lags)
{
    ral_cell_t a;
    int *n = CALLOC(lags, int);
    double *sum = CALLOC(lags, double);
    ral_array_t *variogram = NULL;
    double delta = max_lag / lags;
    int d = floor(max_lag/gd->cell_size);
    TEST(n && sum);
    FOR(a, gd) {
	if (GCD(gd, a)) {
	    ral_cell_t b;
	    for (b.y = a.y - d; b.y <= a.y + d; b.y++)
		for (b.x = a.x - d; b.x <= a.x + d; b.x++) {
		    /* note: not checking whether b is masked out */
		    if (GCin(gd, b) AND GCD(gd, b)) {
			int k = floor(DISTANCE_BETWEEN_CELLS(a, b)*gd->cell_size/delta);
			if (k < lags) {
			    double h = GCv(gd, a) - GCv(gd, b);
			    n[k]++;
			    sum[k] += h*h;
			}
		    }
		}
	}
    }
    FAIL_UNLESS(variogram = ral_new(ralArray));
    {
	int i;
	double lag = delta/2;
	for (i = 0; i < lags; i++) {
            ral_array_t *at_lag = ral_new(ralArray);
            ral_array_insert(at_lag, ral_real_new(lag), 0);
            ral_array_insert(at_lag, ral_real_new(sum[i]/(2*n[i])), 1);
            ral_array_insert(at_lag, ral_integer_new(n[i]), 2);
            ral_array_insert(variogram, at_lag, i);
            lag += delta;
	}
    }
    free(n);
    free(sum);
    return variogram;
 fail:
    FREE(n);
    FREE(sum);
    return NULL;
}

/* from http://www.me.unm.edu/~bgreen/ME360/Solving%20Linear%20Equations.pdf */

void ral_normal(double *a, double *b, int m);
void ral_pivot(double *a, double *b, int m);
void ral_forelm(double *a, double *b, int m);
void ral_baksub(double *a, double *b, double *x, int m);

/* Here are a series of routines that solve multiple linear equations
   using the Gaussian Elimination technique */
void ral_gauss(double *x, double *a, double *b, int m)
{
    /* Normalize the matrix */
    ral_normal (a, b, m);
    /* Arrange the equations for diagonal dominance */
    ral_pivot (a, b, m);
    /* Put into upper triangular form */
    ral_forelm (a, b, m);
    /* Do the back substitution for the solution */
    ral_baksub (a, b, x, m);
}

/* This routine normalizes each row of the matrix so that the largest
   term in a row has an absolute value of one */
void ral_normal(double *a, double *b, int m)
{
    int i;
    for (i = 0; i < m; i++)
    {
	double big = 0.0;
	int j;
	for (j = 0; j < m; j++)
	    if (big < fabs(a[i*m+j])) big = fabs(a[i*m+j]);
	for (j = 0; j < m; j++)
	    a[i*m+j] = a[i*m+j] / big;
	b[i] = b[i] / big;
    }
}

/* This routine attempts to rearrange the rows of the matrix so
   that the maximum value in each row occurs on the diagonal. */
void ral_pivot(double *a, double *b, int m)
{
    int i;
    double temp;
    for (i = 0; i < m-1; i++)
    {
	int ibig = i, j;
	for (j = i+1; j < m; j++)
	    if (fabs (a[ibig*m+i]) < fabs (a[j*m+i])) ibig = j;
	if (ibig != i)
	{
	    for (j = 0; j < m; j++)
	    {
		temp = a[ibig*m+j];
		a[ibig*m+j] = a[i*m+j];
		a[i*m+j] = temp;
	    }
	    temp = b[ibig];
	    b[ibig] = b[i];
	    b[i] = temp;
	}
    }
}

/* This routine does the forward sweep to put the matrix in to upper
   triangular form */
void ral_forelm(double *a, double *b, int m)
{
    int i;
    for (i = 0; i < m-1; i++)
    {
	int j;
	for (j = i+1; j < m; j++)
	{
	    if (a[i*m+i] != 0.0)
	    {
		double fact = a[j*m+i] / a[i*m+i];
		int k;
		for (k = 0; k < m; k++)
		    a[j*m+k] -= a[i*m+k] * fact;
		b[j] -= b[i]*fact;
	    }
	}
    }
}

/* This routine does the back substitution to solve the equations */
void ral_baksub(double *a, double *b, double *x, int m)
{
    int i, j;
    double sum;
    for (j = m-1; j >= 0; j--)
    {
	sum = 0.0;
	for (i = j+1; i < m; i++)
	    sum += x[i] * a[j*m+i];
	x[j] = (b[j] - sum) / a[j*m+j];
    }
}

ral_array_t *ral_grid_krige(ral_grid_t *gd, 
                            ral_rcoords_t *rp, 
                            ral_variogram_function S, 
                            double *param, 
                            double range)
{
    ral_cell_t p = {rp->y, rp->x};
    /* pick values */
    int delta = floor(range/gd->cell_size);
    ral_cell_t c;
    int i, j, n;
    ral_array_t *a = NULL;
    double *w = NULL; /* weights */
    double *d = NULL; /* distances between cells */
    double *b = NULL;
    double f;
    double s2;
    ral_array_t *estimate = NULL;

    FAIL_UNLESS(a = ral_new(ralArray));
    
    /* cells with values in the range */
    for (c.y = p.y - delta; c.y <= p.y + delta; c.y++)
	for (c.x = p.x - delta; c.x <= p.x + delta; c.x++) {
	    /* note: not checking if c is masked out */
	    if (GCin(gd, c) AND GCD(gd, c)) {
		double s = DISTANCE_BETWEEN_CELLS(p, c)*gd->cell_size;
		if (s < range) {
                    ral_array_push(a, ral_rcoords_new(c.x, c.y));
		}
	    }
	}
   
    n = ral_number_of_elements(a)+2; /* cells + slack */
    TEST(w = CALLOC(n, double));
    TEST(d = CALLOC(n*n, double));
    TEST(b = CALLOC(n, double));
    
    for (i = 0; i < n-1; i++) {
	b[i] = S(DISTANCE_BETWEEN_RCOORDS(a->array[i], rp)*gd->cell_size, param);
	d[i*n] = 0;
	for (j = i+1; j < n-1; j++) {
	    d[i*n+j] = S(DISTANCE_BETWEEN_RCOORDS(a->array[i], a->array[j])*gd->cell_size, param);
	}
    }
    for (j = 0; j < n-1; j++)
	d[(n-1)*n+j] = 1;
    d[n*n-1] = 0;
    b[n-1] = 1;

    ral_gauss(w, d, b, n);
    f = 0;
    s2 = 0;
    for (i = 0; i < n-1; i++) {
        c.y = ((ral_rcoords_t*)(a->array[i]))->y;
        c.x = ((ral_rcoords_t*)(a->array[i]))->x;
	f += w[i]*GCv(gd, c);
	s2 += w[i]*b[i];
    }
    s2 += w[n-1];
    ral_delete(a);
    free(w);
    free(d);
    free(b);
    estimate = ral_new(ralArray);
    ral_array_insert(estimate, ral_real_new(f), 0);
    ral_array_insert(estimate, ral_real_new(s2), 1);
    return estimate;
 fail:
    ral_delete(a);
    FREE(w);
    FREE(d);
    FREE(b);
    FREE(estimate);
    return NULL;
}

double ral_spherical(double lag, double *r)
{
    double h;
    if (lag == 0)
	return 0;
    if (lag >= *r)
	return 1;
    h = lag/(*r);
    return h * (1.5 - 0.5 * h * h);
}
