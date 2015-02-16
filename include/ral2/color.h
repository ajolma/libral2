#ifndef RAL_COLOR_H
#define RAL_COLOR_H

/**\file ral2/color.h
   \brief Color class.

   A color is an object, which stores a visual color.
*/

extern ral_class ralColor;
typedef struct ral_color ral_color_t;

ral_color_t *ral_color_new(int red, int green, int blue, int alpha);
void ral_color_set(ral_color_t *self, int red, int green, int blue, int alpha);

#endif
