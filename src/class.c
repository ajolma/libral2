#include "ral2/ral.h"
#include "priv/priv.h"

/*! \brief The name of a class.
 *
 *  \returns Returns the name of the class or "Unnamed class".
 */
const char *ral_class_name(ral_class class)
{
    if (!class) return "null";
    const struct Class *c = class;
    if (c->name)
        return c->name();
    else
        return "Unnamed class";
}

int ral_class_is_a(ral_class class, ral_class other)
{
    if (class == other)
        return 1;
    else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++) {
            if (class->super[i] && ral_class_is_a(class->super[i], other))
                return 1;
        }
        return 0;
    }
    else
        return 0;
}

void call_delete(struct ral_object *object, const struct Class *class);

ral_object_t *call_new(struct ral_object *object, const struct Class *class)
{
    if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i] && !call_new(object, class->super[i])) {
                int j;
                for (j = 0; j < i; j++)
                    call_delete(object, class->super[j]);
                return NULL;
            }
    } 
    if (class->new)
        return class->new(object);
    return object;
}

/*! \brief Create a new object into a class.
 *
 * An object owns or wraps its contents. A wrapper maintains only a
 * link to the contents.
 *
 *  \returns Returns the new object or NULL if it could not be
 *  created. Creation may fail mainly because of lack of memory.
 */
ral_object_t *ral_new(ral_class class)
{
    if (!class) return NULL;
    struct ral_object *object = calloc(1, class->size);
    if (!object) return NULL;
    /*fprintf(stderr, "new %s %i %p\n", ral_name_of_class(c), c->size, p);*/
    object->class = class;
    object->wrap = 0;
    object->string = NULL;
    if (!call_new(object, class)) {
        free(object);
        return NULL;
    }
    return object;
}

/*! \brief The class of an object.
 *
 *  \returns Returns the class of the object.
 */
ral_class ral_class_of(ral_object_t *object)
{
    if (!object) return NULL;
    return class_of(object);
}

/*! \brief Test for class.
 *
 *  \returns Returns 1 if the object belongs to a class, 0
 *  otherwise. The class may be superclass of the actual class of the
 *  object.
 */
int ral_is_a(ral_object_t *object, ral_class class)
{
    struct ral_object *obj = object;
    return ral_class_is_a(class_of(obj), class);
}

void call_delete(struct ral_object *object, const struct Class *class)
{
    if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i]) call_delete(object, class->super[i]);
    }
    if (class->delete) {
        class->delete(object);
    }
}

/*! \brief Delete an object.
 *
 *  Frees the memory and other resources allocated for the object.
 */
void ral_delete(ral_object_t *object)
{
    struct ral_object *obj = object;
    if (!obj || obj == ral_undef) return;
    /*fprintf(stderr, "del %p\n", object);*/
    call_delete(obj, obj->class);
    if (obj->string) free(obj->string);
    free(obj);
}

int call_set(struct ral_object *object, const struct Class *class, ral_object_t *other)
{
    if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i] && !call_set(object, class->super[i], other))
                return 0;
    }
    if (class->set)
        return class->set(object, other);
    return 1;
}

/*! \brief Copy the contents of an object into this object.
 *
 *  The objects should typically have the same class.
 *
 * \returns Returns 1 if the copy was succesful, 0 otherwise. The copy
 * may fail mainly because of lack of memory.
 */
int ral_set(ral_object_t *object, ral_object_t *other)
{
    struct ral_object *a = object;
    struct ral_object *b = other;
    if (!a || a == ral_undef || !a->class || !b) return 0;
    if (class_of(a) != class_of(b)) return 0;
    return call_set(a, a->class, other);
}

/*! \brief Create an exact copy of an object.
 *
 * \returns Returns the new object.
 */
ral_object_t *ral_clone(ral_object_t *object)
{
    struct ral_object *obj = object;
    if (!obj) return NULL;
    if (obj == ral_undef) return ral_undef;
    struct ral_object *clone = ral_new(obj->class);
    if (clone && !ral_set(clone, object)) {
        ral_delete(clone);
        return NULL;
    }
    return clone;
}

long call_as_int(struct ral_object *object, const struct Class *class)
{
    if (class->as_int) {
        return class->as_int(object);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->as_int(object);
    }
    fprintf(stderr, "Missing as_int method.\n");
    return 0;
}

/*! \brief Integer value of the object.
 *
 * For integers returns the value, for reals returns the value
 * rounded, for strings returns the converted value.
 *
 * \returns Returns the integer value.
 *
 * \todo Dates etc.
 */
long ral_as_int(ral_object_t *object)
{
    struct ral_object *obj = object;
    if (!obj || obj == ral_undef) return 0;
    return call_as_int(obj, obj->class);
}

double call_as_real(struct ral_object *object, const struct Class *class)
{
    if (class->as_real) {
        return class->as_real(object);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->as_real(object);
    }
    fprintf(stderr, "Missing as_real method.\n");
    return 0.0;
}

/*! \brief Real value of the object.
 *
 * For integers and reals returns the value, for strings returns the
 * converted value.
 *
 * \returns Returns the real value.
 *
 * \todo Dates etc.
 */
double ral_as_real(ral_object_t *object)
{
    struct ral_object *obj = object;
    if (!obj || obj == ral_undef) return 0;
    return call_as_real(obj, obj->class);
}

const char *call_as_string(struct ral_object *object, const struct Class *class)
{
    if (class->to_string) {
        class->to_string(object);
        return object->string;
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i]) {
                class->super[i]->to_string(object);
                return object->string;
            }
    }
    fprintf(stderr, "Missing as_string method.\n");
    return "";
}

/*! \brief The object stringified.
 *
 * Returns a string representation of the object.
 *
 * \returns Returns the string.
 *
 * \todo Dates etc.
 */
const char *ral_as_string(ral_object_t *object)
{
    struct ral_object *obj = object;
    if (!obj) 
        return "null";
    else if (obj == ral_undef)
        return "undef";
    return call_as_string(obj, obj->class);
}
