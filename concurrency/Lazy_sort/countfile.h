#ifndef COUNTFILE_H
#define COUNTFILE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

// Structure to represent a file
typedef struct
{
    char *name;
    int id;
    char timestamp[20];
} Filec;

extern int sort_byc;

// Structure to hold thread arguments
typedef struct
{
    Filec **arr;
    int start;
    int end;
    int min_val;
    int max_val;
    int *local_count;
} ThreadArgsc;

int map_id(int id);

int map_timestamp(char *str);

int map_string(char *str);

void *count_segment(void *args);

void distributed_file_sort(Filec **arr, int n, int num_threads);

int countsort(int num);

#endif