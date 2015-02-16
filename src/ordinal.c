#include "ral2/ral.h"
#include "priv/priv.h"

/*! \brief Compare two ordinal objects.
 *
 * The objects typically have to have the same classes.
 *
 * \returns Returns -1, 0, or 1 depending how the objects compare.
 */
int ral_cmp(ral_object_t *object, ral_object_t *other)
{
    struct ral_object *a = object;
    struct ral_object *b = other;
    /*fprintf(stderr, "cmp %s to %s\n", as_string(a), as_string(b));*/
    if (!a || a == ral_undef || !b || b == ral_undef) return 0;
    if (class_of(a) == class_of(b) && a->class->cmp)
        return a->class->cmp(a, b);
    else if (class_of(a) == ralString)
        return strcmp(((ral_string_t *)a)->text, ral_as_string(b));
    else if (class_of(b) == ralString)
        return strcmp(ral_as_string(a), ((ral_string_t *)b)->text);
    else if (a->class->as_real && b->class->as_real) {
        double ca = a->class->as_real(a);
        double cb = b->class->as_real(b);            
        if (ca > cb) return 1;
        if (ca < cb) return -1;
        return 0;
    }
    const char *ac = NULL;
    if (a && a->class) ac = ral_class_name(a->class);
    const char *bc = NULL;
    if (b && b->class) bc = ral_class_name(b->class);
    fprintf(stderr, "WARNING: missing cmp method or comparing nulls or non-objects (%s,%s).\n", ac, bc);
    return 0;
}

