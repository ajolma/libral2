#include "ral2/ral.h"
#include "priv/priv.h"

const struct Class _Ordinal = {
    sizeof(struct ral_ordinal),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralOrdinal = &_Ordinal;

const struct Class _Classifier = {
    sizeof(struct ral_classifier),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralClassifier = &_Classifier;

const struct Class _Storage = {
    sizeof(struct ral_storage),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralStorage = &_Storage;

const struct Class _OutputDevice = {
    sizeof(struct ral_output_device),
    {&_Object, NULL, NULL, NULL},
    /*  1 */ NULL,
    /*  2 */ NULL,
    /*  3 */ NULL,
    /*  4 */ NULL,
    /*  5 */ NULL,
    /*  6 */ NULL,
    /*  7 */ NULL,
    /*  8 */ NULL,
    /*  9 */ NULL,
    /* 10 */ NULL,
    /* 11 */ NULL,
    /* 12 */ NULL,
    /* 13 */ NULL,
    /* 14 */ NULL,
    /* 15 */ NULL,
    /* 16 */ NULL,
    /* 17 */ NULL,
    /* 18 */ NULL,
    /* 19 */ NULL,
    /* 20 */ NULL,
    /* 21 */ NULL,
    /* 22 */ NULL,
    /* 23 */ NULL
};

ral_class ralOutputDevice = &_OutputDevice;

/*! \brief The class of the objects that this classifier accepts as input.
 *
 * \returns Returns the class.
 */
ral_class ral_get_input_type(ral_object_t *classifier)
{
    struct ral_object *obj = classifier;
    if (obj) {
        if (obj->class->get_input_type)
            return obj->class->get_input_type(classifier);
        else if (obj->class->super && obj->class->super[0]->get_input_type)
            return obj->class->super[0]->get_input_type(classifier);
        else
            fprintf(stderr, "assert: missing get_input_type method (class = %s)\n", ral_class_name(obj->class));
    }
    return NULL;
}

/*! \brief The class of the objects that this classifier outputs.
 *
 * \returns Returns the class.
 */
ral_class ral_get_output_type(ral_object_t *classifier)
{
    struct ral_object *obj = classifier;
    if (obj) {
        if (obj->class->get_output_type)
            return obj->class->get_output_type(classifier);
        else if (obj->class->super && obj->class->super[0]->get_output_type)
            return obj->class->super[0]->get_output_type(classifier);
        else
            fprintf(stderr, "assert: missing get_output_type method (class = %s)\n", ral_class_name(obj->class));
    }
    return NULL;
}

int call_classify(struct ral_object *classifier, const struct Class *class, ral_object_t *output, ral_object_t *input)
{
    if (class->classify) {
        return class->classify(classifier, output, input);
    } else if (class->super) {
        int i;
        for (i = 0; i < MAX_SUPER_CLASSES; i++)
            if (class->super[i])
                return class->super[i]->classify(classifier, output, input);
    }
    fprintf(stderr, "Missing classify method.\n");
    return 0;
}

/*! \brief Sets the value of the output object based on the input object.
 *
 * \returns Returns true if the classification was successful.
 */
int ral_classify(ral_object_t *classifier, ral_object_t *output, ral_object_t *input)
{
    struct ral_object *rule = classifier;
    return call_classify(rule, rule->class, output, input);
}
