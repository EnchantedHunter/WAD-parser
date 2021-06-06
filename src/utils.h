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

typedef struct _LINKEDLIST
{
    void * next;
    void * data;
    void * mip_0;
    void * mip_1;
    void * mip_2;
    void * mip_3;
    void * palette;
    void * info_header;
} LINKEDLIST;

void* readFile(const char* file, uint32_t* size);
void writeBytesFile(unsigned char* data, char* file, int counts);

#endif