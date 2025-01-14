#include "countfile.h"

// Mapping functions
int map_id(int id)
{
    return id;
}

int map_timestamp(char *str)
{
    // int year, month, day, hour, minute, second;
    // if (sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second) == 6)
    // {
    //     return year * 31536000 + month * 2592000 + day * 86400 + hour * 3600 + minute * 60 + second;
    // }
    // return -1; // Return a negative value for invalid timestamps

    int year, month, day, hour, minute, second;
    sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second);
    return year * 31536000 + month * 2592000 + day * 86400 + hour * 3600 + minute * 60 + second;
}

int map_string(char *str)
{
    int value = 0;

    for (int i = 0; i < strlen(str) && str[i] != '.'; i++)
    {
        // Compute 26^(character value) for the current character
        int char_value = (int)pow(26, (str[i] - 'A' + 1));

        // Add positional weight by multiplying by a small factor based on position
        value += char_value + i * 100;
    }

    return value;
}

// Thread function to count frequencies in a segment
void *count_segment(void *args)
{
    ThreadArgsc *thread_args = (ThreadArgsc *)args;
    int range = thread_args->max_val - thread_args->min_val + 1;

    // Initialize local count array
    for (int i = 0; i < range; i++)
    {
        thread_args->local_count[i] = 0;
    }

    // Count frequencies in this segment
    for (int i = thread_args->start; i < thread_args->end; i++)
    {
        Filec *file = thread_args->arr[i];
        int index = 0;
        switch (sort_byc)
        {
        case 0: // Sort by Name
            index = map_string(file->name) - thread_args->min_val;
            break;
        case 1: // Sort by ID
            index = map_id(file->id) - thread_args->min_val;
            break;
        case 2: // Sort by Timestamp
            index = map_timestamp(file->timestamp) - thread_args->min_val;
            // if (index < 0)
            // {
            //     index = -1; // Assign a negative index for invalid timestamps
            // }
            // break;
        }
        if (index >= 0)
        {
            thread_args->local_count[index]++;
        }
    }

    pthread_exit(NULL);
}

void distributed_file_sort(Filec **arr, int n, int num_threads)
{

    if (n <= 1)
        return;

    // Find the column to sort by
    char sort_by_str[100];
    // printf("enter the parameter:");
    scanf("%s", sort_by_str);
    if (strcmp(sort_by_str, "Name") == 0)
    {
        sort_byc = 0;
    }
    else if (strcmp(sort_by_str, "ID") == 0)
    {
        sort_byc = 1;
    }
    else if (strcmp(sort_by_str, "Timestamp") == 0)
    {
        sort_byc = 2;
    }
    else
    {
        printf("Invalid parameter to sort!!!\n");
        return;
    }
    // printf("value of sort_byc %d\n",sort_byc);

    // Find range of mapped values
    int min_val, max_val;
    switch (sort_byc)
    {
    case 0: // Sort by Name
        min_val = map_string(arr[0]->name);
        max_val = min_val;
        for (int i = 1; i < n; i++)
        {
            int value = map_string(arr[i]->name);
            if (value < min_val)
                min_val = value;
            if (value > max_val)
                max_val = value;
        }
        break;
    case 1: // Sort by ID
        min_val = map_id(arr[0]->id);
        max_val = min_val;
        for (int i = 1; i < n; i++)
        {
            int value = map_id(arr[i]->id);
            if (value < min_val)
                min_val = value;
            if (value > max_val)
                max_val = value;
        }
        break;
    case 2: // Sort by Timestamp
        min_val = map_timestamp(arr[0]->timestamp);
        max_val = min_val;
        for (int i = 0; i < n; i++)
        {
            int value = map_timestamp(arr[i]->timestamp);
            // if (value >= 0 && (value < min_val || min_val == 0)) {
            //     min_val = value;
            // }
            // if (value >= 0 && value > max_val) {
            //     max_val = value;
            // }
            if (value < min_val)
                min_val = value;
            if (value > max_val)
                max_val = value;
        }
        // if (min_val == 0 && max_val == 0)
        // {
        //     min_val = -1; // Set min_val to -1 if all timestamps are invalid
        //     max_val = -1;
        // }
        break;
    }
    int range = max_val - min_val + 1;

    // Create threads and their arguments
    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    ThreadArgsc *thread_args = (ThreadArgsc *)malloc(num_threads * sizeof(ThreadArgsc));
    int **local_counts = (int **)malloc(num_threads * sizeof(int *));

    // Allocate local count arrays for each thread
    for (int i = 0; i < num_threads; i++)
    {
        local_counts[i] = (int *)calloc(range, sizeof(int));
    }

    // Create total count array
    int *total_count = (int *)calloc(range, sizeof(int));

    // Calculate segment size for each thread
    int segment_size = n / num_threads;
    int remainder = n % num_threads;
    int current_start = 0;

    // Create and start threads
    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i].arr = arr;
        thread_args[i].start = current_start;
        thread_args[i].end = current_start + segment_size + (i < remainder ? 1 : 0);
        thread_args[i].min_val = min_val;
        thread_args[i].max_val = max_val;
        thread_args[i].local_count = local_counts[i];

        pthread_create(&threads[i], NULL, count_segment, &thread_args[i]);

        current_start = thread_args[i].end;
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Combine count arrays
    for (int i = 0; i < num_threads; i++)
    {
        for (int j = 0; j < range; j++)
        {
            total_count[j] += local_counts[i][j];
        }
    }

    // Reconstruct sorted array
    Filec **sorted_arr = (Filec **)malloc(n * sizeof(Filec *));
    int index = 0;
    for (int i = 0; i < range; i++)
    {
        int value = i + min_val;
        while (total_count[i] > 0)
        { // Only loop while there's count left for this mapped value
            for (int k = 0; k < n; k++)
            {
                int map_value = (sort_byc == 0) ? map_string(arr[k]->name) : (sort_byc == 1) ? map_id(arr[k]->id)
                                                                                             : map_timestamp(arr[k]->timestamp);
                if (map_value == value)
                {
                    sorted_arr[index++] = arr[k];
                    total_count[i]--; // Decrement after adding a file
                    break;            // Exit inner loop once a match is found
                }
            }
        }
    }

    // Replace original array with sorted array
    for (int i = 0; i < n; i++)
    {
        arr[i] = sorted_arr[i];
    }

    // Free allocated memory
    for (int i = 0; i < num_threads; i++)
    {
        free(local_counts[i]);
    }
    free(local_counts);
    free(total_count);
    free(threads);
    free(thread_args);
    free(sorted_arr);
}

int countsort(int num_files)
{
    // int num_files;
    // scanf("%d", &num_files);

    // printf("enterd %d\n",num_files);

    Filec **files = (Filec **)malloc(num_files * sizeof(Filec *));
    int valid_files = 0;

    // printf("Enter the file names:\n");

    for (int i = 0; i < num_files; i++)
    {
        files[i] = (Filec *)malloc(sizeof(Filec));
        char name[100];
        if (scanf("%s %d %s", name, &files[i]->id, files[i]->timestamp) == 3)
        {
            files[i]->name = strdup(name);
            valid_files++;
        }
        else
        {
            // Handle invalid input
            printf("Invalid input for file %d, skipping...\n", i);
            free(files[i]);
            files[i] = NULL;
            // Don't decrement num_files, as it's already done in the loop
        }
    }

    distributed_file_sort(files, valid_files, 4);

    for (int i = 0; i < valid_files; i++)
    {
        if (files[i] != NULL)
        {
            printf("%s %d %s\n", files[i]->name, files[i]->id, files[i]->timestamp);
            free(files[i]->name);
            free(files[i]);
        }
    }
    free(files);

    return 0;
}