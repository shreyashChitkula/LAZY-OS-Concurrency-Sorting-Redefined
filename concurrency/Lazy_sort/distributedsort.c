#include "countfile.h"
#include "mergesort.h"

File files[MAX_FILES];
File sorted_files[MAX_FILES];
pthread_mutex_t lock;
char sortColumn[16];

int sort_byc=0;

void printArray(File arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        char timestampStr[20];
        strftime(timestampStr, sizeof(timestampStr), "%Y-%m-%dT%H:%M:%S", &arr[i].timestamp);
        printf("%s %d %s\n", arr[i].name, arr[i].id, timestampStr);
    }
}

int main()
{
    int fileCount;
    scanf("%d", &fileCount);

    if(fileCount <= 42){
        countsort(fileCount);
        
        return 0;
    }

    pthread_mutex_init(&lock, NULL);

    // Input reading
    for (int i = 0; i < fileCount; i++)
    {
        char timestampStr[20];
        scanf("%s %d %s", files[i].name, &files[i].id, timestampStr);
        if (!parseTimestamp(timestampStr, &files[i].timestamp))
            exit(1);
    }
    scanf("%s", sortColumn);

    // Determine sorting method based on sortColumn

    if (fileCount > 42)
    {
        printf("\n\n merge sort \n\n");
        for (int i = 0; i < fileCount; i++)
        {
            char timestampStr[20];
            scanf("%s %d %s", files[i].name, &files[i].id, timestampStr);
            if (!parseTimestamp(timestampStr, &files[i].timestamp))
                exit(1);
        }
        scanf("%s", sortColumn);

        // Start merge sort
        ThreadArgs args = {0, fileCount - 1, 0};

        pthread_t tid;
        pthread_create(&tid, NULL, mergeSort, &args);
        pthread_join(tid, NULL);
    }

    printArray(files, fileCount);
    pthread_mutex_destroy(&lock);
    return 0;
}