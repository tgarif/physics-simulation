#ifndef UTIL_H_
#define UTIL_H_

#include <stddef.h>

char* readFile(const char* filename);
char* custom_strdup(const char* str);

typedef struct {
    void* array;
    size_t size;
    size_t capacity;
    size_t elementSize;
} DynamicArray;

typedef struct {
    char* key;
    void* value;
    struct KeyValue* next;
} KeyValue;

typedef struct HashMap {
    KeyValue** table;
    size_t size;
    size_t capacity;
} DynamicHashMap;

void initialize(DynamicArray* dynArray, size_t initialCapacity, size_t elementSize);
void push(DynamicArray* dynArray, const void* value);
void insert(DynamicArray* dynArray, size_t position, const void* start, size_t count);
void cleanup(DynamicArray* dynArray);

unsigned int hash(const char* key, size_t capacity);
void rehash(DynamicHashMap* map);
void initHashMap(DynamicHashMap* map, size_t initialCapacity);
void insertIntoHashMap(DynamicHashMap* map, const char* key, void* value);
void* getFromHashMap(DynamicHashMap* map, const char* key);
void freeHashMap(DynamicHashMap* map);

#endif
