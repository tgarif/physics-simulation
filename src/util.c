#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char* custom_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy) strcpy(copy, str);
    return copy;
}

void initialize(DynamicArray* dynArray, size_t initialCapacity, size_t elementSize) {
    dynArray->array = malloc(initialCapacity * elementSize);
    dynArray->size = 0;
    dynArray->capacity = initialCapacity;
    dynArray->elementSize = elementSize;
}

void push(DynamicArray* dynArray, const void* value) {
    if (dynArray->size == dynArray->capacity) {
        dynArray->capacity *= 2;
        dynArray->array = realloc(dynArray->array, dynArray->capacity * dynArray->elementSize);
    }
    memcpy((char*)dynArray->array + dynArray->size * dynArray->elementSize, value, dynArray->elementSize);
    dynArray->size++;
}

void insert(DynamicArray* dynArray, size_t position, const void* start, size_t count) {
    if (position > dynArray->size) {
        fprintf(stderr, "Error: Insert position out of bounds.\n");
        return;
    }

    while (dynArray->size + count > dynArray->capacity) {
        dynArray->capacity *= 2;
        dynArray->array = realloc(dynArray->array, dynArray->capacity * dynArray->elementSize);
    }

    // Shift elements to the right to make space for the new range
    char* base = (char*)dynArray->array;
    memmove(base + (position + count) * dynArray->elementSize,
        base + position * dynArray->elementSize,
        (dynArray->size - position) * dynArray->elementSize);

    memcpy(base + position * dynArray->elementSize, start, count * dynArray->elementSize);

    dynArray->size += count;
}

void cleanup(DynamicArray* dynArray) {
    free(dynArray->array);
    dynArray->array = NULL;
    dynArray->size = dynArray->capacity = dynArray->elementSize = 0;
}

unsigned int hash(const char* key, size_t capacity) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % capacity;
}

void rehash(DynamicHashMap* map) {
    size_t oldCapacity = map->capacity;
    KeyValue** oldTable = map->table;

    // Double the capacity
    map->capacityj *= 2;
    map->table = malloc(map->capacity * sizeof(KeyValue*));
    for (size_t i = 0; i < map->capacity; i++) {
        map->table[i] = NULL;
    }

    // Re-insert all key-value pairs into the new table
    for (size_t i = 0; i < oldCapacity; i++) {
        KeyValue* current = oldTable[i];
        while (current) {
            KeyValue* next = current->next;
            insert(map, current->key, current->value);
            free(current->key);
            free(current);
            current = next;
        }
    }

    free(oldTable);
}

void initHashMap(DynamicHashMap* map, size_t initialCapacity) {
    map->capacity = initialCapacity;
    map->size = 0;
    map->table = malloc(initialCapacity * sizeof(KeyValue*));
    for (size_t i = 0; i < initialCapacity; i++) {
        map->table[i] = NULL;
    }
}

void insertIntoHashMap(DynamicHashMap* map, const char* key, void* value) {
    unsigned int index = hash(key, map->capacity);

    KeyValue* current = map->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Insert a new key-value pair
    KeyValue* newNode = malloc(sizeof(KeyValue));
    newNode->key = custom_strdup(key);
    newNode->value = value;
    newNode->next = map->table[index];
    map->table[index] = newNode;
    map->size++;

    // Resize if load factor exceeds 0.75
    if ((float)map->size / map->capacity > 0.75) {
        rehash(map);
    }
}

void* getFromHashMap(DynamicHashMap* map, const char* key) {
    unsigned int index = hash(key, map->capacity);
    KeyValue* current = map->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void freeHashMap(DynamicHashMap* map) {
    for (size_t i = 0; i < map->capacity; i++) {
        KeyValue* current = map->table[i];
        while (current) {
            KeyValue* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(map->table);
}
