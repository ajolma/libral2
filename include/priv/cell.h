#ifndef	CELL_H
#define	CELL_H

struct ral_cell {
    int x;
    int y;
};
typedef struct ral_cell ral_cell_t;

typedef int ral_cell_fct(ral_cell_t c, void *param);

/** 1 up, 2 up-right, 3 right, 4 down-right, etc */
ral_cell_t RAL_CALL ral_cell_move(ral_cell_t c, int dir); 

#define SAME_CELL(a, b)                         \
    ((a).x == (b).x AND (a).y == (b).y)

#define DISTANCE_BETWEEN_CELLS(a, b)            \
    (sqrt(((double)((b).x)-(double)((a).x)) *   \
          ((double)((b).x)-(double)((a).x)) +   \
          ((double)((b).y)-(double)((a).y)) *   \
          ((double)((b).y)-(double)((a).y))))

#define DISTANCE_BETWEEN_RCOORDS(a, b)                                  \
    (sqrt(((double)(((ral_rcoords_t*)(b))->x)-(double)(((ral_rcoords_t*)(a))->x)) * \
          ((double)(((ral_rcoords_t*)(b))->x)-(double)(((ral_rcoords_t*)(a))->x)) + \
          ((double)(((ral_rcoords_t*)(b))->y)-(double)(((ral_rcoords_t*)(a))->y)) * \
          ((double)(((ral_rcoords_t*)(b))->y)-(double)(((ral_rcoords_t*)(a))->y))))

#define RAL_DISTANCE_UNIT(dir) \
    (EVEN((dir)) ? SQRT2 : 1)

void ral_rect_in_src(ral_cell_t c, double *tr, ral_polygon_t P, int *bbox);

struct ral_window {
    ral_cell_t up_left; 
    ral_cell_t down_right;
};
typedef struct ral_window ral_window_t;

#define RAL_WINDOW_WIDTH(w) ((w).down_right.x-(w).up_left.x+1)
#define RAL_WINDOW_HEIGHT(w) ((w).down_right.y-(w).up_left.y+1)

#endif
