#ifndef UTIL_H_
#define UTIL_H_

#include <stddef.h>

char* readFile(const char* filename);

typedef struct {
    void* array;
    size_t size;
    size_t capacity;
    size_t elementSize;
} DynamicArray;

void initialize(DynamicArray* dynArray, size_t initialCapacity, size_t elementSize);
void push(DynamicArray* dynArray, const void* value);
void insert(DynamicArray* dynArray, size_t position, const void* start, size_t count);
void cleanup(DynamicArray* dynArray);

#endif
