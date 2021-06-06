/* 
 * WAD parser
 * 
 *
 * Autor: Enchanted Hunter
 */

#include "utils.h"

void* readFile(const char* file, uint32_t* size){
	void *source = NULL;
	FILE *fp = fopen(file, "r");
	if (fp != NULL) {

		if (fseek(fp, 0L, SEEK_END) == 0) {

			uint64_t bufsize = ftell(fp);
			if (bufsize == -1) { 
				fclose(fp);
				return NULL;
			}else{
				*size = bufsize;
			}

			source = (unsigned char *)malloc(sizeof(char) * (bufsize + 1));

			if (fseek(fp, 0L, SEEK_SET) != 0) { 
				fclose(fp);
				return NULL;
			}

			uint32_t newLen = fread(source, sizeof(char), bufsize, fp);
			if ( ferror( fp ) != 0 ) {
				fputs("Error reading file", stderr);
			}
		}
		fclose(fp);
	}
	return source;
}

void writeBytesFile(unsigned char* data, char* file, int counts){

    FILE *f = fopen(file, "wb");
    
    int results = fwrite (data , sizeof(char), counts, f);

    if (results == EOF) {
        fputs("Error write file", stderr);
    }
    fclose(f);
}