#ifndef RAL_CLASSIFIER_H
#define RAL_CLASSIFIER_H

/**\file ral2/classifier.h 
   \brief An abstract base class for classifiers (such as hashes and bins).

   A classifier is (defined here as) an object, which can compute the
   value of an object (the input) from another object (the
   output). The value can be both discrete and continuous, ordinal and
   non-ordinal. Classifiers are used for reclassifying (mapping) the
   cell values of a grid, for example converting all cells with
   integer value 1 to value 5, and for visualization, for coloring all
   cells with value 5 with red.
*/

extern ral_class ralClassifier;
typedef struct ral_classifier ral_classifier_t;

ral_class       ral_get_input_type(ral_object_t *classifier);
ral_class       ral_get_output_type(ral_object_t *classifier);
int             ral_classify(ral_object_t *classifier, 
                             ral_object_t *output, 
                             ral_object_t *input);

#endif
