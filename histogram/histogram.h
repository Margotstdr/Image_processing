#ifndef histogram_H
#define histogram_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bmp8/bmp8.h"
#include "../bmp24/bmp24.h"

// Function declarations
unsigned int * bmp8_computeHistogram(t_bmp8 *img);
unsigned int * bmp8_computeCDF(unsigned int *hist);
void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq);
void bmp24_equalize(t_bmp24 *img);

#endif // histogram_H
