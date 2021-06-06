/* 
 * WAD parser
 * 
 *
 * Autor: Enchanted Hunter
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wad.h"
#include "utils.h"
#include "bmp.h"

#define BAR_PAYLOAD '#'

void P_Bar_Step(int procent){
    int i = procent;
    int j = 0;

    printf("[");
    for(j = 0 ; j < i; j++){
        printf("%c", BAR_PAYLOAD);
    }
    for(j = 0 ; j < 20 - i; j++){
        printf("%s", " ");
    }
    printf("]%d",i*5);
    printf("\r");
    fflush(stdout);
}

int main(int argc, char** argv){

    if(argc != 2){
        printf("Введите путь до WAD файла!\n");
        return 1;
    }

    uint32_t wad_len;
    void* wad = readFile(*(argv + 1), &wad_len);

    WADHEADER* header = (WADHEADER*)wad;

    if( *(header->szMagic + 3) == '2' || *(header->szMagic + 3) == '3' ){

        int i;
        for(i = 0 ; i < header->nDir; i++){
            WADDIRENTRY* dir = (WADDIRENTRY*)(wad + header->nDirOffset + sizeof(WADDIRENTRY)*i);
            BSPMIPTEXWAD* tex = (BSPMIPTEXWAD*)(wad + dir->nFilePos);

            void* image = malloc(tex->nWidth * tex->nHeight * 3 * 2);

            unsigned char indices[tex->nWidth * tex->nHeight];
            memcpy(indices, wad + dir->nFilePos + *(tex->nOffsets + 0), tex->nWidth * tex->nHeight);

            unsigned char palette[256*3];
            memcpy(palette, wad + dir->nFilePos + *(tex->nOffsets + 3) + ((tex->nWidth/8) * (tex->nHeight/8)) + 2, 256*3);

            for(uint32_t i = 0 ; i < tex->nWidth * tex->nHeight ; i++){
                unsigned char* paletteIdx = palette + ((unsigned char)(indices[i])*3);
                memcpy(image + i*3, paletteIdx, 3);   
                *((unsigned char*)image + i*3 + 0) = *((unsigned char*)paletteIdx + 2);     
                *((unsigned char*)image + i*3 + 1) = *((unsigned char*)paletteIdx + 1);              
                *((unsigned char*)image + i*3 + 2) = *((unsigned char*)paletteIdx + 0);              
            }

            for(int j = 0 ; j < tex->nHeight; j++ ){
                for(int i = 0 ; i < tex->nWidth*3; i++){
                    *((unsigned char*)image + tex->nWidth * tex->nHeight * 3 + i + j*tex->nWidth*3) = ((unsigned char*)image)[i + (tex->nHeight - j - 1)*tex->nWidth*3];//[(bspTex->nWidth*3 - i ) + j*bspTex->nWidth*3]; //
                }
            }

            char saveName[MAXTEXTURENAME + 4];
            sprintf(saveName, "%s.bmp", dir->szName);

            saveBmp(saveName, tex->nWidth, tex->nHeight, image + tex->nWidth * tex->nHeight * 3);
            P_Bar_Step(  (int)((20.0 * i) / header->nDir) );
            free(image);
        }

    }else{
        printf("Не корректный WAD файл!\n");
        return 1;
    }

    fflush(stdout);
    free(wad);

    return 0;
}