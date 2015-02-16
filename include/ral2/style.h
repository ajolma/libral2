#ifndef	STYLE_H
#define	STYLE_H

/**\file ral2/style.h
   \brief Style constants
   
   
*/

/* visual attributes */

extern const char *ral_style_layer_transparency;

ral_array_t *ral_visual_attributes();

/* supported attributes are obtained from a layer as a list */

/* well known symbols */

extern const char *ral_symbol_square;
extern const char *ral_symbol_dot;
extern const char *ral_symbol_cross;
extern const char *ral_symbol_arrow;
extern const char *ral_symbol_flow_direction;
extern const char *ral_symbol_wind_rose;

#endif
