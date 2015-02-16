#ifndef RAL_GRID_IO_H
#define RAL_GRID_IO_H

/**\file ral2/grid/io.h
   \brief Input/output methods for grids

*/

/** gets a piece of a GDAL raster by GDALRasterIO,
    creates either an integer or real grid depending on the data type of GDAL raster,
    returns NULL with error message or 
    NULL without an error if the clip_region does not overlap datasets bbox
*/
ral_grid_t RAL_CALL *ral_grid_new_using_GDAL(GDALDatasetH dataset, 
                                             int band, 
                                             ral_wbox_t *clip_region, 
                                             double cell_size);

/** printf all rows */
int RAL_CALL ral_grid_print(ral_grid_t *gd);

int RAL_CALL ral_grid_save_ascii(ral_grid_t *gd, char *outfile);

/** write the contents of the raster into a binary file, does _not_ create a header file
 */
int RAL_CALL ral_grid_write(ral_grid_t *gd, char *filename, ral_error_t **e);

#endif
