#ifndef RAL_GRID_H
#define RAL_GRID_H

/**\file ral2/grid.h
   \brief Grid class

   The origin of the cell coordinate system is top left (x = 0, y =
   0), while the origin of the world coordinate system is bottom left
   (x = 0, y = 0). A grid is either an integer grid or real number
   grid. The actual datatype is set in configuration.

   Many methods work in-place, i.e., change the parameter grid (self).
   All comparison operations silently transform the first argument
   real grid into an integer (actuall, a boolean) grid.
*/

extern ral_class ralGrid;

typedef struct ral_grid ral_grid_t;

typedef enum {
    integer,
    real 
} ral_grid_type_t;

ral_grid_t RAL_CALL *ral_grid_new(ral_grid_type_t type, int width, int height);

ral_grid_t RAL_CALL *ral_grid_new_like(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e);

ral_grid_t RAL_CALL *ral_grid_new_copy(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e);

void ral_grid_delete(ral_grid_t *gd);

void *ral_grid_data(ral_grid_t *gd);

int RAL_CALL ral_grid_get_height(ral_grid_t *gd);
int RAL_CALL ral_grid_get_width(ral_grid_t *gd);
ral_grid_type_t RAL_CALL ral_grid_get_type(ral_grid_t *gd);

double RAL_CALL ral_grid_get_cell_size(ral_grid_t *gd);
ral_wbox_t * RAL_CALL ral_grid_get_world(ral_grid_t *gd);

ral_object_t *RAL_CALL ral_grid_get_no_data_value(ral_grid_t *gd);
int RAL_CALL ral_grid_set_no_data_value(ral_grid_t *gd, ral_object_t *value, ral_error_t **e);

void RAL_CALL ral_grid_flip_horizontal(ral_grid_t *gd);
void RAL_CALL ral_grid_flip_vertical(ral_grid_t *gd);

/** coerces the grid into a new data_type */
int RAL_CALL ral_grid_coerce(ral_grid_t *gd, ral_grid_type_t type, ral_error_t **e);

/** are the two grids of same size and from same geographical area (accuracy not withstanding) */
int RAL_CALL ral_grid_overlayable(ral_grid_t *g1, ral_grid_t *g2);

void RAL_CALL ral_grid_set_bounds(ral_grid_t *gd, double cell_size, double minX, double minY);

void RAL_CALL ral_grid_copy_bounds(ral_grid_t *from, ral_grid_t *to);

/** exactly at maxX returns width, and at maxY returns height 
    no checks: the cell and point may well be outside of the grid world
*/
ral_rcoords_t * RAL_CALL ral_grid_point2cell(ral_grid_t *gd, ral_wcoords_t * p);

/** the returned point is the center of the cell */
ral_wcoords_t * RAL_CALL ral_grid_cell2point(ral_grid_t *gd, ral_rcoords_t * c);

/** the returned point is the up left corner of the cell */
ral_wcoords_t * RAL_CALL ral_grid_cell2point_upleft(ral_grid_t *gd, ral_rcoords_t * c);

/** Local methods */

ral_object_t * RAL_CALL ral_grid_get(ral_grid_t *gd, ral_rcoords_t * c);
void RAL_CALL ral_grid_set(ral_grid_t *gd, ral_rcoords_t * c, ral_object_t *value, ral_error_t **e);

/** makes a binary grid: 1: there is data, 0: there is no data */
int RAL_CALL ral_grid_is_defined(ral_grid_t *gd);

/** computes NOT gd for all cells according to 3VL:
    <table>
    <tr><td><strong>gd</strong></td><td><strong>NOT gd</strong></td></tr>
    <tr><td>true</td><td>false</td></tr>
    <tr><td>false</td><td>true</td></tr>
    <tr><td>unknown</td><td>unknown</td></tr>
    </table>
 */
int RAL_CALL ral_grid_not(ral_grid_t *gd);

/** computes gd1 AND gd2 for all cells according to 3VL:
    <table>
    <tr><td><strong>gd1</strong></td><td><strong>gd2</strong></td><td><strong>gd1 AND gd2</strong></td></tr>
    <tr><td>true</td><td>true</td><td>true</td></tr>
    <tr><td>true</td><td>false</td><td>false</td></tr>
    <tr><td>true</td><td>unknown</td><td>unknown</td></tr>
    <tr><td>false</td><td>true</td><td>false</td></tr>
    <tr><td>false</td><td>false</td><td>false</td></tr>
    <tr><td>false</td><td>unknown</td><td>false</td></tr>
    <tr><td>unknown</td><td>true</td><td>unknown</td></tr>
    <tr><td>unknown</td><td>false</td><td>false</td></tr>
    <tr><td>unknown</td><td>unknown</td><td>unknown</td></tr>
    </table>

*/
int RAL_CALL ral_grid_and_grid(ral_grid_t *gd1, ral_grid_t *gd2);
/** computes gd1 OR gd2 for all cells according to 3VL:
    <table>
    <tr><td><strong>gd1</strong></td><td><strong>gd2</strong></td><td><strong>gd1 OR gd2</strong></td></tr>
    <tr><td>true</td><td>true</td><td>true</td></tr>
    <tr><td>true</td><td>false</td><td>true</td></tr>
    <tr><td>true</td><td>unknown</td><td>true</td></tr>
    <tr><td>false</td><td>true</td><td>true</td></tr>
    <tr><td>false</td><td>false</td><td>false</td></tr>
    <tr><td>false</td><td>unknown</td><td>unknown</td></tr>
    <tr><td>unknown</td><td>true</td><td>true</td></tr>
    <tr><td>unknown</td><td>false</td><td>unknown</td></tr>
    <tr><td>unknown</td><td>unknown</td><td>unknown</td></tr>
    </table>
*/
int RAL_CALL ral_grid_or_grid(ral_grid_t *gd1, ral_grid_t *gd2);

void RAL_CALL ral_grid_add(ral_grid_t *gd, ral_object_t *x);
int RAL_CALL ral_grid_add_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_sub_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
void RAL_CALL ral_grid_mult(ral_grid_t *gd, ral_object_t *x);
int RAL_CALL ral_grid_mult_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
void RAL_CALL ral_grid_div(ral_grid_t *gd, ral_object_t *x, int reversed, ral_error_t **e);
int RAL_CALL ral_grid_div_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
void RAL_CALL ral_grid_modulus(ral_grid_t *gd, int x, int reversed);
int RAL_CALL ral_grid_modulus_grid(ral_grid_t *gd1, ral_grid_t *gd2);
void RAL_CALL ral_grid_power(ral_grid_t *gd, double x, int reversed);
int RAL_CALL ral_grid_power_grid(ral_grid_t *gd1, ral_grid_t *gd2);

ral_grid_t RAL_CALL *ral_grid_round(ral_grid_t *gd, ral_error_t **e);
int RAL_CALL ral_grid_abs(ral_grid_t *gd);
int RAL_CALL ral_grid_acos(ral_grid_t *gd);
int RAL_CALL ral_grid_atan(ral_grid_t *gd);
int RAL_CALL ral_grid_atan2(ral_grid_t *gd1, ral_grid_t *gd2);
int RAL_CALL ral_grid_ceil(ral_grid_t *gd);
int RAL_CALL ral_grid_cos(ral_grid_t *gd);
int RAL_CALL ral_grid_cosh(ral_grid_t *gd);
int RAL_CALL ral_grid_exp(ral_grid_t *gd);
int RAL_CALL ral_grid_floor(ral_grid_t *gd);
int RAL_CALL ral_grid_log(ral_grid_t *gd);
int RAL_CALL ral_grid_log10(ral_grid_t *gd);
int RAL_CALL ral_grid_pow(ral_grid_t *gd, ral_object_t *value);
int RAL_CALL ral_grid_sin(ral_grid_t *gd);
int RAL_CALL ral_grid_sinh(ral_grid_t *gd);
int RAL_CALL ral_grid_sqrt(ral_grid_t *gd);
int RAL_CALL ral_grid_tan(ral_grid_t *gd);
int RAL_CALL ral_grid_tanh(ral_grid_t *gd);

int RAL_CALL ral_grid_lt(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_gt(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_le(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_ge(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_eq(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_ne(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);
int RAL_CALL ral_grid_cmp(ral_grid_t *gd, ral_object_t *x, ral_error_t **e);

int RAL_CALL ral_grid_lt_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_gt_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_le_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_ge_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_eq_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_ne_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);
int RAL_CALL ral_grid_cmp_grid(ral_grid_t *gd1, ral_grid_t *gd2, ral_error_t **e);

void RAL_CALL ral_grid_min(ral_grid_t *gd, ral_object_t *x);
void RAL_CALL ral_grid_max(ral_grid_t *gd, ral_object_t *x);

int RAL_CALL ral_grid_min_grid(ral_grid_t *gd1, ral_grid_t *gd2);
int RAL_CALL ral_grid_max_grid(ral_grid_t *gd1, ral_grid_t *gd2);

/** the new value of each cell is a random portion of the old value
 * using uniform distribution [0..1] */
void RAL_CALL ral_grid_random(ral_grid_t *gd);

/** Focal methods */

ral_array_t * RAL_CALL ral_grid_get_focal(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);
ral_object_t *RAL_CALL ral_grid_focal_sum(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);
ral_object_t *RAL_CALL ral_grid_focal_mean(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);
ral_object_t *RAL_CALL ral_grid_focal_variance(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);
ral_object_t *RAL_CALL ral_grid_focal_count(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);
ral_object_t *RAL_CALL ral_grid_focal_count_of(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus, RAL_INTEGER value);
ral_interval_t * RAL_CALL ral_grid_focal_range(ral_grid_t *grid, ral_rcoords_t * c, ral_array_t * focus);

ral_grid_t RAL_CALL *ral_grid_focal_sum_grid(ral_grid_t *grid, ral_array_t * focus);
ral_grid_t RAL_CALL *ral_grid_focal_mean_grid(ral_grid_t *grid, ral_array_t * focus);
ral_grid_t RAL_CALL *ral_grid_focal_variance_grid(ral_grid_t *grid, ral_array_t * focus);
ral_grid_t RAL_CALL *ral_grid_focal_count_grid(ral_grid_t *grid, ral_array_t * focus, ral_error_t **e);
ral_grid_t RAL_CALL *ral_grid_focal_count_of_grid(ral_grid_t *grid, ral_array_t * focus, RAL_INTEGER value, ral_error_t **e);

/** Zonal methods */

ral_hash_t * RAL_CALL ral_grid_zonal_count(ral_grid_t *gd, ral_grid_t *zones);
ral_hash_t * RAL_CALL ral_grid_zonal_count_of(ral_grid_t *gd, ral_grid_t *zones, RAL_INTEGER value);
ral_hash_t * RAL_CALL ral_grid_zonal_sum(ral_grid_t *gd, ral_grid_t *zones);
ral_hash_t * RAL_CALL ral_grid_zonal_range(ral_grid_t *gd, ral_grid_t *zones);
ral_hash_t * RAL_CALL ral_grid_zonal_min(ral_grid_t *gd, ral_grid_t *zones); /** deprecated, use ral_grid_zonal_range */
ral_hash_t * RAL_CALL ral_grid_zonal_max(ral_grid_t *gd, ral_grid_t *zones); /** deprecated, use ral_grid_zonal_range */
ral_hash_t * RAL_CALL ral_grid_zonal_mean(ral_grid_t *gd, ral_grid_t *zones);
ral_hash_t * RAL_CALL ral_grid_zonal_variance(ral_grid_t *gd, ral_grid_t *zones);
ral_hash_t * RAL_CALL ral_grid_zonal_contents(ral_grid_t *gd, ral_grid_t *zones);

/** Global methods */

void RAL_CALL ral_grid_set_all(ral_grid_t *gd, ral_object_t *value, ral_error_t **e);
ral_interval_t * RAL_CALL ral_grid_get_value_range(ral_grid_t *gd);

/** count, sum, mean, and variance of all cell values (except of 'no data' cells)*/
long RAL_CALL ral_grid_count(ral_grid_t *gd);
long RAL_CALL ral_grid_count_of(ral_grid_t *gd, RAL_INTEGER value);
double RAL_CALL ral_grid_sum(ral_grid_t *gd);
double RAL_CALL ral_grid_mean(ral_grid_t *gd);
double RAL_CALL ral_grid_variance(ral_grid_t *gd);

ral_bin_t * RAL_CALL ral_grid_get_histogram(ral_grid_t *gd, ral_array_t * bins);

/** returns a hash int=>int, works only for integer grids */
ral_hash_t * RAL_CALL ral_grid_get_contents(ral_grid_t *gd);

ral_grid_t RAL_CALL *ral_grid_cross(ral_grid_t *a, ral_grid_t *b, ral_error_t **e);

/** if a then b = c */
void RAL_CALL ral_grid_if_then(ral_grid_t *a, ral_grid_t *b, ral_object_t *c);
/** if a then b = c else d */
void RAL_CALL ral_grid_if_then_else(ral_grid_t *a, ral_grid_t *b, ral_object_t *c, ral_object_t *d);
/** if a then b = c */
void RAL_CALL ral_grid_if_then_grid(ral_grid_t *a, ral_grid_t *b, ral_grid_t *c);

/** classifier can be a hash or bin for integer grids and bin for real
    grids may change the data type of the grid */
void RAL_CALL ral_grid_reclassify(ral_grid_t *gd, ral_classifier_t * classifier, ral_error_t **e);

#endif
