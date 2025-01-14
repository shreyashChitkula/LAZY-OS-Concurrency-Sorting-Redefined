#ifndef MERGESORT_H
#define MERGESORT_H

#include "utils.h"

typedef struct {
    int left;
    int right;
    int depth;
} ThreadArgs;

extern File files[MAX_FILES];

extern pthread_mutex_t lock;
extern char sortColumn[16];

extern void printArray(File arr[], int size);

int parseTimestamp(const char *timestampStr, struct tm *timestamp);

int compareByName(const File *a, const File *b);

int compareById(const File *a, const File *b);

int compareByTimestamp(const File *a, const File *b);

void merge(int left, int mid, int right);

void *mergeSort(void *args);

int mergefile(int fileCount);

#endif