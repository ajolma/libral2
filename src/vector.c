#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *ral_vector_new(ral_object_t *_self)
{
    ral_vector_t *self = _self;
    return self;
}

void ral_vector_delete(ral_object_t *_self)
{
    /*ral_vector_t *self = _self;*/
}

const char *ral_vector_name()
{
    return "Vector";
}

ral_class OGRFieldType2Class(OGRFieldType t)
{
    switch (t) {
    case OFTInteger:
        return ralInteger;
        break;
    case OFTIntegerList:
    case OFTRealList:
    case OFTStringList:
    case OFTWideStringList:
        return ralArray;
        break;
    case OFTReal:
        return ralReal;
        break;
    case OFTString:
    case OFTWideString:
        return ralString;
        break;
    case OFTBinary:
        return ralString;
        break;
    case OFTDate:
        return ralString;
        break;
    case OFTTime:
        return ralString;
        break;
    case OFTDateTime:
        return ralString;
        break;
    }
    return NULL;
}

ral_object_t *OGRField2Object(OGRFeatureH f, int index, OGRFieldType t)
{
    switch (t) {
    case OFTInteger:
        return ral_integer_new(OGR_F_GetFieldAsInteger(f, index));
    case OFTIntegerList:
    case OFTRealList:
    case OFTStringList:
    case OFTWideStringList:
        return NULL;
    case OFTReal:
        return ral_real_new(OGR_F_GetFieldAsDouble(f, index));        
    case OFTString:
    case OFTWideString:
        return ral_string_new(OGR_F_GetFieldAsString(f, index));
    case OFTBinary:
        return ral_string_new(OGR_F_GetFieldAsString(f, index));
    case OFTDate:
        return ral_string_new(OGR_F_GetFieldAsString(f, index));
    case OFTTime:
        return ral_string_new(OGR_F_GetFieldAsString(f, index));
    case OFTDateTime:
        return ral_string_new(OGR_F_GetFieldAsString(f, index));
    }
    return NULL;
}

ral_class ral_vector_get_attr_type(ral_object_t *_self, const char *attr)
{
    ral_vector_t *self = _self;
    if (strcmp(attr, ral_attribute_fid) == 0)
        return ralInteger;
    else if (strcmp(attr, ral_attribute_z_value) == 0)
        return ralReal;
    else if (strcmp(attr, ral_attribute_m_value) == 0)
        return ralReal;
    if (!self->ogr_layer) return NULL;
    OGRFeatureDefnH defn = OGR_L_GetLayerDefn(self->ogr_layer);
    int index = OGR_L_FindFieldIndex(self->ogr_layer, attr, 1);
    if (index >= 0) {
        OGRFieldDefnH fd = OGR_FD_GetFieldDefn(defn, index);
        OGRFieldType t = OGR_Fld_GetType(fd);
        return OGRFieldType2Class(t);
    } else
        return NULL;
}

ral_array_t *ral_vector_get_attributes(ral_object_t *_self)
{
    ral_vector_t *self = _self;
    ral_array_t *a = ral_new(ralArray);
    ral_array_insert(a, ral_string_new_wrap(ral_attribute_fid), 0);
    ral_array_insert(a, ral_string_new_wrap(ral_attribute_z_value), 1); /* if data has it */
    ral_array_insert(a, ral_string_new_wrap(ral_attribute_m_value), 2); /* if data has it */
    OGRFeatureDefnH defn = OGR_L_GetLayerDefn(self->ogr_layer);
    int i;
    for (i = 0; i < OGR_FD_GetFieldCount(defn); i++) {
        OGRFieldDefnH fd = OGR_FD_GetFieldDefn(defn, i);
        ral_array_insert(a, ral_string_new(OGR_Fld_GetNameRef(fd)), 3+i);
    }
    return a;
}

ral_interval_t *ral_vector_get_value_range(ral_object_t *_self, const char *attr)
{
    ral_vector_t *self = _self;
    ral_interval_t *ab = NULL;
    if (!self->ogr_layer) return NULL;
    OGRFeatureDefnH defn = OGR_L_GetLayerDefn(self->ogr_layer);
    int index = OGR_L_FindFieldIndex(self->ogr_layer, attr, 1);
    if (index >= 0) {
        OGRFieldDefnH fd = OGR_FD_GetFieldDefn(defn, index);
        OGRFieldType t = OGR_Fld_GetType(fd);
        OGR_L_ResetReading(self->ogr_layer);
        OGRFeatureH feature; 
        while ((feature = OGR_L_GetNextFeature(self->ogr_layer))) {
            ral_object_t *d = OGRField2Object(feature, index, t);
            if (ab) {
                if (ral_cmp(d, ab->min) < 0)
                    ral_set(ab->min, d);
                else if (ral_cmp(d, ab->max) > 0)
                    ral_set(ab->max, d);
            } else
                ab = ral_interval_new(ral_clone(d), ral_clone(d), 3);
            ral_delete(d);
        }
    }
    return ab;
}

void ral_vector_render(ral_object_t *_self, ral_output_device_t *device, ral_hash_t * style, ral_error_t **e)
{
    ral_error_t *e0 = NULL;
    ral_error_t **e2 = e ? e : &e0;
    ralLayer->render(_self, device, style, e2);
    if (*e2) {
        if (!e) 
            fprintf(stderr, "%s\n", ral_as_string(*e2));
        return;
    }
    ral_vector_t *self = _self;
    OGRLayerH ogr = self->ogr_layer;
    ral_pixbuf_t *pb = (ral_pixbuf_t *)device;
    
    ral_vis_info_t *ifc = ral_hash_lookup(self->layer.visual_information, ral_style_fill_color);
    ral_vis_info_t *ibc = ral_hash_lookup(self->layer.visual_information, ral_style_border_color);
    ral_vis_info_t *isymbol = ral_hash_lookup(self->layer.visual_information, ral_style_symbol);
    ral_vis_info_t *isymbol_size = ral_hash_lookup(self->layer.visual_information, ral_style_symbol_size);
    ral_vis_info_t *iline_width = ral_hash_lookup(self->layer.visual_information, ral_style_line_width);

    CPLPushErrorHandler(ral_cpl_error);
    OGR_L_SetSpatialFilterRect(ogr, device->world.min.x, device->world.min.y, device->world.max.x, device->world.max.y);
    OGR_L_ResetReading(ogr);
    
    ral_feature_t *feature = ral_new(ralFeature);
    while ((feature->ogr_feature = OGR_L_GetNextFeature(ogr))) {

        ral_color_t *fc = ifc->deflt;
        if (ifc->visual) {
            ral_feature_get_value(feature, ifc->value, ifc->attr);
            if (ral_classify(ifc->rule, ifc->visual, ifc->value))
                fc = ifc->visual;
        }
        ral_color_t *bc = ibc->deflt;
        if (ibc->visual) {
            ral_feature_get_value(feature, ibc->value, ibc->attr);
            if (ral_classify(ibc->rule, ibc->visual, ibc->value))
                bc = ibc->visual;
        }
        ral_real_t *line_width = iline_width->deflt;
        if (iline_width->visual) {
            ral_feature_get_value(feature, iline_width->value, iline_width->attr);
            if (ral_classify(iline_width->rule, iline_width->visual, iline_width->value))
                line_width = iline_width->visual;
        }
        ral_symbol_t *symbol = isymbol->deflt;
        if (isymbol->visual) {
            ral_feature_get_value(feature, isymbol->value, isymbol->attr);
            if (ral_classify(isymbol->rule, isymbol->visual, isymbol->value))
                symbol = isymbol->visual;
        }
        ral_integer_t *symbol_size = isymbol_size->deflt;
        if (isymbol_size->visual) {
            ral_feature_get_value(feature, isymbol_size->value, isymbol_size->attr);
            if (ral_classify(isymbol_size->rule, isymbol_size->visual, isymbol_size->value))
                symbol_size = isymbol_size->visual;
        }

        /*fprintf(stderr, "%s: color %s\n", as_string(self->layer.fill_color.value), as_string(fc));*/
        int i, n = feature_geometry_collection_size(feature);
        for (i = 0; i < n; i++) {
            FAIL_UNLESS(feature_prepare_geometry(feature, i));
            switch (feature->geometry->type) {
            case wkbUnknown:
            case wkbNone:
                break;
            case wkbPoint:
            case wkbMultiPoint:
            case wkbPoint25D:
            case wkbMultiPoint25D:
                if (symbol->wkt == ral_symbol_square) {
                    FAIL_UNLESS(ral_pixbuf_render_squares(pb, feature->geometry, symbol_size->value, bc, fc));
                } else if (symbol->wkt == ral_symbol_dot) {
                    FAIL_UNLESS(ral_pixbuf_render_dots(pb, feature->geometry, symbol_size->value, fc));
                } else if (symbol->wkt == ral_symbol_wind_rose) {
                    FAIL_UNLESS(ral_pixbuf_render_wind_roses(pb, feature, fc));
                } else {
                    FAIL_UNLESS(ral_pixbuf_render_crosses(pb, feature->geometry, symbol_size->value, fc));
                }
                break;
            case wkbLineString:
            case wkbMultiLineString:
            case wkbLineString25D:
            case wkbMultiLineString25D:
                if (fc) {
                    if (line_width) cairo_set_line_width(pb->cr, ral_as_real(line_width));
                    FAIL_UNLESS(ral_pixbuf_render_polylines(pb, feature->geometry, fc));
                }
                break;
            case wkbPolygon:
            case wkbMultiPolygon:
            case wkbPolygon25D:
            case wkbMultiPolygon25D:
                if (fc) FAIL_UNLESS(ral_pixbuf_render_polygons(pb, feature->geometry, fc));
                if (bc) {
                    if (line_width) cairo_set_line_width(pb->cr, ral_as_real(line_width));
                    FAIL_UNLESS(ral_pixbuf_render_polylines(pb, feature->geometry, bc));
                }
                break;
            default:
                fprintf(stderr, "warning: a request to render geometry of type %i\n", feature->geometry->type);
            }
        }

        OGR_F_Destroy(feature->ogr_feature);
    }
fail:
    ral_delete(feature);
    OGR_L_SetSpatialFilter(ogr, NULL);
    CPLPopErrorHandler();
}

static const struct Class _Vector = {
    sizeof(struct ral_vector),
    {&_Layer, NULL, NULL, NULL},
    /*  1 */ ral_vector_new,
    /*  2 */ ral_vector_get_attributes,
    /*  3 */ ral_vector_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ ral_vector_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ ral_vector_render,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ ral_vector_get_attr_type,
    /* 20 */ ral_vector_get_value_range,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralVector = &_Vector;

ral_vector_t * ral_vector_new_from_OGR(OGRLayerH ogr_layer)
{
    ral_vector_t * v = ral_new(ralVector);
    if (v)
        v->ogr_layer = ogr_layer;
    return v;
}
