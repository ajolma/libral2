#ifndef	DEM_H
#define	DEM_H

#define FLOW(fdg, c) (ral_cell_move((c), GCiv((fdg), (c))))

#define RAL_DIRECTIONS(dir) \
    for ((dir) = 1; (dir) < 9; (dir)++)

#define RAL_NEXT_DIR(d) \
    ((d) > 7 ? 1 : (d) + 1)

#define RAL_PREV_DIR(d) \
    ((d) > 1 ? (d) - 1 : 8)

#define RAL_INV_DIR(d) \
    ((d) > 4 ? (d) - 4 : (d) + 4)

struct pour_point {
    /** flow dir grid */
    ral_grid_t *fdg;
    /** dem grid */
    ral_grid_t *dem;
    /** current flat area or pit cell's catchment */
    ral_grid_t *mark;
    /** elevation of the flat area */
    double zf;
    /** flatness threshold */
    double dz;
    /** a border cell after marking the area, start the walk from here */
    ral_cell_t bc;
    /** bc is valid */
    int bc_found;
    /** direction away from the area, in topological sense?, suitable for borderwalk */
    int dir_out;
    /** when looking for the pour point, do we
	look for lowest border cell (default) or lowest cell adjacent to the area? */
    int test_inner;
    /** inner pour point found? */
    int pp_found;
    /** inner pour point */
    ral_cell_t ipp;
    /** elevation of the inner pour point */
    double z_ipp;
    /** slope from the inner pour point to its downslope point in the depression */
    double slope_in;
    /** slope from the inner pour point to the the outer pour point */
    double slope_out;
    /** distance from the ipp to opp */
    int dio;
    /** direction from inner_pour_point to outer_pour_point */
    int in2out;
    /** pour to no_data? */
    int pour_to_no_data;
    /** is valid only if pour_to_no_data is false */
    ral_cell_t opp;
    /** elevation of the outer pour point */
    double z_opp;
    int counter;
};

typedef struct pour_point pour_point_t;

struct catchment {
    /** outlet cell of catchment i, i = 0..n-1 */
    ral_cell_t *outlet;
    /** outlet cell of the catchment into which catchment i drains */
    ral_cell_t *down;
    int n;
    int size;
    int delta;
};

typedef struct catchment catchment_t;

struct slopes {
    ral_grid_t *fdg;             /* flow dir grid */
    ral_grid_t *dem;             /* dem grid */
    ral_grid_t *flat;            /* all flat area */
    ral_grid_t *ss;              /* slope sums */
    ral_grid_t *uag;
    double dz;             /* flatness threshold */
};

typedef struct slopes slopes_t;

struct lakedata {
    ral_grid_t *lakes;
    ral_grid_t *streams;
    ral_grid_t *fdg;
    ral_grid_t *uag;
    ral_grid_t *mark;
    int lid;       /* lake, whose next-to-shore cells we are boating */
    int sid;       /* stored stream id */
    int nsid;      /* next available stream id */
    double min_l;  /* min_l in pruning */
    int outlet_found;
    double max_ua; /* maximum upslope area */
    ral_cell_t outlet;   /* the cell with max_ua */
    int bc_found;  /* as bc in ral_pour_point */
    ral_cell_t bc;
    int dir_out;
    int counter;
};

typedef struct lakedata lakedata_t;

struct treedata {
    pour_point_t pp;
    /* fill these when calling tree or lake */
    catchment_t *catchment;
    ral_grid_t *subs;
    ral_grid_t *streams;
    ral_grid_t *lakes;
    int k;
    int headwaters; /* are headwaters marked as separate subs */
    /* these are needed only in internal routines */
    int lid;
    ral_cell_t a;
    ral_cell_t last_stream_section_end;
};

typedef struct treedata treedata_t;

#endif
