
#ifndef UTILS_H
#define UTILS_H

#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


#define MAX_FILES 1000
#define MAX_NAME_LENGTH 128
#define MAX_ID 1000         // Assuming a range for IDs for count sort
#define MAX_YEAR 2100  




typedef struct {
    char name[MAX_NAME_LENGTH];
    int id;
    struct tm timestamp;
} File;


#endif