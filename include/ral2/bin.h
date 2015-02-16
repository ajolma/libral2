#ifndef RAL_BIN_H
#define RAL_BIN_H

/**\file ral2/bin.h
   \brief Bin class.

   A bin is a <a href="storage_8h.html">storage</a>, which divides a
   dimension into continuous regions. A bin is also a <a
   href="classifier_8h.html">classifier</a>, as it maps numeric values into
   arbitrary objects.
*/

extern ral_class ralBin;
typedef struct ral_bin ral_bin_t;

void ral_bin_set(ral_bin_t * self, ral_array_t *bins, ral_array_t *values);
void ral_bin_set_bins(ral_bin_t * bin, ral_array_t *bins);
ral_object_t *ral_bin_bin(ral_bin_t * bin, int i);
ral_object_t *ral_bin_value(ral_bin_t * bin, int i);


#endif
