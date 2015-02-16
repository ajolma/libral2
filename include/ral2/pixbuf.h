#ifndef RAL_PIXBUF_H
#define RAL_PIXBUF_H

/**\file ral2/pixbuf.h
   \brief Pixbuf class

   A pixbuf is a graphical device for rendering.
*/

extern ral_class ralPixbuf;
typedef struct ral_pixbuf ral_pixbuf_t;

int ral_pixbuf_create(ral_pixbuf_t * self, int width, int height, double min_x, double max_y, double pixel_size, int red, int green, int blue);

int ral_pixbuf_create_from_grid(ral_pixbuf_t * self, ral_grid_t *gd);

void ral_pixbuf_create_from_data(ral_pixbuf_t * self,
                                 unsigned char *image, 
                                 int image_rowstride, 
                                 guchar *gpixbuf, 
                                 GdkPixbufDestroyNotify destroy_fn, 
                                 GdkColorspace colorspace,
                                 gboolean has_alpha,
                                 int rowstride,
                                 int bits_per_sample,
                                 int width,
                                 int height,
                                 double min_x,
                                 double min_y,
                                 double max_x,
                                 double max_y,
                                 double pixel_size);

/**\brief For converting a complete cairo image into a gdk pixbuf for GUI */
int ral_pixbuf_cairo2pixbuf(ral_pixbuf_t * self);

GdkPixbuf *ral_pixbuf_get_pixbuf(ral_pixbuf_t * self);

int ral_pixbuf_render_grid_flow_directions(ral_pixbuf_t * pb, ral_grid_t *gd, ral_color_t * color);

int ral_pixbuf_render_squares(ral_pixbuf_t * pb, ral_geometry_t * g, int symbol_size, ral_color_t * bc, ral_color_t * fc);

int ral_pixbuf_render_dots(ral_pixbuf_t * pb, ral_geometry_t * g, int radius, ral_color_t * color);

int ral_pixbuf_render_crosses(ral_pixbuf_t * pb, ral_geometry_t * g, int symbol_size, ral_color_t * color);

int ral_pixbuf_render_wind_roses(ral_pixbuf_t * pb, ral_feature_t * feature, ral_color_t * color);

int ral_pixbuf_render_polylines(ral_pixbuf_t * pb, ral_geometry_t * g, ral_color_t * color);

int ral_pixbuf_render_polygons(ral_pixbuf_t * pb, ral_geometry_t * g, ral_color_t * color);

#endif
