#include "ral2/ral.h"
#include "priv/priv.h"

void *Raster_new(void *self)
{
    ((ral_raster_t *)self)->grid = NULL;
    return self;
}

void Raster_delete(void *_self)
{
    /*raster_t self = _self;*/
}

const char *Raster_name()
{
    return "Raster";
}

ral_array_t *Raster_get_attributes(ral_object_t *self)
{
    ral_array_t *a = ral_new(ralArray);
    ral_array_insert(a, ral_string_new_wrap(ral_attribute_cell_value), 0);
    /* add here RAT */
    return a;
}

ral_class Raster_get_attr_type(void *_self, const char *attr)
{
    ral_raster_t *self = _self;
    if (strcmp(attr, ral_attribute_cell_value) == 0) {
        if (self->grid->type == integer)
            return ralInteger;
        else
            return ralReal;
    }
    return NULL;
}

ral_interval_t *Raster_get_value_range(void *_self, const char *attr)
{
    ral_raster_t * self = _self;
    if (self->grid) {
        return ral_grid_get_value_range(self->grid);
    }
    return NULL;
}

int Raster_classify(void *self, void *val, void *attr)
{
    return 0; 
}

#ifdef XX
void ral_raster_mult_raster(ral_raster_t * r1, ral_raster_t * r2, ral_error *e)
{
    test_overlay(r1, r2, e); /* macro, return if test fails */
    ral_cell_t c;
    if (class_of(r1) == IntegerRaster && class_of(r2) == IntegerRaster) {
        FOR(c, r1) {
            if (RCivD(r1, c) && RCivD(r2, c)) {
                long r = (long)RCiv(r1, c) * (long)RCiv(r2, c);
                test_iv(r, e); /* macro, return if test fails */
                RCiv(r1, c) = r;
            } else
                RCiv_set(r1, c, ral_undef, e);
        }
    }
}
#endif

ral_array_t * Raster_layer_attributes(ral_raster_t * self)
{
    ral_array_t * a = ral_new(ralArray);
    ral_string_t *s = ral_new(ralString);
    ral_integer_t * i = ral_new(ralInteger);
    ral_string_wrap(s, ral_attribute_cell_value);
    i->value = 0;
    ral_insert(a, s, i);
    ral_delete(i);
    ral_delete(s);
    return a;
}

void get_cell_value(ral_raster_t * self, ral_cell_t c, void *value, const char *attr)
{
    ral_grid_t *gd = self->grid;
    if (attr == ral_attribute_cell_value) {
        if (gd->type == integer) {
            ((ral_integer_t *)value)->value = (long)GCiv(gd, c);
        }
    }
}

void Raster_render(ral_object_t *_self, ral_output_device_t *device, ral_hash_t *style, ral_error_t **e)
{
    ral_error_t *e0 = NULL;
    ral_error_t **e2 = e ? e : &e0;
    ralLayer->render(_self, device, style, e2);
    if (*e2) {
        if (!e) 
            fprintf(stderr, "%s\n", ral_as_string(*e2));
        return;
    }
    ral_raster_t *self = _self;
    ral_grid_t *gd = self->grid;
    ral_pixbuf_t *pb = (ral_pixbuf_t *)device;

    ral_vis_info_t *ifc = ral_hash_lookup(self->layer.visual_information, ral_style_fill_color);
    ral_vis_info_t *isymbol = ral_hash_lookup(self->layer.visual_information, ral_style_symbol);
    ral_vis_info_t *isymbol_size = ral_hash_lookup(self->layer.visual_information, ral_style_symbol_size);

    ral_cell_t pixel, c;
    if (!isymbol->visual) {        
        for (pixel.y = 0; pixel.y < pb->height; pixel.y++) {
            c.y = PBi2GDi(pb, pixel.y, gd);
            for (pixel.x = 0; pixel.x < pb->width; pixel.x++) {
                c.x = PBj2GDj(pb, pixel.x, gd);
                /*fprintf(stderr, "cell %i, %i\n", c.y, c.x);*/
                if (GCin(gd, c) AND GCD(gd, c)) {
                    
                    /* Setting the style: 1) use a rule, if there is
                     * one, 2) if the rule does not match or there is
                     * no rule, use the default. */
                    
                    ral_color_t *fc = ifc->deflt;
                    if (ifc->visual) {
                        get_cell_value(self, c, ifc->value, ifc->attr);
                        if (ral_classify(ifc->rule, ifc->visual, ifc->value))
                            fc = ifc->visual;
                    }

                    /*fprintf(stderr, "color = %s\n", as_string(fill_color));*/
                    PBsetCell(pb, pixel, fc);
                    /* restore color->alpha if alpha, needed for single color */
                }
            }
        }
    } 
    else if (((ral_symbol_t *)(isymbol->deflt))->wkt == ral_symbol_flow_direction)
        ral_pixbuf_render_grid_flow_directions(pb, gd, ifc->deflt);
    else {
        ral_cell_t c, pixel;
        for(c.y = 0; c.y < gd->height; c.y++) {
            pixel.y = GDi2PBi(gd, c.y, pb);
            if ((pixel.y < 0) OR (pixel.y >= pb->height)) continue;
            for(c.x = 0; c.x < gd->width; c.x++) {
                if (GCND(gd, c)) continue;
                pixel.x = GDj2PBj(gd, c.x, pb);
                if ((pixel.x < 0) OR (pixel.x >= pb->width)) continue;

                ral_color_t *fc = ifc->deflt;
                if (ifc->visual) {
                    get_cell_value(self, c, ifc->value, ifc->attr);
                    if (ral_classify(ifc->rule, ifc->visual, ifc->value))
                        fc = ifc->visual;
                }
                ral_symbol_t *symbol = isymbol->deflt;
                if (isymbol->visual) {
                    get_cell_value(self, c, ifc->value, ifc->attr);
                    if (ral_classify(isymbol->rule, isymbol->visual, isymbol->value))
                        symbol = isymbol->visual;
                }
                ral_integer_t *symbol_size = isymbol_size->deflt;
                if (isymbol_size->visual) {
                    get_cell_value(self, c, ifc->value, ifc->attr);
                    if (ral_classify(isymbol_size->rule, isymbol_size->visual, isymbol_size->value))
                        symbol_size = isymbol_size->visual;
                }

                if (symbol->wkt == ral_symbol_square) {
                    int i2,j2;
                    for (i2 = MAX(0, pixel.y-symbol_size->value+1); i2 < MIN(pb->height, pixel.y+symbol_size->value); i2++)
                        for (j2 = MAX(0, pixel.x-symbol_size->value+1); j2 < MIN(pb->width, pixel.x+symbol_size->value); j2++)
                            PBset(pb, i2, j2, fc);
                }
                else if (symbol->wkt == ral_symbol_dot) {
                    FILLED_CIRCLE(pb, pixel, symbol_size->value, fc, PBsetCell);
                } 
                else if (symbol->wkt == ral_symbol_cross) {
                    int i2,j2;
                    for (i2 = MAX(0, pixel.y-symbol_size->value+1); i2 < MIN(pb->height, pixel.y+symbol_size->value); i2++)
                        PBset(pb, i2, pixel.x, fc);
                    for (j2 = MAX(0, pixel.x-symbol_size->value+1); j2 < MIN(pb->width, pixel.x+symbol_size->value); j2++)
                        PBset(pb, pixel.y, j2, fc);
                }
                

            }
        }
        
    }
}

static const struct Class _Raster = {
    sizeof(struct ral_raster),
    {&_Layer, NULL, NULL, NULL},
    /*  1 */ Raster_new,
    /*  2 */ Raster_get_attributes,
    /*  3 */ Raster_delete,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ Raster_name,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ Raster_render,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ Raster_classify,
    /* 19 */ Raster_get_attr_type,
    /* 20 */ Raster_get_value_range,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralRaster = &_Raster;

ral_raster_t *ral_raster_new_from_grid(ral_grid_t *gd)
{
    ral_raster_t *self = ral_new(ralRaster);
    if (self)
        self->grid = gd;
    return self;
}
