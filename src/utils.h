/* 
 * WAD parser
 * 
 *
 * Autor: Enchanted Hunter
 */
 
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void* readFile(const char* file, uint32_t* size);
void writeBytesFile(unsigned char* data, char* file, int counts);

#endif