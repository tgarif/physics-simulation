#include "file.h"

#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* filename) {
    FILE* fp;
    long size = 0;
    char* content;

    /* Read File to get size */
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp) + 1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(filename, "r");
    content = malloc(size);
    fread(content, 1, size - 1, fp);
    content[size - 1] = '\0';
    fclose(fp);

    return content;
}
