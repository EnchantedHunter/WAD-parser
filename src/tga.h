/* 
 * WAD parser
 * 
 *
 * Autor: Enchanted Hunter
 */

#ifndef _TGA_H_
#define _TGA_H_

#include <math.h>

typedef unsigned char byte;

int compress(const void * image, int width, int height, void * output, void* palette, unsigned char change_palette);
int mip_n_n(const void * image, int width, int height,  void * output);

#endif