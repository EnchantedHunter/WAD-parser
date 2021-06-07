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
#include <unistd.h>
#include <dirent.h>

#include "wad.h"
#include "utils.h"
#include "bmp.h"
#include "tga.h"

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
    usleep(59000u);
}

extern const byte defaultPalette[768];

int main(int argc, char** argv){

    if(argc != 2){
        printf("Введите путь до папки с bmp текстурами!\n");
        return 1;
    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (argv[1])) != NULL) {

    LINKEDLIST * info = (LINKEDLIST*)malloc(sizeof(LINKEDLIST));
    info->next = NULL;

    uint32_t nDir = 0;

    while ((ent = readdir (dir)) != NULL) {
        if(strstr(ent->d_name, ".bmp")){

            printf ("%s\n", ent->d_name);
            
            char path_name_ext[sizeof(argv[1]) + strlen(ent->d_name) + 2 ];  
            sprintf(path_name_ext, "%s/%s", argv[1], ent->d_name);

            uint32_t bmp_len;
            void * bmp = readFile(path_name_ext, &bmp_len);

            BITMAPFILEHEADER * file_header = (BITMAPFILEHEADER*)bmp;
            BITMAPINFOHEADER * info_header = (BITMAPINFOHEADER*)(bmp + sizeof(BITMAPFILEHEADER));

            uint32_t w, h;
            w = info_header->biWidth;
            h = info_header->biHeight;

            if(w % 8 != 0 || h % 8 != 0){
                printf("Кроме текстуры %s, её размеры не кратны 8ми.\n", ent->d_name);
                free(bmp);
                continue;
            }

            if( w > 2048 || h > 2048){
                printf("Размеры изображения %s больше 2048.\n", ent->d_name);
                free(bmp);
                continue;
            }

            void * payload = bmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
            void * payload_inv = malloc(w*h*3);

            for(int jj = 0 ; jj < h; jj++ ){
                for(int ii = 0 ; ii < w*3; ii++){
                    ((unsigned char*)payload_inv)[ii + jj*w*3] = ((unsigned char*)payload)[ii + (h - jj - 1)*w*3];
                }
            }

            void * palette = malloc(sizeof(defaultPalette));
            memcpy(palette, defaultPalette, sizeof(defaultPalette));

            void * compressed_mip_0 = malloc(w * h);
            void * compressed_mip_1 = malloc(w * h / 4);
            void * compressed_mip_2 = malloc(w * h / 16);
            void * compressed_mip_3 = malloc(w * h / 64);
            
            compress(payload_inv, w, h, compressed_mip_0, palette, 0);

            mip_n_n(compressed_mip_0, w, h, compressed_mip_1);
            mip_n_n(compressed_mip_1, w/2, h/2, compressed_mip_2);
            mip_n_n(compressed_mip_2, w/4, h/4, compressed_mip_3);

            void * palette_r = malloc(sizeof(defaultPalette));
            
            for(int m = 0 ; m < 256 ; m++){
                *((unsigned char*)palette_r + m*3 + 0) = *((unsigned char*)palette + m*3 + 2); 
                *((unsigned char*)palette_r + m*3 + 1) = *((unsigned char*)palette + m*3 + 1); 
                *((unsigned char*)palette_r + m*3 + 2) = *((unsigned char*)palette + m*3 + 0); 
            }
            free(palette);
            //info

            WADDIRENTRY* direntry = (WADDIRENTRY*)malloc(sizeof(WADDIRENTRY));
            strncpy(direntry->szName, ent->d_name, 12);
            direntry->szName[11] = '\0';
            char * ent = strstr(direntry->szName, ".");
            if(ent!= NULL){
                *ent = '\0';
            }
            direntry->bCompression = (unsigned char)0;
            direntry->nSize = w*h + w*h/4 + w*h/16 + w*h/64 + 2 + 768 + 2 ; 
            direntry->nDiskSize = w*h + w*h/4 + w*h/16 + w*h/64 + 2 + 768 + 2 ;
            direntry->nType = 'C';

            void * next = info;    
            while (1)
            {
                if(((LINKEDLIST*)next)->next != NULL){
                    next = ((LINKEDLIST*)next)->next; 
                }else break;
            }

            LINKEDLIST * im_info = (LINKEDLIST*)malloc(sizeof(LINKEDLIST));
            im_info->next = NULL;
            im_info->data = (void*)direntry;
            im_info->mip_0 = compressed_mip_0;
            im_info->mip_1 = compressed_mip_1;
            im_info->mip_2 = compressed_mip_2;
            im_info->mip_3 = compressed_mip_3;
            im_info->palette = palette_r;
            im_info->info_header = (void*) info_header;

            ((LINKEDLIST*)next)->next = (void*)im_info;
            nDir++;
        }
    }
    closedir (dir);
    
    WADHEADER wheader;
    strncpy(wheader.szMagic, "WAD3", 4);
    wheader.nDir = nDir;
    wheader.nDirOffset = sizeof(WADHEADER);
    
    FILE *f = fopen("output.wad", "wb");
    
    fwrite (((void*)&wheader) , 1, sizeof(WADHEADER), f);

    void* next = info->next;
    int i = 0;
    int lll = 0;

    while(1){
        ((WADDIRENTRY*)(((LINKEDLIST*)next)->data))->nFilePos =
         sizeof(WADHEADER) + nDir*sizeof(WADDIRENTRY) + lll;
        void * save_direntry = (void*) ((LINKEDLIST*)next)->data;
        fwrite ( save_direntry , 1, sizeof(WADDIRENTRY), f);
        i++;

        uint32_t www = ((BITMAPINFOHEADER*)((LINKEDLIST*)next)->info_header)->biWidth;
        uint32_t hhh = ((BITMAPINFOHEADER*)((LINKEDLIST*)next)->info_header)->biHeight;
        lll += sizeof(BSPMIPTEXWAD) + www*hhh + www*hhh/4 + www*hhh/16 + www*hhh/64 + 2 + 768 + 2;

        if(((LINKEDLIST*)next)->next != NULL){
            next = ((LINKEDLIST*)next)->next; 
        }else break;
    }

    next = info->next;
    i = 0;
    uint32_t ll = 0;

    while(1){

        WADDIRENTRY * direntry = (WADDIRENTRY*) (((LINKEDLIST*)next)->data);
        BITMAPINFOHEADER * info_header = (BITMAPINFOHEADER*) (((LINKEDLIST*)next)->info_header);
        LINKEDLIST* list = ((LINKEDLIST*)next);

        uint32_t w, h;
        w = info_header->biWidth;
        h = info_header->biHeight;

        BSPMIPTEXWAD texwad;
        strncpy(texwad.szName, direntry->szName, 12);
        texwad.szName[11] = '\0';
        texwad.nWidth = w;
        texwad.nHeight = h;
        texwad.nOffsets[0] = sizeof(BSPMIPTEXWAD) ;
        texwad.nOffsets[1] = sizeof(BSPMIPTEXWAD) + w*h ;
        texwad.nOffsets[2] = sizeof(BSPMIPTEXWAD) + w*h + w*h/4;
        texwad.nOffsets[3] = sizeof(BSPMIPTEXWAD) + w*h + w*h/4 + w*h/16;

        fwrite ( (void*)&texwad , 1, sizeof(BSPMIPTEXWAD), f);

        uint16_t pal_size = (uint16_t)256;
        uint16_t padding = (uint16_t)0;

        fwrite ( list->mip_0 , 1, w*h, f);
        fwrite ( list->mip_1 , 1, w*h/4, f);
        fwrite ( list->mip_2 , 1, w*h/16, f);
        fwrite ( list->mip_3 , 1, w*h/64, f);
        fwrite ( &pal_size , 2, 1, f);
        fwrite ( list->palette , 1, 768, f);
        fwrite ( &padding , 2, 1, f);

        ll += w*h + w*h/4 + w*h/16 + w*h/64 + 2 + 256*3 + 2;
        i++;

        if(((LINKEDLIST*)next)->next != NULL){
            next = ((LINKEDLIST*)next)->next; 
        }else break;
    }

    fclose(f);

    } else {
        /* could not open directory */
        printf("Нет возможности открыть директорию.");
        return EXIT_FAILURE;
    }

    return 0;
}