#ifndef RAL_H
#define RAL_H

/**\file ral.h
   \brief The main header file to include this library API 

   Errors are reported via a ral_error variable, which is created in
   the case of an error. Usage pattern:
   ral_error err = NULL;
   function_call(a, b, c, &err);
   if (err) {
       report_error(ral_as_string(err));
       delete(err);
   }
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

/* GDAL */
#include <gdal.h>
#include <ogr_api.h>
#include <ogr_srs_api.h>

/* cairo */
#include <cairo.h>
#include <pango/pangocairo.h>

/* GTK+ */
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

/* local configuration of the library: */
#include <ral2/config.h>

/* subsystems: */
#include <ral2/class.h>
#include <ral2/error.h>
#include <ral2/ordinal.h>
#include <ral2/integer.h>
#include <ral2/real.h>
#include <ral2/string.h>
#include <ral2/interval.h>
#include <ral2/data_point.h>
#include <ral2/classifier.h>
#include <ral2/linear_interpolation.h>
#include <ral2/color.h>
#include <ral2/symbol.h>
#include <ral2/array.h>
#include <ral2/storage.h>
#include <ral2/kvp.h>
#include <ral2/hash.h>
#include <ral2/bin.h>
#include <ral2/geometry.h>
#include <ral2/feature.h>
#include <ral2/layer.h>
#include <ral2/coords.h>
#include <ral2/grid.h>
#include <ral2/pixbuf.h>
#include <ral2/grid/dem.h>
#include <ral2/grid/image.h>
#include <ral2/grid/io.h>
#include <ral2/grid/neighborhood.h>
#include <ral2/grid/rasterize.h>
#include <ral2/grid/resample.h>
#include <ral2/grid/statistics.h>
#include <ral2/raster.h>
#include <ral2/vector.h>
#include <ral2/style.h>

#endif
