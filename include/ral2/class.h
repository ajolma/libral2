#ifndef	RAL_CLASS_H
#define	RAL_CLASS_H

/**\file class.h
   \brief The root class (Object)

   These methods are defined for all classes but depending on the
   class it may support it or not.

   Classes are pointers to const structs.

   Objects are pointers to structs (ral_object or structs that extend
   it), although the basic object type is void. Void is used to not
   introduce a great need for casting in the case of polymorphic
   methods.

   There is no garbage collection. As long as possible objects should
   be created and deleted in the same scope. Exceptions are container
   objects, which take ownership of objects, and constructors, which
   give away the ownership. Wrapper objects are objects which do not
   own their contents and thus do not delete them when they are
   deleted.
*/

/**\brief Type for classes. */
typedef const struct Class * ral_class;

extern ral_class ralObject;
extern ral_class ralDate;
extern ral_class ralTime;
extern ral_class ralDateTime;
extern ral_class ralGrayscale;
extern ral_class ralHueScale;
extern ral_class ralColorComponentScale;
extern ral_class ralStorage;
extern ral_class ralOutputDevice;
extern ral_class ralIntegerRaster;
extern ral_class ralRealRaster;

const char *ral_class_name(ral_class class);
int         ral_class_is_a(ral_class class, ral_class other);

/**\brief Root class for objects. */
typedef void ral_object_t;
/**\brief Class for dates (the day of month, month, and the year plus
 * calendar system). */
typedef struct ral_date ral_date_t;
/**\brief Class for time (hours, minutes, and seconds after
 * midnight). */
typedef struct ral_time ral_time_t;
/**\brief Class for date and time plus time zone. */
typedef struct ral_date_time ral_date_time_t;
/**\brief Class for computing grayscale values from ordinal values. */
typedef struct ral_grayscale ral_grayscale_t;
/**\brief Class for computing hues from ordinal values. */
typedef struct ral_hue_scale ral_hue_scale_t;
/**\brief Class for computing color component values from ordinal values. */
typedef struct ral_color_component_scale ral_color_component_scale_t;
/**\brief Abstract base class for storages. */
typedef struct ral_storage ral_storage_t;
/**\brief Abstract base class for all cairo output devices. */
typedef struct ral_output_device ral_output_device_t;

/**\brief A special object for undefined values. 
 *
 * An undefined object represents an unknown but existing thing. Undef
 * is not NULL. NULL means "no object".
 */
extern ral_object_t *ral_undef;

ral_object_t   *ral_new(ral_class class);
ral_class       ral_class_of(ral_object_t *object);
int             ral_is_a(ral_object_t *object, ral_class class);
void            ral_delete(ral_object_t *object);
int             ral_set(ral_object_t *object, ral_object_t *other);
ral_object_t   *ral_clone(ral_object_t *object);
long            ral_as_int(ral_object_t *object);
double          ral_as_real(ral_object_t *object);
const char     *ral_as_string(ral_object_t *object);

#endif
