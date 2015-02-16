#ifndef RAL_STATISTICS_H
#define RAL_STATISTICS_H

/**\file ral2/grid/statistics.h
   \brief Geostatistical methods for grids
*/

/**\brief Computes sample variogram y(h).
   
   The result is a an array of three arrays. The first contains the
   lags, the second contains the variogram values, and the third
   contains the number of observations associated with each lag.
 */
ral_array_t * RAL_CALL ral_grid_variogram(ral_grid_t *gd, double max_lag, int lags);

typedef double ral_variogram_function(double lag, double *param);

/**\brief Spherical variogram function. */
double ral_spherical(double lag, double *param);

/**\brief Estimate a value using kriging. 

   The result is an array that contains the estimated value and its
   expected error at point p.
*/
ral_array_t * RAL_CALL ral_grid_krige(ral_grid_t *gd, 
                                      ral_rcoords_t * p, 
                                      ral_variogram_function S, 
                                      double *param, 
                                      double range);

#endif
