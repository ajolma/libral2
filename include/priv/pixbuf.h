#ifndef	PIXBUF_H
#define	PIXBUF_H

#define PIXELSIZE 4
#define PIXBUF_PIXELSIZE 3

#define PBi(pb,i,j) (i)*(pb)->image_rowstride+(j)*PIXELSIZE

#define PB(pb,i,j,R,G,B)                                \
    {	(pb->image)[PBi(pb,i,j)+3] = 255;               \
        (pb->image)[PBi(pb,i,j)+2] = (R);		\
	(pb->image)[PBi(pb,i,j)+1] = (G);		\
	(pb->image)[PBi(pb,i,j)+0] = (B);	}

#define PBset(pb,i,j,c)                                                 \
    {   (pb->image)[PBi(pb,i,j)+3] = 255;                               \
        (pb->image)[PBi(pb,i,j)+2] =                                    \
	    MIN(MAX(((255-((c)->alpha))*(pb->image)[PBi(pb,i,j)+2]+((c)->alpha)*((c)->red))/255,0),255); \
	(pb->image)[PBi(pb,i,j)+1] =                                    \
	    MIN(MAX(((255-((c)->alpha))*(pb->image)[PBi(pb,i,j)+1]+((c)->alpha)*((c)->green))/255,0),255); \
	(pb->image)[PBi(pb,i,j)+0] =                                    \
	    MIN(MAX(((255-((c)->alpha))*(pb->image)[PBi(pb,i,j)+0]+((c)->alpha)*((c)->blue))/255,0),255);}

#define PBsetTest(pb,i,j,c)                                             \
    if ((i) >= 0 AND (i) < (pb)->height AND (j) >= 0 AND (j) < (pb)->width) \
        PBset((pb),(i),(j),(c))

#define PBsetCell(pb, pixel, color) PBset(pb,pixel.y,pixel.x,color)

#define PIXBUF_GET_PIXEL_COLOR(pb, pixel, color)                        \
    {                                                                   \
        (color)->alpha = (pb->image)[PBi(pb, (pixel).y, (pixel).x)+3];  \
        (color)->red = (pb->image)[PBi(pb, (pixel).y, (pixel).x)+2];    \
        (color)->green = (pb->image)[PBi(pb, (pixel).y, (pixel).x)+1];  \
        (color)->blue = (pb->image)[PBi(pb, (pixel).y, (pixel).x)+0];   \
    }

/** pixbuf coords to grid coords */
#define PBi2GDi(pb, i, gd)                                      \
    (floor((((gd)->world.max.y -                                \
             (pb->device.world.max.y -                          \
              (double)(i)*pb->pixel_size)))/(gd)->cell_size))

/** pixbuf coords to grid coords */
#define PBj2GDj(pb, j, gd) (floor(((pb->device.world.min.x +            \
                                    (double)(j)*pb->pixel_size) - (gd)->world.min.x)/(gd)->cell_size))

/** grid coordinates to pixbuf coords */
#define GDi2PBi(gd, i, pb)						\
    (floor((pb->device.world.max.y - (gd)->world.max.y + (gd)->cell_size * ((double)(i)+0.5))/pb->pixel_size))

/** grid coordinates to pixbuf coords */
#define GDj2PBj(gd, j, pb)						\
    (floor(((gd)->world.min.x - pb->device.world.min.x + (gd)->cell_size * ((double)(j)+0.5))/pb->pixel_size))

/** pixbuf coords to grid coords */
#define PIXEL2CELL(pixel, gd, cell)             \
    (cell).y = PBi2GDi(pb, (pixel).y, (gd));	\
    (cell).x = PBj2GDj(pb, (pixel).x, (gd))

#endif
