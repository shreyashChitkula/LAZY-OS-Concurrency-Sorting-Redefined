#include "mergesort.h"
 // Global variable for the sort column

// Function to parse ISO 8601 timestamps
int parseTimestamp(const char *timestampStr, struct tm *timestamp) {
    int year, month, day, hour, minute, second;
    if (sscanf(timestampStr, "%4d-%2d-%2dT%2d:%2d:%2d", 
               &year, &month, &day, &hour, &minute, &second) != 6) {
        return 0; // Return 0 for failure
    }
    timestamp->tm_year = year - 1900; // tm_year is year since 1900
    timestamp->tm_mon = month - 1;    // tm_mon is months since January (0-11)
    timestamp->tm_mday = day;
    timestamp->tm_hour = hour;
    timestamp->tm_min = minute;
    timestamp->tm_sec = second;
    timestamp->tm_isdst = -1;         // Not setting daylight saving time

    return 1; // Return 1 for success
}

// Comparators
int compareByName(const File *a, const File *b) { return strcmp(a->name, b->name); }
int compareById(const File *a, const File *b) { return a->id - b->id; }
int compareByTimestamp(const File *a, const File *b) {
    time_t time1 = mktime((struct tm *)&a->timestamp);
    time_t time2 = mktime((struct tm *)&b->timestamp);
    return (time1 > time2) - (time1 < time2);
}

// Merge function
void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    File L[n1], R[n2];
    for (int i = 0; i < n1; i++) L[i] = files[left + i];
    for (int j = 0; j < n2; j++) R[j] = files[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    int (*comparator)(const File *, const File *);
    if (strcmp(sortColumn, "Name") == 0) comparator = compareByName;
    else if (strcmp(sortColumn, "ID") == 0) comparator = compareById;
    else if (strcmp(sortColumn, "Timestamp") == 0) comparator = compareByTimestamp;
    else exit(1);

    pthread_mutex_lock(&lock);
    while (i < n1 && j < n2) files[k++] = comparator(&L[i], &R[j]) <= 0 ? L[i++] : R[j++];
    while (i < n1) files[k++] = L[i++];
    while (j < n2) files[k++] = R[j++];
    pthread_mutex_unlock(&lock);
}

// Limited-depth merge sort
void *mergeSort(void *args) {
    ThreadArgs *arg = (ThreadArgs *)args;
    int left = arg->left, right = arg->right, depth = arg->depth;

    if (left < right) {
        int mid = left + (right - left) / 2;
        ThreadArgs arg1 = {left, mid, depth + 1};
        ThreadArgs arg2 = {mid + 1, right, depth + 1};

        if (depth < 2) { // Limit thread depth for controlled parallelism
            pthread_t tid1, tid2;
            pthread_create(&tid1, NULL, mergeSort, &arg1);
            pthread_create(&tid2, NULL, mergeSort, &arg2);
            pthread_join(tid1, NULL);
            pthread_join(tid2, NULL);
        } else { // Use sequential merge sort beyond the threading depth
            mergeSort(&arg1);
            mergeSort(&arg2);
        }
        merge(left, mid, right);
    }
    return NULL;
}

int mergefile(int fileCount) {
    
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < fileCount; i++) {
        char timestampStr[20];
        scanf("%s %d %s", files[i].name, &files[i].id, timestampStr);
        if (!parseTimestamp(timestampStr, &files[i].timestamp)) exit(1);
    }
    scanf("%s", sortColumn);

    // Start merge sort
    ThreadArgs args = {0, fileCount - 1, 0};

    pthread_t tid;
    pthread_create(&tid, NULL, mergeSort, &args);
    pthread_join(tid, NULL);

    printArray(files, fileCount);
    pthread_mutex_destroy(&lock);
    return 0;
}
