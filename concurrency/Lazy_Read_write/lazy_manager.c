#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_FILES 10

// Struct to hold request information
typedef struct
{
    int user_id;
    int file_id;
    char operation[10];
    int request_time;
} Request;

// Global variables
int r, w, d, n, c, T;
int file_status[MAX_FILES] = {0}; // 0: available, 1: writing, -1: deleted
int active_users[MAX_FILES] = {0};
int file_exists[MAX_FILES] = {0};
pthread_mutex_t file_mutex[MAX_FILES];
pthread_cond_t file_cond[MAX_FILES];
int request_count = 0;

pthread_t threads[MAX_USERS];
Request requests[MAX_USERS];

// Time tracker
time_t start_time;

// Function to get elapsed time
int get_time()
{
    return (int)(time(NULL) - start_time);
}

// Function to print messages with colors
void print_message(const char *message, const char *color)
{
    printf("%s%s%s\n", color, message, "\033[0m");
}

// Function to handle each request
void *handle_request(void *arg)
{
    Request *req = (Request *)arg;
    int elapsed_time = req->request_time;

    // Wait until request time
    while (get_time() < elapsed_time)
    {
        sleep(1);
    }

    char msg[100];
    snprintf(msg, sizeof(msg), "User %d has made request for performing %s on file %d at %d seconds",
             req->user_id, req->operation, req->file_id + 1, elapsed_time);
    print_message(msg, "\033[33m"); // Yellow color

    // Wait 1 second for LAZY to start processing
    sleep(1);

    pthread_mutex_lock(&file_mutex[req->file_id]);

    // Check impatience limit
    if (get_time() - elapsed_time >= T)
    {
        snprintf(msg, sizeof(msg), "User %d canceled the request due to no response at %d seconds",
                 req->user_id, get_time());
        print_message(msg, "\033[31m"); // Red color
        pthread_mutex_unlock(&file_mutex[req->file_id]);
        return NULL;
    }

    // snprintf(msg, sizeof(msg), "LAZY has taken up the request of User %d at %d seconds", req->user_id, get_time());
    // print_message(msg, "\033[35m"); // Pink color

    if (strcmp(req->operation, "READ") == 0)
    {

        if (!file_exists[req->file_id])
        {
            snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                     req->user_id, get_time());
            print_message(msg, "\033[37m"); // White color
            pthread_mutex_unlock(&file_mutex[req->file_id]);
            return NULL;
        }

        // Wait if read limit is reached
        while (active_users[req->file_id] >= c || file_status[req->file_id] == -1)
        {
            pthread_cond_wait(&file_cond[req->file_id], &file_mutex[req->file_id]);

            // Check if the file still exists after waking up
            if (!file_exists[req->file_id])
            {
                snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                         req->user_id, get_time());
                print_message(msg, "\033[37m"); // White color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                free(req);
                return NULL;
            }
            if (get_time() - elapsed_time >= T)
            {
                snprintf(msg, sizeof(msg), "User %d canceled the request due to no response at %d seconds",
                         req->user_id, get_time());
                print_message(msg, "\033[31m"); // Red color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                return NULL;
            }
        }

        // Start reading
        snprintf(msg, sizeof(msg), "LAZY has taken up the request of User %d at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[35m"); // Pink color
        active_users[req->file_id]++;
        pthread_mutex_unlock(&file_mutex[req->file_id]);

        // Simulate read time
        sleep(r);

        pthread_mutex_lock(&file_mutex[req->file_id]);
        active_users[req->file_id]--;
        snprintf(msg, sizeof(msg), "The request for User %d was completed at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[32m"); // Green color
        pthread_cond_broadcast(&file_cond[req->file_id]);
        pthread_mutex_unlock(&file_mutex[req->file_id]);
    }
    else if (strcmp(req->operation, "WRITE") == 0)
    {
        if (!file_exists[req->file_id])
        {
            snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                     req->user_id, get_time());
            print_message(msg, "\033[37m"); // White color
            pthread_mutex_unlock(&file_mutex[req->file_id]);
            free(req);
            return NULL;
        }

        // Wait if read limit is reached
        while (active_users[req->file_id] >= c || file_status[req->file_id] != 0)
        {
            pthread_cond_wait(&file_cond[req->file_id], &file_mutex[req->file_id]);

            // Check if the file still exists after waking up
            if (!file_exists[req->file_id])
            {
                snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                         req->user_id, get_time());
                print_message(msg, "\033[37m"); // White color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                return NULL;
            }
            if (get_time() - elapsed_time >= T)
            {
                snprintf(msg, sizeof(msg), "User %d canceled the request due to no response at %d seconds",
                         req->user_id, get_time());
                print_message(msg, "\033[31m"); // Red color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                return NULL;
            }
        }

        // Start writing
        snprintf(msg, sizeof(msg), "LAZY has taken up the request of User %d at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[35m"); // Pink color
        file_status[req->file_id] = 1;
        active_users[req->file_id]++;
        pthread_mutex_unlock(&file_mutex[req->file_id]);

        // Simulate write time
        sleep(w);

        pthread_mutex_lock(&file_mutex[req->file_id]);
        file_status[req->file_id] = 0;
        active_users[req->file_id]--;
        snprintf(msg, sizeof(msg), "The request for User %d was completed at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[32m"); // Green color
        pthread_cond_broadcast(&file_cond[req->file_id]);
        pthread_mutex_unlock(&file_mutex[req->file_id]);
    }
    else if (strcmp(req->operation, "DELETE") == 0)
    {

        // Decline immediately if the file doesn't exist
        if (!file_exists[req->file_id])
        {
            snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                     req->user_id, get_time());
            print_message(msg, "\033[37m"); // White color
            pthread_mutex_unlock(&file_mutex[req->file_id]);
            return NULL;
        }

        // Wait if any read or write is in progress
        while (file_status[req->file_id] != 0 || active_users[req->file_id] > 0)
        {
            pthread_cond_wait(&file_cond[req->file_id], &file_mutex[req->file_id]);

            // Check if the file still exists after waking up
            if (!file_exists[req->file_id])
            {
                snprintf(msg, sizeof(msg), "LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.",
                         req->user_id, get_time());
                print_message(msg, "\033[37m"); // White color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                return NULL;
            }
            if (get_time() - elapsed_time >= T)
            {
                snprintf(msg, sizeof(msg), "User %d canceled the request due to no response at %d seconds",
                         req->user_id, get_time());
                print_message(msg, "\033[31m"); // Red color
                pthread_mutex_unlock(&file_mutex[req->file_id]);
                return NULL;
            }
        }

        // Start deleting
        snprintf(msg, sizeof(msg), "LAZY has taken up the request of User %d at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[35m"); // Pink color
        file_status[req->file_id] = -1;
        active_users[req->file_id]++;
        pthread_mutex_unlock(&file_mutex[req->file_id]);

        // Simulate delete time
        sleep(d);

        pthread_mutex_lock(&file_mutex[req->file_id]);
        file_status[req->file_id] = 0;
        active_users[req->file_id]--;
        file_exists[req->file_id] = 0; // Mark file as deleted
        snprintf(msg, sizeof(msg), "The request for User %d was completed at %d seconds", req->user_id, get_time());
        print_message(msg, "\033[32m");                   // Green color
        pthread_cond_broadcast(&file_cond[req->file_id]); // Notify other threads that may be waiting
        pthread_mutex_unlock(&file_mutex[req->file_id]);
    }

    return NULL;
}

void initialize()
{
    // Initialize files as available
    for (int i = 0; i < n; i++)
    {
        file_status[i] = 0;
        file_exists[i] = 1;
        pthread_mutex_init(&file_mutex[i], NULL);
        pthread_cond_init(&file_cond[i], NULL);
    }
    start_time = time(NULL);
}

void simulate_lazy_manager()
{
    print_message("LAZY has woken up!", "\033[35m"); // Pink color
    initialize();

    for (int i = 0; i < request_count; i++)
    {
        pthread_create(&threads[i], NULL, handle_request, &requests[i]);
    }

    for (int i = 0; i < request_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("LAZY has no more pending requests and is going back to sleep!");
}

int main()
{
    // Input parsing
    scanf("%d %d %d", &r, &w, &d);
    scanf("%d %d %d", &n, &c, &T);

    while (1)
    {
        char operation[10];
        int user_id, file_id, request_time;
        scanf("%s", operation);
        if (strcmp(operation, "STOP") == 0)
            break;
        user_id = atoi(operation);
        scanf("%d %s %d", &file_id, requests[request_count].operation, &request_time);

        requests[request_count].user_id = user_id;
        requests[request_count].file_id = file_id - 1;
        requests[request_count].request_time = request_time;
        request_count++;
    }

    simulate_lazy_manager();

    return 0;
}
