#include "ral2/ral.h"
#include "priv/priv.h"

void *Feature_new(void *_self)
{
    ral_feature_t *self = _self;
    self->ogr_feature = NULL;
    self->geometry = ral_new(ralGeometry);
    return (void *)self;
}

void Feature_delete(void *_self)
{
    ral_feature_t *self = _self;
    if (self->geometry) ral_delete(self->geometry);
    return;
}

const char *Feature_name()
{
    return "Feature";
}

const struct Class _Feature = {
    sizeof(struct ral_feature),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ Feature_new,
    /*  2 */ NULL,
    /*  3 */ Feature_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Feature_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ ral_feature_get_value,
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

ral_class ralFeature = &_Feature;

int feature_geometry_collection_size(ral_feature_t *self)
{
    OGRGeometryH geometry = OGR_F_GetGeometryRef(self->ogr_feature);
    if (wkbFlatten(OGR_G_GetGeometryType(geometry)) == wkbGeometryCollection)
        return OGR_G_GetGeometryCount(geometry);
    else
        return 1;
}

int feature_prepare_geometry(ral_feature_t *self, int i)
{
    OGRGeometryH geometry = OGR_F_GetGeometryRef(self->ogr_feature);
    if (wkbFlatten(OGR_G_GetGeometryType(geometry)) == wkbGeometryCollection)
        geometry = OGR_G_GetGeometryRef(geometry, i);
    ral_geometry_set_from_ogr_geometry(self->geometry, geometry);
    return self->geometry ? 1 : 0;
}

int feature_geometry_type(ral_feature_t *self)
{
    return OGR_G_GetGeometryType(OGR_F_GetGeometryRef(self->ogr_feature));
}

void ral_feature_get_value(ral_object_t *feature,
                           ral_object_t *value, 
                           const char *attr)
{
    ral_feature_t *self = feature;
    if (attr) {
        int index = OGR_F_GetFieldIndex(self->ogr_feature, attr);
        if (class_of(value) == ralInteger)
            ((ral_integer_t *)value)->value = (long)OGR_F_GetFieldAsInteger(self->ogr_feature, index);
        else if (class_of(value) == ralReal)
            ((ral_real_t *)value)->value = OGR_F_GetFieldAsDouble(self->ogr_feature, index);
        else if (class_of(value) == ralString)
            ral_string_set((ral_string_t *)value, OGR_F_GetFieldAsString(self->ogr_feature, index));
    }
}

/*! \brief The attributes.
 *
 * A feature contains data associated with attributes.
 *
 * \returns Returns the attributes in an array.
 */
ral_array_t *ral_get_attributes(ral_object_t *feature)
{
    struct ral_object *obj = feature;
    if (obj) {
        if (obj->class->get_attributes)
            return obj->class->get_attributes(feature);
        else if (obj->class->super && obj->class->super[0]->get_attributes)
            return obj->class->super[0]->get_attributes(feature);
        else
            fprintf(stderr, "assert: missing get attributes method (class = %s)\n", ral_class_name(obj->class));
    }
    return NULL;
}

/*! \brief The class of an attribute.
 *
 * This method returns the type of the data associated with a specific
 * attribute.
 *
 * \returns Returns the class.
 */
ral_class ral_get_attr_type(ral_object_t *feature, const char *attr)
{
    struct ral_object *obj = feature;
    if (obj && obj->class->get_attr_type)
        return obj->class->get_attr_type(obj, attr);
    else {
        assert(0 && "missing get attr type method");
        return NULL;
    }
}
