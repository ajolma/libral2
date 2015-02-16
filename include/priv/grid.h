#ifndef	GRID_H
#define	GRID_H

/* The implementation datatype for integer rasters */
#define RAL_INTEGER short
#define RAL_INTEGER_TYPE_NAME "short"

/* Minimum integer value */
#define RAL_INTEGER_MIN SHRT_MIN

/* Maximum integer value */
#define RAL_INTEGER_MAX SHRT_MAX

/* The implementation datatype for real rasters */
#define RAL_REAL float
#define RAL_REAL_TYPE_NAME "float"

/* is the cell in the grid */
#define GCin(gd, c)                                                     \
    ((c).y >= 0 AND (c).x >= 0 AND (c).y < (gd)->height AND (c).x < (gd)->width)

#define GCout(gd, c)                                                    \
    ((c).y < 0 OR (c).x < 0 OR (c).y >= (gd)->height OR (c).x >= (gd)->width)

#define FOR(c, gd)                                      \
    for((c).y = 0; (c).y < (gd)->height; (c).y++)       \
        for((c).x = 0; (c).x < (gd)->width; (c).x++)

#define GRID_INDEX(i, j, N) (j)+(N)*(i)

#define Giv(gd, i, j) \
    (((RAL_INTEGER *)((gd)->data))[GRID_INDEX((i),(j),((gd)->width))])

#define Gnv(gd, i, j) \
    (((RAL_REAL *)((gd)->data))[GRID_INDEX((i),(j),((gd)->width))])

/* integer value from a grid cell */
#define GCiv(gd, c)                                                     \
    (((RAL_INTEGER *)((gd)->data))[GRID_INDEX(((c).y),((c).x),((gd)->width))])

#define GCivs(gd, c, buffer)                    \
    sprintf((buffer), "%d", GCiv(gd, c))

#define INTEGER_GRID_SET_CELL(grid, cell, value) GCiv((grid), (cell)) = (value)

/* float value from a grid cell */
#define GCnv(gd, c)                                                     \
    (((RAL_REAL *)((gd)->data))[GRID_INDEX(((c).y),((c).x),((gd)->width))])

#define REAL_GRID_SET_CELL(grid, cell, value) GCnv((grid), (cell)) = (value)

/* value from a grid cell */
#define GCv(gd, c)                                      \
    ((gd)->type == integer ? GCiv(gd, c) : GCnv(gd, c))

#define GCivD(gd, c)                                    \
    ((gd)->has_no_data_value ?                          \
     (GCiv((gd), (c)) != (gd)->integer_no_data_value) : \
     TRUE)

#define GCnvD(gd, c)                                    \
    ((gd)->has_no_data_value ?                          \
     (GCnv((gd), (c)) != (gd)->real_no_data_value) :    \
     TRUE)

#define GCD(gd, c)                                              \
    ((gd)->has_no_data_value ?                                  \
     ((gd)->type == integer ?                                   \
      (GCiv((gd), (c)) != (gd)->integer_no_data_value) :        \
      (GCnv((gd), (c)) != (gd)->real_no_data_value)) : TRUE)

/* integer grid cell has no data */
#define GCivND(gd, c) (!(GCivD(gd, c)))

/* float grid cell has no data */
#define GCnvND(gd, c) (!(GCnvD(gd, c)))

#define GCND(gd, c)                             \
    ((gd)->has_no_data_value ?                  \
     ((gd)->type == integer ?                   \
      GCivND(gd, c) :                           \
      GCnvND(gd, c)) :                          \
     FALSE)

/** Bresenham as presented in Foley & Van Dam */
#define LINE(grid, cell1, cell2, pen, assignment)               \
            {						        \
             ral_cell_t c;					\
             int di, dj, incr1, incr2, d,			\
                 iend, jend, idirflag, jdirflag;		\
             cell1.y = MAX(MIN(cell1.y,grid->height-1),0);	\
             cell1.x = MAX(MIN(cell1.x,grid->width-1),0);	\
             cell2.y = MAX(MIN(cell2.y,grid->height-1),0);	\
             cell2.x = MAX(MIN(cell2.x,grid->width-1),0);	\
             di = abs(cell2.y-cell1.y);                         \
             dj = abs(cell2.x-cell1.x);                         \
             if (dj <= di) {					\
                 d = 2*dj - di;                                 \
                 incr1 = 2*dj;                                  \
                 incr2 = 2 * (dj - di);                         \
                 if (cell1.y > cell2.y) {			\
                     c.y = cell2.y;				\
                     c.x = cell2.x;				\
                     jdirflag = (-1);                           \
                     iend = cell1.y;				\
                 } else {					\
                     c.y = cell1.y;				\
                     c.x = cell1.x;				\
                     jdirflag = 1;				\
                     iend = cell2.y;				\
                 }						\
                 assignment(grid, c, pen);			\
                 if (((cell2.x - cell1.x) * jdirflag) > 0) {	\
                     while (c.y < iend) {			\
                         c.y++;                                 \
                         if (d <0) {				\
                             d+=incr1;                          \
                         } else {				\
                             c.x++;				\
                             d+=incr2;                          \
                         }					\
                         assignment(grid, c, pen);		\
                     }                                          \
                 } else {					\
                     while (c.y < iend) {			\
                         c.y++;                                 \
                         if (d <0) {				\
                             d+=incr1;                          \
                         } else {				\
                             c.x--;				\
                             d+=incr2;                          \
                         }					\
                         assignment(grid, c, pen);		\
                     }                                          \
                 }						\
             } else {                                           \
                 d = 2*di - dj;                                 \
                 incr1 = 2*di;                                  \
                 incr2 = 2 * (di - dj);                         \
                 if (cell1.x > cell2.x) {			\
                     c.x = cell2.x;				\
                     c.y = cell2.y;				\
                     jend = cell1.x;				\
                     idirflag = (-1);                           \
                 } else {					\
                     c.x = cell1.x;				\
                     c.y = cell1.y;				\
                     jend = cell2.x;				\
                     idirflag = 1;				\
                 }						\
                 assignment(grid, c, pen);			\
                 if (((cell2.y - cell1.y) * idirflag) > 0) {	\
                     while (c.x < jend) {			\
                         c.x++;                                 \
                         if (d <0) {				\
                             d+=incr1;                          \
                         } else {				\
                             c.y++;				\
                             d+=incr2;                          \
                         }					\
                         assignment(grid, c, pen);		\
                     }                                          \
                 } else {					\
                     while (c.x < jend) {			\
                         c.x++;                                 \
                         if (d <0) {				\
                             d+=incr1;                          \
                         } else {				\
                             c.y--;				\
                             d+=incr2;                          \
                         }					\
                         assignment(grid, c, pen);		\
                     }                                          \
                 }						\
             }                                                  \
            }

/** from somewhere in the net, does not look very good if r is small(?) */
#define FILLED_CIRCLE(grid, c, r, pen, assignment)              \
    {                                                           \
     int a, b, di, dj, r2 = r*r;                                \
     di = MAX(-r, -(c).y);                                      \
     a = r2 - di*di;                                            \
     while (1) {                                                \
         dj = MAX(-r, -(c).x);                                  \
         b = dj*dj;                                             \
         while (1) {                                            \
             ral_cell_t d;                                      \
             d.y = (c).y+di;                                    \
             d.x = (c).x+dj;                                    \
             if (d.x >= (grid)->width) break;                   \
             if (b < a) assignment(grid, d, pen);               \
             dj++;                                              \
             if (dj > r) break;                                 \
             b += 2*dj - 1;                                     \
         }                                                      \
         di++;                                                  \
         if (di > r OR (c).y+di >= (grid)->height) break;	\
         a -= 2*di - 1;                                         \
     }                                                          \
    }

typedef struct {
    float weight;
    int dest;
} ral_DijkEdge;

typedef struct {
    ral_DijkEdge* connections; /* An array of edges which has this as the starting node */
    int numconnect;
    float distance;
    int isDead;
} ral_DijkVertex;

#endif
