#ifndef RAL_ERROR_H
#define RAL_ERROR_H

/**\file ral2/error.h
   \brief Error class.

   A error object is used for reporting errors and warnings.
*/

extern ral_class ralError;

typedef struct ral_error ral_error_t;

ral_error_t *ral_error_new(int level, char *format, ...);

int ral_error_level(ral_error_t *e);

#endif
