#ifndef RAL_STRING_H
#define RAL_STRING_H

/**\file ral2/string.h
   \brief String class

   A string object stores a string.
*/

extern ral_class ralString;
typedef struct ral_string ral_string_t;

ral_string_t *ral_string_new(const char *str);
ral_string_t *ral_string_new_wrap(const char *str);
ral_string_t *ral_string_set(ral_string_t * self, const char *str);
ral_string_t *ral_string_wrap(ral_string_t * self, const char *str);


#endif
