#ifndef	PRIV_CLASS_H
#define	PRIV_CLASS_H

/* abstract and private classes */
extern ral_class ralOrdinal;
extern ral_class ralVisInfo;

/* classes used as super classes */
extern const struct Class _Object;
extern const struct Class _Ordinal;
extern const struct Class _Integer;
extern const struct Class _Real;
extern const struct Class _Color;
extern const struct Class _Symbol;
extern const struct Class _Classifier;
extern const struct Class _Storage;
extern const struct Class _LinearInterpolation;
extern const struct Class _OutputDevice;
extern const struct Class _Feature;
extern const struct Class _Layer;

#define MAX_SUPER_CLASSES 4

struct Class {
    size_t size;
    const struct Class *super[MAX_SUPER_CLASSES];
    /*  1 */ ral_object_t *(*new)(ral_object_t *self);
    /*  2 */ ral_array_t *(*get_attributes)(ral_object_t *self);
    /*  3 */ void (*delete)(ral_object_t *self);
    /*  4 */ int (*set)(ral_object_t *self, ral_object_t *b);
    /*  5 */ void *free_slot2;
    /*  6 */ const char *(*name)();
    /*  7 */ long (*as_int)(ral_object_t *self);
    /*  8 */ double (*as_real)(ral_object_t *self);
    /*  9 */ void (*to_string)(ral_object_t *self);
    /* 10 */ void (*render)(ral_object_t *self, ral_output_device_t *pb, ral_hash_t * style, ral_error_t **e);
    /* 11 */ int (*cmp)(ral_object_t *self, ral_object_t *b);
    /* 12 */ int (*insert)(ral_object_t *self, ral_object_t *obj, ral_object_t *key);
    /* 13 */ unsigned (*number_of_elements)(ral_object_t *self);
    /* 14 */ void (* get_value)(ral_object_t *self, ral_object_t *value, const char *attr);
    /* 15 */ ral_object_t *(*lookup)(ral_object_t *self, ral_object_t *key);
    /* 16 */ int (*remove)(ral_object_t *self, ral_object_t *key);
    /* 17 */ ral_array_t *(*keys)(ral_object_t *self);
    /* 18 */ int (*classify)(ral_object_t *classifier, ral_object_t *output, ral_object_t *input);
    /* 19 */ ral_class (*get_attr_type)(ral_object_t *self, const char *attr);
    /* 20 */ ral_interval_t *(*get_value_range)(ral_object_t *self, const char *attr);
    /* 21 */ void *free_slot;
    /* 22 */ ral_class (*get_input_type)(ral_object_t *obj);
    /* 23 */ ral_class (*get_output_type)(ral_object_t *obj);
};

/* the contents of a wrap object should not be deleted in destructor */
/* the contents of the object as string after a call to as_string */

#define object_items                            \
    const struct Class *class;                  \
    int wrap;                                   \
    char *string                                

struct ral_object {
    object_items;
};

#define class_of(o) ((struct ral_object *)o)->class

struct ral_error {
    object_items;
    int level; /* 0: critical, 1: warning */
};

struct ral_ordinal {
    object_items;
};

struct ral_integer {
    object_items;
    int inf; /* -1 = -inf, 1 = inf */
    long value;
};

struct ral_real {
    object_items;
    int inf; /* -1 = -inf, 1 = inf */
    double value;    
};

struct ral_string {
    object_items;
    char *text;
};

struct ral_interval {
    object_items;
    int end_point_inclusion; /* 0 = () 1 = [) 2 = (] 3 = [] */
    ral_object_t *min;
    ral_object_t *max;
};

struct ral_data_point {
    object_items;
    ral_object_t *x;
    ral_object_t *y;
};

struct ral_classifier {
    object_items;
};

struct ral_linear_interpolation {
    object_items;
    ral_data_point_t *a;
    ral_data_point_t *b;
    double min, max, k; /* pre-computed for double, double coords */
};

struct ral_grayscale { /* is-a LinearInterpolation from iv or nv to gray value */
    struct ral_linear_interpolation super;
};

struct ral_hue_scale { /* is-a LinearInterpolation from iv or nv to hue value */
    struct ral_linear_interpolation super;
};

struct ral_color_component_scale { /* is-a LinearInterpolation from iv or nv to a single channel value of a color */
    struct ral_linear_interpolation super;
};

struct ral_color {
    object_items;
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short alpha;
};

struct ral_symbol {
    object_items;
    char *wkt;
};

struct ral_storage {
    object_items;
    /* maybe add here int strict_schema; */
};

struct ral_array {
    object_items;
    int size; /* size of the array */
    int largest_index;
    ral_object_t **array;
};

/* key - value pair */
struct ral_kvp {
    object_items;
    char *key;
    ral_object_t *value;
    ral_kvp_t *next;
};

struct ral_hash {
    object_items;
    size_t size_of_table; /* size of the table */
    ral_kvp_t **table; /* table of linked lists */
};

struct ral_bin {
    object_items;
    size_t size;
    ral_object_t **bins;
    ral_object_t **values;
};

struct ral_geometry {
    object_items;
    /** all actual points are only here, others are just pointers into this */
    ral_point_t *points;
    int n_points;
    /** parts[i].nodes points into points */
    ral_polygon_t *parts;
    int n_parts;
    OGRwkbGeometryType *part_types;
    OGRwkbGeometryType type;
};

struct ral_feature {
    object_items;
    OGRFeatureH ogr_feature;
    int destroy_geometry;  /** true if the geometry is _not_ a reference to the feature */
    ral_geometry_t *geometry;
    int is_undef;
    long lng_value;
    double dbl_value;
    const char *str_value;
};

int feature_geometry_collection_size(ral_feature_t *self);
int feature_prepare_geometry(ral_feature_t *self, int i);
int feature_geometry_type(ral_feature_t *feature);
void feature_set_value_from_field(ral_feature_t *feature, ral_object_t *value, char *field);

#define ral_style_feature_transparency "feature transparency"
#define ral_style_fill_color "fill color"
#define ral_style_border_color "border color"
#define ral_style_line_width "line width"
#define ral_style_symbol "symbol"
#define ral_style_symbol_size "symbol size"

extern const char * const visual_attribute[];

extern const char *ral_attribute_fid;
extern const char *ral_attribute_z_value;
extern const char *ral_attribute_m_value;
extern const char *ral_attribute_cell_value;

char *ral_stock_string(const char *str);
void ral_delete_non_stock_string(char *str);

struct ral_vis_info {
    object_items;
    char *attr; /* feature attribute */
    ral_object_t *value; /* of the feature attribute */
    ral_class visual_class; /* the class of the visual value */
    ral_object_t *deflt; /* default visual value */
    ral_object_t *rule; /* how to get visual from feature attribute */
    ral_object_t *visual; /* actual visual value */
};
typedef struct ral_vis_info ral_vis_info_t;

ral_vis_info_t *ral_vis_info_new(ral_class visual_class);

struct ral_layer { /* a geospatial layer */
    object_items;
    int layer_transparency;
    ral_hash_t *visual_information; /* keys are all known visual attributes */
};

struct ral_output_device {
    object_items;
    ral_rectangle_t world; /** geographic world */
};

/**\brief a cairo image canvas and a gdk style pixbuf

The idea is to first draw to the cairo image and then convert it into
a gdk pixbuf for GUI.

*/
struct ral_pixbuf {
    ral_output_device_t device;
    unsigned char *image; /** cairo image, each pixel is 4 bytes XRGB (BGRX if little endian) */
    int image_rowstride; /** rowstride of the cairo image */
    guchar *pixbuf; /** pixbuf data, each pixel is 3 bytes RGB, freed in pixbuf_destroy_notify */
    GdkPixbufDestroyNotify destroy_fn; /** needed for gdk pixbuf */
    GdkColorspace colorspace; /** needed for gdk pixbuf */
    gboolean has_alpha; /** needed for gdk pixbuf */
    int rowstride; /** needed for gdk pixbuf */
    int bits_per_sample; /** needed for gdk pixbuf */
    int width;
    int height;
    double pixel_size; /** size of pixel in geographic space */
    cairo_surface_t *surface;
    cairo_t *cr;
};

struct ral_rcoords {
    object_items;
    int x;
    int y;
};

struct ral_wcoords {
    object_items;
    double x;
    double y;
};

struct ral_wbox {
    object_items;
    double x_min;
    double y_min;
    double x_max;
    double y_max;
};

struct ral_grid {
    object_items;
    ral_grid_type_t type;
    int width;
    int height;
    double cell_size; /** cells are square */
    /** min.x is the left edge of first cell in line,
       max.x is the right edge of the last cell in a line */
    ral_rectangle_t world;
    int has_no_data_value;
    RAL_INTEGER integer_no_data_value;
    RAL_REAL real_no_data_value;
    void *data; /** is NULL if width * height M == 0 */
};

struct ral_raster { /* a geospatial raster layer, is-a layer*/
    ral_layer_t layer;
    ral_grid_t *grid;
};

struct ral_vector { /* a geospatial vector layer, is-a layer */
    ral_layer_t layer;
    OGRLayerH ogr_layer;
};

#endif
