#include "ral2/ral.h"
#include "priv/priv.h"

void CPL_DLL CPL_STDCALL ral_cpl_error(CPLErr eclass, int code, const char *msg)
{
    fprintf(stderr, "GDAL: %s\n", msg);
    if (code == CE_Fatal)	
	exit(1);
}

#define P2G(xp,yp,t,xg,yg) xg = (t[0]) + (t[1])*(xp) + (t[2])*(yp); \
    yg = (t[3]) + (t[4])*(xp) + (t[5])*(yp);
#define G2P(xg,yg,t,xp,yp) xp = floor((t[0]) + (t[1])*(xg) + (t[2])*(yg)); \
    yp = floor((t[3]) + (t[4])*(xg) + (t[5])*(yg));

ral_grid_t RAL_CALL *ral_grid_new_using_GDAL(GDALDatasetH dataset, int band, ral_wbox_t *clip_region, double cell_size)
{
    GDALRasterBandH hBand;
    GDALDataType datatype;
    int width, height; /* size of the grid */
    ral_grid_type_t gd_type;
    int W, H; /* size of the full GDAL raster */
    int north_up, east_up, x0, y0, x1, y1, w, h; /* the clip region (P) */
    int ws, hs; /* the size of the buffer into which to rasterIO to */
    CPLErr err;
    ral_grid_t *gd_s = NULL, *gd_t = NULL;
    double t[6] = {0,1,0,0,0,1}; /* the geotransformation P -> G */
    double tx[6]; /* the inverse geotransformation G -> P */
    ral_rectangle_t GDAL; /* boundaries of the GDAL raster */
    ral_point_t p0, p1, p2, p3; /* locations of corners of clip region: 
                                   top left, top right, bottom right, bottom left, i.e,
                                   0,0       W',0       W',H'         0,H' */

    hBand = GDALGetRasterBand(dataset, band);
    ASSERT(hBand, BAD);
  
    CPLPushErrorHandler(ral_cpl_error);

    GDALGetGeoTransform(dataset, t);
    north_up = (t[2] == 0) AND (t[4] == 0);
    east_up = (t[1] == 0) AND (t[5] == 0);
    /*fprintf(stderr, "\nt is\n%f %f %f\n%f %f %f\n", t[0],t[1],t[2],t[3],t[4],t[5]);*/

    W = GDALGetRasterXSize(dataset);
    H = GDALGetRasterYSize(dataset);
    /*fprintf(stderr, "cell size is %f, raster size is %i %i\n", cell_size, W, H);*/

    /* test all corners to find the min & max xg and yg */
    {
        double x, y;
        P2G(0, 0, t, GDAL.min.x, GDAL.min.y);
        GDAL.max.x = GDAL.min.x;
        GDAL.max.y = GDAL.min.y;
        P2G(0, H, t, x, y);
        GDAL.min.x = MIN(x, GDAL.min.x);
        GDAL.min.y = MIN(y, GDAL.min.y);
        GDAL.max.x = MAX(x, GDAL.max.x);
        GDAL.max.y = MAX(y, GDAL.max.y);
        P2G(W, H, t, x, y);
        GDAL.min.x = MIN(x, GDAL.min.x);
        GDAL.min.y = MIN(y, GDAL.min.y);
        GDAL.max.x = MAX(x, GDAL.max.x);
        GDAL.max.y = MAX(y, GDAL.max.y);
        P2G(W, 0, t, x, y);
        GDAL.min.x = MIN(x, GDAL.min.x);
        GDAL.min.y = MIN(y, GDAL.min.y);
        GDAL.max.x = MAX(x, GDAL.max.x);
        GDAL.max.y = MAX(y, GDAL.max.y);
    }

    /*
    fprintf(stderr, "clip region is %f %f %f %f\n", clip_region->x_min, clip_region->y_min, clip_region->x_max, clip_region->y_max);
    fprintf(stderr, "GDAL raster is %f %f %f %f\n", GDAL.min.x, GDAL.min.y, GDAL.max.x, GDAL.max.y);
    */
    clip_region->x_min = MAX(clip_region->x_min, GDAL.min.x);
    clip_region->y_min = MAX(clip_region->y_min, GDAL.min.y);
    clip_region->x_max = MIN(clip_region->x_max, GDAL.max.x);
    clip_region->y_max = MIN(clip_region->y_max, GDAL.max.y);
    /*fprintf(stderr, "visible region is %f %f %f %f\n", clip_region->x_min, clip_region->y_min, clip_region->x_max, clip_region->y_max);*/
    GIVE_UP_IF((clip_region->x_min > clip_region->x_max) || (clip_region->y_min > clip_region->y_max));

    /* the inverse transformation, from georeferenced space to pixel space */
    {
        double tmp = t[2]*t[4] - t[1]*t[5];
        tx[0] = (t[0]*t[5] - t[2]*t[3])/tmp;
        tx[1] = -t[5]/tmp;
        tx[2] = t[2]/tmp;
        tx[3] = (t[1]*t[3] - t[0]*t[4])/tmp;
        tx[4] = t[4]/tmp;
        tx[5] = -t[1]/tmp;
    }

    /* the clip region in pixel space */
    /* test all corners to find the min & max xp and yp (x0..x1, y0..y1) */
    {
        int x, y;
        G2P(clip_region->x_min, clip_region->y_min, tx, x0, y0);
        x1 = x0;
        y1 = y0;
        G2P(clip_region->x_min, clip_region->y_max, tx, x, y);
        x0 = MIN(x, x0);
        y0 = MIN(y, y0);
        x1 = MAX(x, x1);
        y1 = MAX(y, y1);
        G2P(clip_region->x_max, clip_region->y_max, tx, x, y);
        x0 = MIN(x, x0);
        y0 = MIN(y, y0);
        x1 = MAX(x, x1);
        y1 = MAX(y, y1);
        G2P(clip_region->x_max, clip_region->y_min, tx, x, y);
        x0 = MIN(x, x0);
        y0 = MIN(y, y0);
        x1 = MAX(x, x1);
        y1 = MAX(y, y1);
        x0 = MAX(x0, 0); x0 = MIN(x0, W-1);
        x1 = MAX(x1, 0); x1 = MIN(x1, W-1);
        y0 = MAX(y0, 0); y0 = MIN(y0, H-1);
        y1 = MAX(y1, 0); y1 = MIN(y1, H-1);
    }
    w = x1 - x0 + 1;
    h = y1 - y0 + 1;
    /*fprintf(stderr, "visible region in raster is %i %i %i %i\n", x0, y0, x1, y1);*/
    GIVE_UP_IF(w <= 0 || h <= 0);
    
    /* the corners of the raster clip */
    P2G(x0, y0, t, p0.x, p0.y);
    P2G(x1+1, y0, t, p1.x, p1.y);
    P2G(x1+1, y1+1, t, p2.x, p2.y);
    P2G(x0, y1+1, t, p3.x, p3.y);
    /*fprintf(stderr, "source corners: (%f,%f  %f,%f  %f,%f  %f,%f)\n", p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);*/

    /* the width and height of the resized raster clip */
    /* the resized clip has the same corner coordinates as the clip */
    ws = round(sqrt((p1.x-p0.x)*(p1.x-p0.x)+(p1.y-p0.y)*(p1.y-p0.y))/cell_size);
    hs = round(sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y))/cell_size);
    /*fprintf(stderr, "clipped source size is %i, %i\n", ws, hs);*/

    switch (GDALGetRasterDataType(hBand)) {
    case GDT_Byte:
    case GDT_UInt16:
    case GDT_Int16:
    case GDT_UInt32:
    case GDT_Int32:
	gd_type = integer;
	switch (sizeof(RAL_INTEGER)) { /* hmm.. this breaks if INTEGER is unsigned */
	case 1:
	    datatype = GDT_Byte;
	    break;
	case 2:
	    datatype = GDT_Int16;
	    break;
	case 4:
	    datatype = GDT_Int32;
	    break;
	default:
	    ASSERT(0, "RAL_INTEGER has unsupported size");
	}
	break;
    case GDT_Float32:
    case GDT_Float64:
	gd_type = real;
	switch (sizeof(RAL_REAL)) {
	case 4:
	    datatype = GDT_Float32;
	    break;
	case 8:
	    datatype = GDT_Float64;
	    break;
	default:
	    ASSERT(0, "RAL_REAL has unsupported size");
	}
	break;
    default:
	ASSERT(0, "complex data type not supported");
    }
 
    /* size of the grid in display */
    
    width = ceil(fabs(clip_region->x_max - clip_region->x_min)/cell_size);
    height = ceil(fabs(clip_region->y_max - clip_region->y_min)/cell_size);
    
    /*fprintf(stderr, "display size is %i, %i\n", width, height);*/

    FAIL_UNLESS(gd_s = ral_grid_new(gd_type, ws, hs));
    FAIL_UNLESS(gd_t = ral_grid_new(gd_type, width, height));

    ral_grid_set_bounds(gd_t, cell_size, clip_region->x_min, clip_region->y_max-height*cell_size);
  
    err = GDALRasterIO(hBand, GF_Read, x0, y0, w, h, gd_s->data, ws, hs, datatype, 0, 0);
    FAIL_UNLESS(err == CE_None);
    {
	int success;
	double value = GDALGetRasterNoDataValue(hBand, &success);
	if (success) {
            ral_real_t *x = ral_new(ralReal);
            x->value = value;
            ral_grid_set_no_data_value(gd_t, x, NULL);
            ral_delete(x);
        }
        ral_grid_set_all(gd_t, ral_undef, NULL);
    }
    /* from source to target */
    {
        /* b = from p0 to p1 */
	double bx = p1.x - p0.x;
	double by = p1.y - p0.y;
        double b = east_up ? 0 : bx/by;
	double b2 = east_up ? 1 : sqrt(1+1/b/b);
	/*fprintf(stderr, "b = %f %f (%f)\n", bx, by, b);*/
        /* c = from p0 to p3 */
	double cx = p3.x - p0.x;
	double cy = p3.y - p0.y;
        double c = north_up ? 0 : cx/cy;
        double c2 = north_up ? 1 : sqrt(1+1/c/c);
	/*fprintf(stderr, "c = %f %f (%f)\n", cx, cy, c);*/
	ral_cell_t ct;
        if (gd_t->type == real) {
            FOR(ct, gd_t) {
                /* p = the location of the center of the target pixel in georeferenced coordinates */
                double px = clip_region->x_min + cell_size*(ct.x+0.5);
                double py = clip_region->y_max - cell_size*(ct.y+0.5);
		/* vector a = from p0 to p */
                double ax = px - p0.x;
		double ay = py - p0.y;
		/* x = the vector from p0 to the beginning of y */
                double x = east_up ? ay : (north_up ? ax : (ax-c*ay)/(1-c/b));
                if (!east_up AND !north_up AND (x/b > 0)) continue;
		/* the length */
                x = fabs(x)*b2;
		/* y = length of the parallel to c vector from b to p */
                double y = east_up ? ax : (north_up ? ay : (ax-b*ay)/(1-b/c));
                if (!east_up AND !north_up AND (y/c < 0)) continue;
		/* the length */
                y = fabs(y)*c2;
		ral_cell_t cs; /* the coordinates in the clipped raster */
                cs.x = floor(x/cell_size);
		cs.y = floor(y/cell_size);
                if (GCin(gd_s, cs)) 
                    GCnv(gd_t, ct) = GCnv(gd_s, cs);
            }
        } else {
            FOR(ct, gd_t) {
                /* p = the location of the center of the target pixel in georeferenced coordinates */
                double px = clip_region->x_min + cell_size*(ct.x+0.5);
                double py = clip_region->y_max - cell_size*(ct.y+0.5);
		/* vector a = from p0 to p */
                double ax = px - p0.x;
		double ay = py - p0.y;
		/* x = the vector from p0 to the beginning of y */
                double x = east_up ? ay : (north_up ? ax : (ax-c*ay)/(1-c/b));
                if (!east_up AND !north_up AND (x/b > 0)) continue;
		/* the length */
                x = fabs(x)*b2;
		/* y = length of the parallel to c vector from b to p */
                double y = east_up ? ax : (north_up ? ay : (ax-b*ay)/(1-b/c));
                if (!east_up AND !north_up AND (y/c < 0)) continue;
		/* the length */
                y = fabs(y)*c2;
		ral_cell_t cs; /* the coordinates in the clipped raster */
                cs.x = floor(x/cell_size);
		cs.y = floor(y/cell_size);
                /*
                if ((cs.y == 0 AND cs.x == 0) OR (cs.y == hs-1 AND cs.x == 0) OR 
                    (cs.y == 0 AND cs.x == ws-1) OR (cs.y == hs-1 AND cs.x == ws-1) OR
                    (ct.y == 0 AND ct.x == 0) OR (ct.y == height-1 AND ct.x == width-1)) {
		    fprintf(stderr, "p = %f %f\n", px, py);
		    fprintf(stderr, "a = %f %f\n", ax, ay);
		    fprintf(stderr, "x = %f\n", x);
		    fprintf(stderr, "y = %f\n", y);
		    fprintf(stderr, "copy %i, %i -> %i, %i\n", cs.x, cs.y, ct.x, ct.y);
		}
                */
                if (GCin(gd_s, cs)) {
                    GCiv(gd_t, ct) = GCiv(gd_s, cs);
                }
            }
        }
    }
    CPLPopErrorHandler();
    ral_delete(gd_s);
    return gd_t;
fail:
    CPLPopErrorHandler();
    ral_delete(gd_s);
    ral_delete(gd_t);
    return NULL;
}

int ral_grid_save_ascii(ral_grid_t *gd, char *outfile)
{
    const char *fct = "ral_gd2a";
    FILE *f = fopen(outfile,"w");
    if (!f) {
        fprintf(stderr,"%s: %s: %s\n", fct, outfile, strerror(errno)); 
        return 0;
    }
    /* the header */
    fprintf(f,"ncols         %i\n",gd->width);
    fprintf(f,"nrows         %i\n",gd->height);
    fprintf(f,"xllcorner     %lf\n",gd->world.min.x);
    fprintf(f,"yllcorner     %lf\n",gd->world.min.y);
    fprintf(f,"cellsize      %lf\n",gd->cell_size);

    if (gd->type == integer) {
        ral_cell_t c;
        fprintf(f,"NODATA_value  %i\n", gd->integer_no_data_value);
        for (c.y = 0; c.y < gd->height; c.y++) {
            for (c.x = 0; c.x < gd->width; c.x++) {
                fprintf(f,"%i ",GCiv(gd, c));
            }
            fprintf(f,"\n");
        }
    } else {
        ral_cell_t c;
        fprintf(f,"NODATA_value  %f\n", gd->real_no_data_value);
        for (c.y = 0; c.y < gd->height; c.y++) {
            for (c.x = 0; c.x < gd->width; c.x++) {
                fprintf(f,"%f ",GCnv(gd, c));
            }
            fprintf(f,"\n");
        }
    }
    
    fclose(f);
    return 1;
}

int RAL_CALL ral_grid_write(ral_grid_t *gd, char *filename, ral_error_t **e)
{
    FILE *f = NULL;
    size_t l = 0;

    if (!(f = fopen(filename,"wb")) && e) {
        *e = ral_error_new(0, "%s: %s\n", filename, strerror(errno));
        goto fail;
    }

    if (gd->type == integer)
        l = fwrite(gd->data,sizeof(RAL_INTEGER),gd->height*gd->width,f);
    else
        l = fwrite(gd->data,sizeof(RAL_REAL),gd->height*gd->width,f);

    if (l != gd->height*gd->width && e) {
        *e = ral_error_new(0, "File write failed.");
        goto fail;
    }

    fclose(f);
    return 1;
fail:
    if (f) fclose(f);
    return 0;
}

int ral_grid_print(ral_grid_t *gd)
{
    int i, j;
    if (gd->type == integer) {
        for (i = 0; i < gd->height; i++) {
            for (j = 0; j < gd->width; j++) {
                printf("%i ",Giv(gd, i, j));
            }
            printf("\n");
        }
    } else {
        for (i = 0; i < gd->height; i++) {
            for (j = 0; j < gd->width; j++) {
                printf("%f ",Gnv(gd, i, j));
            }
            printf("\n");
        }
    } 
    return 1;
}
