#include "ral2/ral.h"
#include "priv/priv.h"

ral_cell_t ral_cell_move(ral_cell_t c, int dir) 
{
    switch (dir) {
    case 1:{ c.y--;         break; }
    case 2:{ c.y--; c.x++;  break; }
    case 3:{ c.x++;         break; }
    case 4:{ c.y++; c.x++;  break; }
    case 5:{ c.y++;         break; }
    case 6:{ c.y++; c.x--;  break; }
    case 7:{ c.x--;         break; }
    case 8:{ c.y--; c.x--;  break; }
    }
    return c;
}

int ral_cell_dir(ral_cell_t a, ral_cell_t b)
{
    if (b.y < a.y AND b.x == a.x)
        return 1;
    else if (b.y < a.y AND b.x > a.x)
        return 2;
    else if (b.y == a.y AND b.x > a.x)
        return 3;
    else if (b.y > a.y AND b.x > a.x)
        return 4;
    else if (b.y > a.y AND b.x == a.x)
        return 5;
    else if (b.y > a.y AND b.x < a.x)
        return 6;
    else if (b.y == a.y AND b.x < a.x)
        return 7;
    else 
        return 8;
}

/* 
   this computes the area (polygon P) that the cell c (of destination
   raster) covers in source raster and its bounding box

   is this now a bit different than elsewhere?
   
   should we assume that tr translates center points of cells?
 */
void ral_rect_in_src(ral_cell_t c, double *tr, ral_polygon_t P, int *bbox)
{
    P.nodes[0].y = tr[0]+tr[1]*((double)c.y-0.0)+tr[2]*((double)c.x-0.0);
    bbox[2] = bbox[0] = ceil(P.nodes[0].y);
    P.nodes[0].x = tr[3]+tr[4]*((double)c.y-0.0)+tr[5]*((double)c.x-0.0);
    bbox[3] = bbox[1] = ceil(P.nodes[0].x);
    P.nodes[1].y = tr[0]+tr[1]*((double)c.y-0.0)+tr[2]*((double)c.x+1.0);
    bbox[0] = MIN(bbox[0],ceil(P.nodes[1].y));
    bbox[2] = MAX(bbox[2],ceil(P.nodes[1].y));
    P.nodes[1].x = tr[3]+tr[4]*((double)c.y-0.0)+tr[5]*((double)c.x+1.0);
    bbox[1] = MIN(bbox[1],floor(P.nodes[1].x));
    bbox[3] = MAX(bbox[3],floor(P.nodes[1].x));
    P.nodes[2].y = tr[0]+tr[1]*((double)c.y+1.0)+tr[2]*((double)c.x+1.0);
    bbox[0] = MIN(bbox[0],floor(P.nodes[2].y));
    bbox[2] = MAX(bbox[2],floor(P.nodes[2].y));
    P.nodes[2].x = tr[3]+tr[4]*((double)c.y+1.0)+tr[5]*((double)c.x+1.0);
    bbox[1] = MIN(bbox[1],floor(P.nodes[2].x));
    bbox[3] = MAX(bbox[3],floor(P.nodes[2].x));
    P.nodes[3].y = tr[0]+tr[1]*((double)c.y+1.0)+tr[2]*((double)c.x-0.0);
    bbox[0] = MIN(bbox[0],floor(P.nodes[3].y));
    bbox[2] = MAX(bbox[2],floor(P.nodes[3].y));
    P.nodes[3].x = tr[3]+tr[4]*((double)c.y+1.0)+tr[5]*((double)c.x-0.0);
    bbox[1] = MIN(bbox[1],ceil(P.nodes[3].x));
    bbox[3] = MAX(bbox[3],ceil(P.nodes[3].x));
}
