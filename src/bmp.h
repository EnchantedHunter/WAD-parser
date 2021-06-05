/* 
 * WAD parser
 * 
 *
 * Autor: Enchanted Hunter
 */

#ifndef _BMP_H_
#define _BMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct tagBITMAPFILEHEADER
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD    biSize;
    LONG     biWidth;
    LONG     biHeight;
    WORD     biPlanes;
    WORD     biBitCount;
    DWORD    biCompress;
    DWORD    biSizeImage;
    LONG     biXPelsPerMeter;
    LONG     biYPelsPerMeter;
    DWORD    biClrUsed;
    DWORD    biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
        BYTE    b;
        BYTE    g;
        BYTE    r;
} RGB_data;

int saveBmp(char *filename, int width, int height, unsigned char *data);

#endif