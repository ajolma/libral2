#include "ral2/ral.h"
#include "priv/priv.h"

ral_object_t *VisInfo_new(ral_object_t *_self)
{
    ral_vis_info_t *self = _self;
    self->visual_class = NULL;
    self->attr = NULL;
    self->value = NULL;
    self->rule = NULL;
    self->deflt = NULL;
    self->visual = NULL;
    return self;
}

void VisInfo_delete(ral_object_t *_self)
{
    ral_vis_info_t *self = _self;
    ral_delete_non_stock_string(self->attr);
    ral_delete(self->value);
    /* rule is owned by style param */
    /* deflt is owned by the style */
    ral_delete(self->visual);
}

ral_vis_info_t *ral_vis_info_new(ral_class visual_class)
{
    ral_vis_info_t *self = ral_new(ralVisInfo);
    self->visual_class = visual_class;
    return self;
}

static const struct Class _VisInfo = {
    sizeof(struct ral_vis_info),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ VisInfo_new,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
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

ral_class ralVisInfo = &_VisInfo;

ral_object_t *Layer_new(ral_object_t *_self)
{
    ral_layer_t *self = _self;
    self->layer_transparency = 0;
    self->visual_information = ral_new(ralHash);
    return self;
}

/* delete needed, rule is pointer to style hash, attr is pointer into
 * a static or into style hash, visual is rule if rule is NULL,
 * otherwise it is deletable, value is NULL, undef or deletable */

void Layer_delete(ral_object_t *_self)
{
    ral_layer_t * self = _self;
    ral_delete(self->visual_information);
}

/* visual style is by rule, default, or fall back */
void ral_layer_parse_style(ral_layer_t * self, ral_hash_t * style, const char *visual_attr, ral_vis_info_t *info)
{
    ral_object_t *this_style = ral_hash_lookup(style, visual_attr);
    if (this_style && class_of(this_style) == ralArray) {
        /* use given rule */
        char *attr = (char *)ral_as_string(ral_lookup(this_style, 0)); /* get feature attr */
        info->attr = ral_stock_string(attr); /* make sure it is a stock string if it is one */
        info->value = ral_new(ral_get_attr_type(self, info->attr));
        info->rule = ral_lookup(this_style, ral_integer_new(1));
        if (info->rule) { /* should check that it really is a rule */
            info->visual = ral_new(info->visual_class);
            /* pick up the default from the rule */
            ral_string_t *key = ral_string_new_wrap("*");
            info->deflt = ral_lookup(info->rule, key);
            ral_delete(key);
        } else ; /* really expected a working rule... */
    } else if (this_style && class_of(this_style) == info->visual_class) {
        /* use given default */
        info->deflt = this_style;
    } else {
        /* fall back */
        info->deflt = ral_new(info->visual_class);
        /* set the default? */
    }
}

void ral_layer_render(ral_object_t *layer, ral_output_device_t *device, ral_hash_t * style, ral_error_t **e)
{
    if (class_of(style) != ralHash) {
        if (e)
            *e = ral_error_new(1, "The argument 'style' is not a hash.");
        else
            fprintf(stderr, "The argument 'style' is not a hash.");
        return;
    }
    ral_layer_t *self = layer;
    ral_string_t *key = ral_string_new_wrap(ral_style_layer_transparency);
    ral_integer_t *transparency = ral_lookup(style, key);
    if (transparency && class_of(transparency) == ralInteger) 
        self->layer_transparency = transparency->value;
    int i;
    for (i = 0; strcmp(visual_attribute[i], "") != 0; i += 2) {
        ral_vis_info_t *info = ral_vis_info_new((ral_class)visual_attribute[i+1]);
        ral_layer_parse_style(self, style, visual_attribute[i], info);
        ral_string_wrap(key, visual_attribute[i]);
        ral_insert(self->visual_information, info, key);
    }
    ral_delete(key);
}

const struct Class _Layer = {
    sizeof(struct ral_layer),
    {&_Storage, NULL, NULL, NULL},
    /*  1 */ Layer_new,
    /*  2 */ NULL,
    /*  3 */ Layer_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ ral_layer_render,
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

ral_class ralLayer = &_Layer;

void call_render(ral_object_t *layer, const struct Class *class, ral_output_device_t *device, ral_hash_t *style, ral_error_t **e)
{
    if (class->render) {
        class->render(layer, device, style, e);
        return;
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i]) {
                class->super[i]->render(layer, device, style, e);
                return;
            }
    }
    fprintf(stderr, "Missing render method.\n");
}

/*! \brief Render the object on a cairo device.
 *
 *
 */
void ral_render(ral_object_t *layer, ral_output_device_t *device, ral_hash_t *style, ral_error_t **e)
{
    struct ral_object *obj = layer;
    call_render(obj, obj->class, device, style, e);
}
