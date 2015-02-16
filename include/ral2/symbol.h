#ifndef RAL_SYMBOL_H
#define RAL_SYMBOL_H

/**\file ral2/symbol.h
   \brief Symbol class

   A symbol object is used for visual symbols.
*/

extern ral_class ralSymbol;
typedef struct ral_symbol ral_symbol_t;

void ral_symbol_set_built_in(ral_symbol_t *self, const char *symbol);

#endif
