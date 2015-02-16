#include "ral2/ral.h"
#include "priv/priv.h"

int call_insert(struct ral_object *storage, const struct Class *class, ral_object_t *object, ral_object_t *key)
{
    if (class->insert) {
        return class->insert(storage, object, key);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->insert(storage, object, key);
    }
    fprintf(stderr, "Missing insert method.\n");
    return 0;
}

/*! \brief Insert an object into this object using the key.
 *
  * \returns Returns the success. Insertion may fail typically because
  * of lack of memory.
 */
int ral_insert(ral_object_t *storage, ral_object_t *object, ral_object_t *key)
{
    struct ral_object *obj = storage;
    return call_insert(obj, obj->class, object, key);
}

int call_remove(struct ral_object *storage, const struct Class *class, ral_object_t *key)
{
    if (class->remove) {
        return class->remove(storage, key);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->remove(storage, key);
    }
    fprintf(stderr, "Missing remove method.\n");
    return 0;
}

/*! \brief Remove an object from this object using the key.
 *
 * \returns Returns 1 if there was an object identified by the key.
 */
int ral_remove(ral_object_t *storage, ral_object_t *key)
{
    struct ral_object *obj = storage;
    return call_remove(obj, obj->class, key);
}

ral_array_t *call_keys(struct ral_object *storage, const struct Class *class)
{
    if (class->keys) {
        return class->keys(storage);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->keys(storage);
    }
    fprintf(stderr, "Missing keys method.\n");
    return NULL;
}

/*! \brief The keys for objects within this object.
 *
 * \returns Returns the keys in an array.
 */
ral_array_t *ral_keys(ral_object_t *storage)
{
    struct ral_object *obj = storage;
    return call_keys(obj, obj->class);
}

int call_number_of_elements(struct ral_object *storage, const struct Class *class)
{
    if (class->number_of_elements) {
        return class->number_of_elements(storage);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->number_of_elements(storage);
    }
    fprintf(stderr, "Missing number_of_elements method.\n");
    return 0;
}

/*! \brief The number of elemental objects within the object.
 *
 * \returns Returns the number of elemental objects within this storage.
 */
int ral_number_of_elements(ral_object_t *storage)
{
    struct ral_object *obj = storage;
    if (!obj || obj == ral_undef) return 0;
    return call_number_of_elements(obj, obj->class);
}

ral_object_t *call_lookup(struct ral_object *storage, const struct Class *class, ral_object_t *key)
{
    if (class->lookup) {
        return class->lookup(storage, key);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->lookup(storage, key);
    }
    fprintf(stderr, "Missing lookup method.\n");
    return NULL;
}

/*! \brief Looks up an object within this object based on a key.
 *
 * \returns Returns a pointer to the object or NULL if the object is
 * not found. The object should not be deleted.
 */
ral_object_t *ral_lookup(ral_object_t *storage, ral_object_t *key)
{
    struct ral_object *obj = storage;
    if (!obj) return NULL;
    if (obj == ral_undef) return ral_undef;
    return call_lookup(obj, obj->class, key);
}

ral_interval_t *call_get_value_range(struct ral_object *storage, const struct Class *class, const char *attr)
{
    if (class->get_value_range) {
        return class->get_value_range(storage, attr);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->get_value_range(storage, attr);
    }
    fprintf(stderr, "Missing get_value_range method.\n");
    return NULL;
}

/*! \brief The value range of an attribute.
 *
 * The objects in the storage may contain data associated with
 * attributes.
 *
 * \returns The interval (value range) of the values of the specified
 * attribute or NULL in the case of an error.
 */
ral_interval_t *ral_get_value_range(ral_object_t *layer, const char *attr)
{
    struct ral_object *obj = layer;
    return call_get_value_range(obj, obj->class, attr);
}
