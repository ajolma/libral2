#include "ral2/ral.h"
#include "priv/priv.h"

/** visual attributes */

const char *ral_style_layer_transparency = "layer transparency";

const char * const visual_attribute[] = {
    ral_style_feature_transparency, (char *)&_Integer,
    ral_style_fill_color, (char *)&_Color,
    ral_style_border_color, (char *)&_Color,
    ral_style_line_width, (char *)&_Real,
    ral_style_symbol, (char *)&_Symbol,
    ral_style_symbol_size, (char *)&_Integer,
    ""
};

ral_array_t *ral_visual_attributes()
{
    ral_array_t *a = ral_new(ralArray);
    return a;
}

const char *ral_attribute_fid = "fid";
const char *ral_attribute_z_value = "z value";
const char *ral_attribute_m_value = "M value";
const char *ral_attribute_cell_value = "cell value";

const char *ral_symbol_square = "square";
const char *ral_symbol_dot = "dot";
const char *ral_symbol_cross = "cross";
const char *ral_symbol_arrow = "arrow";
const char *ral_symbol_flow_direction = "flow direction";
const char *ral_symbol_wind_rose = "wind rose";

char *ral_stock_string(const char *str)
{
    if (strcmp(str, ral_attribute_cell_value) == 0)
        return (char *)ral_attribute_cell_value;
    else
        return (char *)str;
}

void ral_delete_non_stock_string(char *str)
{
    if (strcmp(str, ral_attribute_cell_value) == 0)
        ;
    else
        free(str);
}
