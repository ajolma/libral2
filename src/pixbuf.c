#include "ral2/ral.h"
#include "priv/priv.h"

static void ral_pixbuf_destroy_notify (guchar * pixels, gpointer data)
{
    /*fprintf(stderr,"free %#x\n",pixels);*/
    if (pixels) free(pixels);
}

ral_object_t *Pixbuf_new(ral_object_t *_self)
{
    ral_pixbuf_t *self = _self;
    self->image = NULL;
    self->pixbuf = NULL;
    self->destroy_fn = NULL;
    self->width = 0;
    self->height = 0;
    self->device.world.min.x = 0;
    self->device.world.max.y = 0;
    self->pixel_size = 0;
    self->device.world.max.x = 0;
    self->device.world.min.y = 0;
    self->image_rowstride = 0;
    self->rowstride = 0;
    self->colorspace = GDK_COLORSPACE_RGB;
    self->has_alpha = FALSE;
    self->bits_per_sample = 8;
    return _self;
}

void Pixbuf_delete(ral_object_t *_self)
{
    ral_pixbuf_t *self = _self;
    cairo_destroy(self->cr);
    if (self->image) free(self->image);
    self->image = NULL;
}

const char *Pixbuf_cname()
{
    return "Pixbuf";
}

static const struct Class _Pixbuf = {
    sizeof(struct ral_pixbuf),
    {&_OutputDevice, NULL, NULL, NULL},
    /*  1 */ Pixbuf_new,
    /*  2 */ NULL,
    /*  3 */ Pixbuf_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Pixbuf_cname,
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

ral_class Pixbuf = &_Pixbuf;
ral_class ralPixbuf = &_Pixbuf;

void _ral_pixbuf_cairo_setup(ral_pixbuf_t *self)
{
    self->surface = cairo_image_surface_create_for_data(self->image,
                                                        CAIRO_FORMAT_ARGB32,
                                                        self->width,
                                                        self->height,
                                                        self->image_rowstride);
    self->cr = cairo_create(self->surface);
}

int ral_pixbuf_create(ral_pixbuf_t *self, int width, int height, double min_x, double max_y, double pixel_size, int red, int green, int blue)
{
    if (width*height > 0)
        TEST(self->image = malloc(PIXELSIZE*width*height));
    self->destroy_fn = ral_pixbuf_destroy_notify;
    self->width = width;
    self->height = height;
    self->device.world.min.x = min_x;
    self->device.world.max.y = max_y;
    self->pixel_size = pixel_size;
    self->device.world.max.x = self->device.world.min.x + (double)self->width*self->pixel_size;
    self->device.world.min.y = self->device.world.max.y - (double)self->height*self->pixel_size;
    self->image_rowstride = self->width * PIXELSIZE;
    self->rowstride = self->width * PIXBUF_PIXELSIZE;
    int i,j;
    for (i = 0; i < self->height; i++) 
        for (j = 0; j < self->width; j++)
            PB(self, i, j, red, green, blue);
    _ral_pixbuf_cairo_setup(self);
    return 1;
fail:
    return 0;
}

int ral_pixbuf_create_from_grid(ral_pixbuf_t *self, ral_grid_t *gd)
{
    TEST(self->image = malloc(PIXELSIZE*gd->height*gd->width));
    self->destroy_fn = ral_pixbuf_destroy_notify;
    self->height = gd->height;
    self->width = gd->width;
    self->image_rowstride = self->width * PIXELSIZE;
    self->rowstride = self->width * PIXBUF_PIXELSIZE;
    self->device.world = gd->world;
    self->pixel_size = gd->cell_size;
    int i,j;
    for (i = 0; i < self->height; i++) 
        for (j = 0; j < self->width; j++)
	    PB(self, i, j, 0, 0, 0);
    _ral_pixbuf_cairo_setup(self);
    return 1;
fail:
    return 0;
}

void ral_pixbuf_create_from_data(ral_pixbuf_t *self,
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
                                 double pixel_size)
{
    self->image = image;
    self->image_rowstride = image_rowstride;
    self->pixbuf = gpixbuf;
    self->destroy_fn = destroy_fn;
    self->colorspace = colorspace;
    self->has_alpha = has_alpha;
    self->rowstride = rowstride;
    self->bits_per_sample = bits_per_sample;
    self->width = width;
    self->height = height;
    self->device.world.min.x = min_x;
    self->device.world.min.y = min_y;
    self->device.world.max.x = max_x;
    self->device.world.max.y = max_y;
    self->pixel_size =pixel_size;
    _ral_pixbuf_cairo_setup(self);
}

/* modified from goffice's go-image.c */
int ral_pixbuf_cairo2pixbuf(ral_pixbuf_t *self)
{
    guint i, j;
    unsigned char *src, *dst;

    if (self->pixbuf) free(self->pixbuf);
    TEST(self->pixbuf = malloc(PIXBUF_PIXELSIZE*self->height*self->width));
    self->destroy_fn = ral_pixbuf_destroy_notify;

    dst = self->pixbuf;
    src = self->image;

    for (i = 0; i < self->height; i++) {
	for (j = 0; j < self->width; j++) {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
	    dst[0] = src[2];
	    dst[1] = src[1];
	    dst[2] = src[0];
#else
	    dst[0] = src[1];
	    dst[1] = src[2];
	    dst[2] = src[3];
#endif
	    src += PIXELSIZE;
	    dst += PIXBUF_PIXELSIZE;
	}
    }

    return 1;
fail:
    return 0;
}

GdkPixbuf *ral_pixbuf_get_pixbuf(ral_pixbuf_t *self)
{
    return gdk_pixbuf_new_from_data(self->pixbuf,
                                    self->colorspace,
                                    self->has_alpha,
                                    self->bits_per_sample,
                                    self->width,
                                    self->height,
                                    self->rowstride,
                                    self->destroy_fn,
                                    NULL);
}

int ral_pixbuf_render_grid_flow_directions(ral_pixbuf_t * pb, ral_grid_t *gd, ral_color_t *color)
{
    ral_cell_t c;
    int w = floor(gd->cell_size/pb->pixel_size/2.0)+1;
    int v = floor(w/2.0)+1;
    for(c.y = 0; c.y < gd->height; c.y++) {
	int i = GDi2PBi(gd, c.y, pb);
	for(c.x = 0; c.x < gd->width; c.x++) {
	    if (GCivND(gd, c)) continue;
	    int j = GDj2PBj(gd, c.x, pb);
	    switch (GCiv(gd, c)) {
	    case RAL_N: {
		int a, i2 = GDi2PBi(gd, c.y-1, pb);
		if (j >= 0 AND j < pb->width)
		    for (a = MIN(pb->height-1,i); a >= MAX(0,i2); a--)
			PBset(pb,a,j,color);
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i-w+a,j-a,color);
		    PBsetTest(pb,i-w+a,j+a,color);
		}
		break;
	    }
	    case RAL_NE: {
		int a, d, j2 = GDj2PBj(gd, c.x+1, pb);
		for (d = 0; d < j2 - j + 1; d++) {
		    int a = i-d, b = j+d;
		    PBsetTest(pb,a,b,color);
		}
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i-w+a,j+w,color);
		    PBsetTest(pb,i-w,j+w-a,color);
		}
		break;
	    }
	    case RAL_E: {
		int a, b, j2 = GDj2PBj(gd, c.x+1, pb);
		if (i >= 0 AND i < pb->height)
		    for (b = MAX(0,j); b <= MIN(pb->width-1,j2); b++)
			PBset(pb,i,b,color);
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i-a,j+w-a,color);
		    PBsetTest(pb,i+a,j+w-a,color);
		}
		break;
	    }
	    case RAL_SE: {
		int a, d, j2 = GDj2PBj(gd, c.x+1, pb);
		for (d = 0; d < j2 - j + 1; d++) {
		    int a = i+d, b = j+d;
		    PBsetTest(pb,a,b,color);
		}
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i+w-a,j+w,color);
		    PBsetTest(pb,i+w,j+w-a,color);
		}
		break;
	    }
	    case RAL_S: {
		int a, i2 = GDi2PBi(gd, c.y+1, pb);
		if (j >= 0 AND j < pb->width)
		    for (a = MAX(0,i); a <= MIN(pb->height-1,i2); a++)
			PBset(pb,a,j,color);
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i+w-a,j-a,color);
		    PBsetTest(pb,i+w-a,j+a,color);
		}
		break;
	    }
	    case RAL_SW: {
		int a, d, j2 = GDj2PBj(gd, c.x-1, pb);
		for (d = 0; d < j - j2 + 1; d++) {
		    int a = i+d, b = j-d;
		    PBsetTest(pb,a,b,color);
		}
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i+w-a,j-w,color);
		    PBsetTest(pb,i+w,j-w+a,color);
		}
		break;
	    }
	    case RAL_W: {
		int a, b, j2 = GDj2PBj(gd, c.x-1, pb);
		if (i >= 0 AND i < pb->height)
		    for (b = MIN(pb->width-1,j); b >= MAX(0,j2); b--)
			PBset(pb,i,b,color);
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i-a,j-w+a,color);
		    PBsetTest(pb,i+a,j-w+a,color);
		}
		break;
	    }
	    case RAL_NW: {
		int a, d, j2 = GDj2PBj(gd, c.x-1, pb);
		for (d = 0; d < j - j2 + 1; d++) {
		    int a = i-d, b = j-d;
		    PBsetTest(pb,a,b,color);
		}
		for (a = 0; a < v; a++) {
		    PBsetTest(pb,i-w+a,j-w,color);
		    PBsetTest(pb,i-w,j-w+a,color);
		}
		break;
	    }
	    case RAL_FLAT_AREA: {
		int a, b;
		if (j-v >= 0 AND j-v < pb->width)
		    for (a = MAX(0,i-v); a < MIN(pb->height,i+v+1); a++)
			PBset(pb,a,j-v,color);
		if (i-v >= 0 AND i-v < pb->height)
		    for (b = MAX(0,j-v); b < MIN(pb->width,j+v+1); b++)
			PBset(pb,i-v,b,color);
		if (j+v >= 0 AND j+v < pb->width)
		    for (a = MAX(0,i-v); a < MIN(pb->height,i+v+1); a++)
			PBset(pb,a,j+v,color);
		if (i+v >= 0 AND i+v < pb->height)
		    for (b = MAX(0,j-v); b < MIN(pb->width,j+v+1); b++)
			PBset(pb,i+v,b,color);
		break;
	    }
	    case RAL_PIT_CELL: {
		ral_cell_t c1, c2;
		c1.y = i-v;
		c1.x = j+v;
		c2.y = i+v;
		c2.x = j;
		LINE(pb, c1, c2, color, PBsetCell);
		c1.x = j-v;
		LINE(pb, c1, c2, color, PBsetCell);
		c2.y = i-v;
		c2.x = j+v;
		LINE(pb, c1, c2, color, PBsetCell);
		break;
	    }
	    default:
		break;
	    }
	}
    }
    return 1;
}

int ral_pixbuf_render_squares(ral_pixbuf_t *pb, ral_geometry_t *g, int symbol_size, ral_color_t *border_color, ral_color_t *fill_color)
{
    int a, b;
    for (a = 0; a < g->n_parts; a++)
	for (b = 0; b < g->parts[a].n; b++) {
	    ral_point_t p = g->parts[a].nodes[b];
	    if (POINT_IN_RECTANGLE(p, pb->device.world)) {
		int i = floor((pb->device.world.max.y - p.y)/pb->pixel_size);
		int j = floor((p.x - pb->device.world.min.x)/pb->pixel_size);
		int i2, j2;
		if (i == pb->height OR j == pb->width) /* case where y === min or x == max */
		    continue;
		for (i2 = MAX(0,i-symbol_size+1); i2 < MIN(pb->height,i+symbol_size); i2++)
		    for (j2 = MAX(0,j-symbol_size+1); j2 < MIN(pb->width,j+symbol_size); j2++)
			PBset(pb,i2,j2,fill_color);
	    }
	}
    return 1;
}

int ral_pixbuf_render_dots(ral_pixbuf_t *pb, ral_geometry_t *g, int r, ral_color_t *color)
{
    int a, b;
    for (a = 0; a < g->n_parts; a++)
	for (b = 0; b < g->parts[a].n; b++) {
	    ral_point_t p = g->parts[a].nodes[b];
	    if (POINT_IN_RECTANGLE(p, pb->device.world)) {
		ral_cell_t pixel;
		pixel.y = floor((pb->device.world.max.y - p.y)/pb->pixel_size);
		pixel.x = floor((p.x - pb->device.world.min.x)/pb->pixel_size);
		if (pixel.y == pb->height OR pixel.x == pb->width) /* case where y === min or x == max */
		    continue;
		if (r == 1) {
		    PBsetCell(pb, pixel, color);
		} else {
		    FILLED_CIRCLE(pb, pixel, r, color, PBsetCell);
		}
	    }
	}
    return 1;
}

int ral_pixbuf_render_crosses(ral_pixbuf_t *pb, ral_geometry_t *g, int symbol_size, ral_color_t *color)
{
    int a, b;
    for (a = 0; a < g->n_parts; a++)
	for (b = 0; b < g->parts[a].n; b++) {
	    ral_point_t p = g->parts[a].nodes[b];
	    if (POINT_IN_RECTANGLE(p, pb->device.world)) {
		int i = floor((pb->device.world.max.y - p.y)/pb->pixel_size);
		int j = floor((p.x - pb->device.world.min.x)/pb->pixel_size);
		int i2, j2;
		if (i == pb->height OR j == pb->width)
		    continue;
		for (i2 = MAX(0,i-symbol_size+1); i2 < MIN(pb->height,i+symbol_size); i2++)
		    PBset(pb, i2, j, color);
		for (j2 = MAX(0,j-symbol_size+1); j2 < MIN(pb->width,j+symbol_size); j2++)
		    PBset(pb, i, j2, color);
	    }
	}
    return 1;
}

int ral_pixbuf_render_wind_roses(ral_pixbuf_t *pb, ral_feature_t *feature, ral_color_t *color)
{
    int a, b;
    static char *suunnat[8] = {"N","NE","E","SE","S","SW","W","NW"};
    for (a = 0; a < feature->geometry->n_parts; a++)
	for (b = 0; b < feature->geometry->parts[a].n; b++) {
	    ral_point_t p = feature->geometry->parts[a].nodes[b];
	    if (POINT_IN_RECTANGLE(p,pb->device.world)) {
		int i = floor((pb->device.world.max.y - p.y)/pb->pixel_size);
		int j = floor((p.x - pb->device.world.min.x)/pb->pixel_size);
		int i2, j2;
		int k;
		if (i == pb->height OR j == pb->width)
		    continue;
		for (i2 = MAX(0,i-1); i2 < MIN(pb->height,i+1); i2++)
		    for (j2 = MAX(0,j-1); j2 < MIN(pb->width,j+1); j2++)
			PBset(pb, i2, j2, color);
		for (k = 0; k < 8; k++) {
		    int index = OGR_F_GetFieldIndex(feature->ogr_feature, suunnat[k]);
		    if (index >= 0) {
			int c, size = 5;
                        /*
			double val = OGR_F_GetFieldAsDouble(feature->ogr_feature, index);
			val = INTERPOLATE(nv2ss, val);
			if (k % 2) val /= 1.141593;
			size = floor(val);
                        */
			switch(k) {
			case 0: /* N */
			    for (i2 = MIN(pb->height,i); i2 > MAX(0,i-size); i2--)
				PBset(pb, i2, j, color);
			    break;
			case 1:
			    for (c = 0; c < size; c++)
				PBset(pb, MAX(MIN(i-c,pb->height),0), MAX(MIN(j+c,pb->width),0), color);
			    break;
			case 2: /* E */
			    for (j2 = MAX(0,j); j2 < MIN(pb->width,j+size); j2++)
				PBset(pb, i, j2, color);
			    break;
			case 3:
			    for (c = 0; c < size; c++)
				PBset(pb, MAX(MIN(i+c,pb->height),0), MAX(MIN(j+c,pb->width),0), color);
			    break;
			case 4: /* S */
			    for (i2 = MAX(0,i); i2 < MIN(pb->height,i+size); i2++)
				PBset(pb, i2, j, color);
			    break;
			case 5:
			    for (c = 0; c < size; c++)
				PBset(pb, MAX(MIN(i+c,pb->height),0), MAX(MIN(j-c,pb->width),0), color);
			    break;
			case 6: /* W */
			    for (j2 = MIN(pb->width,j); j2 > MAX(0,j-size); j2--)
				PBset(pb, i, j2, color);
			    break;
			case 7:
			    for (c = 0; c < size; c++)
				PBset(pb, MAX(MIN(i-c,pb->height),0), MAX(MIN(j-c,pb->width),0), color);
			    break;
			}
		    }
		}
	    }
	}
    return 1;
}

int ral_pixbuf_render_polylines(ral_pixbuf_t *pb, ral_geometry_t *g, ral_color_t *color)
{
    
    int i, j;

    cairo_set_source_rgb(pb->cr, (double)color->red/255.0, (double)color->green/255.0, (double)color->blue/255.0);

    for (i = 0; i < g->n_parts; i++)
	for (j = 0; j < g->parts[i].n - 1; j++) {
	    /* draw line from g->parts[i].nodes[j] to g->parts[i].nodes[j+1] */
	    /* clip */
	    ral_line_t l;
	    l.begin = g->parts[i].nodes[j];
	    l.end = g->parts[i].nodes[j+1];
	    if (clip_line_to_rect(&l, pb->device.world)) {
                
                cairo_move_to(pb->cr, 
                              (l.begin.x - pb->device.world.min.x)/pb->pixel_size, 
                              (pb->device.world.max.y - l.begin.y)/pb->pixel_size);

                cairo_line_to(pb->cr, 
                              (l.end.x - pb->device.world.min.x)/pb->pixel_size, 
                              (pb->device.world.max.y - l.end.y)/pb->pixel_size);
                continue;

		ral_cell_t cell1, cell2;
		cell1.y = floor((pb->device.world.max.y - l.begin.y)/pb->pixel_size);
		cell1.x = floor((l.begin.x - pb->device.world.min.x)/pb->pixel_size);
		cell2.y = floor((pb->device.world.max.y - l.end.y)/pb->pixel_size);
		cell2.x = floor((l.end.x - pb->device.world.min.x)/pb->pixel_size);
		LINE(pb, cell1, cell2, color, PBsetCell);
	    }
	}

    cairo_stroke (pb->cr);            

    return 1;
    
    PangoLayout *layout;
    PangoFontDescription *font_description;

    font_description = pango_font_description_new ();
    pango_font_description_set_family (font_description, "serif");
    pango_font_description_set_weight (font_description, PANGO_WEIGHT_BOLD);
    pango_font_description_set_absolute_size (font_description, 32 * PANGO_SCALE);

    layout = pango_cairo_create_layout (pb->cr);
    pango_layout_set_font_description (layout, font_description);
    pango_layout_set_text (layout, "Hello, world åäö ÅÄÖ", -1);

    cairo_set_source_rgb (pb->cr, 0.0, 0.0, 1.0);
    cairo_move_to (pb->cr, 10.0, 50.0);
    pango_cairo_show_layout (pb->cr, layout);

    g_object_unref (layout);
    pango_font_description_free (font_description);
    
    
    return 1;
}

int ral_pixbuf_render_polygons(ral_pixbuf_t *pb, ral_geometry_t *g, ral_color_t *color)
{

    int i, j;

    cairo_set_source_rgb(pb->cr, (double)color->red/255.0, (double)color->green/255.0, (double)color->blue/255.0);

    for (i = 0; i < g->n_parts; i++) {

        int k = 0;

        for (j = 0; j < g->parts[i].n - 1; j++) {
            ral_point_t p = g->parts[i].nodes[j];
            if (k == 0) {
                cairo_move_to(pb->cr, 
                              (p.x - pb->device.world.min.x)/pb->pixel_size, 
                              (pb->device.world.max.y - p.y)/pb->pixel_size);
                k = 1;
            } else {
                cairo_line_to(pb->cr, 
                              (p.x - pb->device.world.min.x)/pb->pixel_size, 
                              (pb->device.world.max.y - p.y)/pb->pixel_size);
            }
        }

        cairo_close_path(pb->cr);
    }

    cairo_stroke_preserve(pb->cr);
    cairo_fill(pb->cr);            

    return 1;

    if (g->n_points == 0) return 1;
    ral_active_edge_table_t *aet_list = ral_get_active_edge_table_array(g->parts, g->n_parts);
    FAIL_UNLESS(aet_list);
    ral_cell_t c;
    double y = pb->device.world.min.y + 0.5*pb->pixel_size;
    for (c.y = pb->height - 1; c.y >= 0; c.y--) {
	double *x;
	int n;
	ral_scanline_at(aet_list, g->n_parts, y, &x, &n);
	if (x) {
	    int draw = 0;
	    int begin = 0;
	    int k;
	    while ((begin < n) AND (x[begin] < pb->device.world.min.x)) {
		begin++;
		draw = !draw;
	    }
	    c.x = 0;
	    for (k = begin; k < n; k++) {
		int jmax = floor((x[k] - pb->device.world.min.x)/pb->pixel_size+0.5);
		while ((c.x < pb->width) AND (c.x < jmax)) {
		    if (draw) PBset(pb, c.y, c.x, color);
		    c.x++;
		}
		if (c.x == pb->width) break;
		draw = !draw;
	    }
	    ral_delete_scanline(&x);
	}
	y += pb->pixel_size;
    }
    ral_active_edge_table_array_destroy(&aet_list, g->n_parts);
    return 1;
fail:
    return 0;
}
 
