#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <process.h>
#include <stdint.h>

volatile int running = 1;

typedef struct node
{
	char direction;
	time_t timestamp;
	struct node *next;
} activity_t;

double calculate_out_threshold(activity_t *head);
double calculate_in_threshold(activity_t *head);
void add_activity(activity_t **head, activity_t **tail, char direction, time_t timestamp);
void remove_old_entries(activity_t **head, time_t current_time);
void ensure_chronological(activity_t **head);
void printList(activity_t* head);
unsigned __stdcall check_time_thread(void *data);
void check_unusual_activity(activity_t **head, activity_t **tail);
void sos();
void print_current_datetime();
void print_in_threshold(activity_t *head);
void print_out_threshold(activity_t *head);

int main()
{
	double outThreshold = 0;
	double inThreshold = 0;
	activity_t *head = NULL;
    activity_t *tail = NULL;

    // Create a sample linked list of a person's activities for a week
    time_t t = time(NULL);
    struct tm *timeinfo = localtime(&t);
    print_current_datetime();
    timeinfo->tm_hour = timeinfo->tm_min = timeinfo->tm_sec = 0; // removed the time part for the customizing the sample linked list
    time_t curr_date = mktime(timeinfo);

    add_activity(&head, &tail, 'X', curr_date - 7 * 24 * 60 * 60 + 6 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 7 * 24 * 60 * 60 + 12 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 7 * 24 * 60 * 60 +  13 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 7 * 24 * 60 * 60 + 16 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 6 * 24 * 60 * 60 + 7 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 6 * 24 * 60 * 60 + 11 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 6 * 24 * 60 * 60  + 13 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 6 * 24 * 60 * 60 + 16 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 5 * 24 * 60 * 60 + 8 * 30 * 60);
    add_activity(&head, &tail, 'N', curr_date - 5 * 24 * 60 * 60 + 12 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 5 * 24 * 60 * 60 + 13 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 5 * 24 * 60 * 60 + 16 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 4 * 24 * 60 * 60 + 8 * 30 * 60);
    add_activity(&head, &tail, 'N', curr_date - 4 * 24 * 60 * 60 + 12 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 4 * 24 * 60 * 60 + 13 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 4 * 24 * 60 * 60 + 16 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 3 * 24 * 60 * 60 + 7 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 3 * 24 * 60 * 60 + 8 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 3 * 24 * 60 * 60 + 10 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 3 * 24 * 60 * 60 + 12 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 3 * 24 * 60 * 60 + 13 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 3 * 24 * 60 * 60 + 16 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 2 * 24 * 60 * 60 + 9  * 60);
    add_activity(&head, &tail, 'N', curr_date - 2 * 24 * 60 * 60 + 10 * 20 * 60);
    add_activity(&head, &tail, 'X', curr_date - 2 * 24 * 60 * 60 + 18 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 2 * 24 * 60 * 60 + 19 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 1 * 24 * 60 * 60 + 8 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 1 * 24 * 60 * 60 + 9 * 60 * 60);
    add_activity(&head, &tail, 'X', curr_date - 1 * 24 * 60 * 60 + 19 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 1 * 24 * 60 * 60 + 20 * 60 * 60);

    add_activity(&head, &tail, 'X', curr_date - 0 * 24 * 60 * 60 + 7 * 60 * 60);
    add_activity(&head, &tail, 'N', curr_date - 0 * 24 * 60 * 60 + 8 * 60 * 60);


    print_in_threshold(head);
    print_out_threshold(head);

    // run unusual activity checker in the background
    // create a new thread to run the check_time_thread function in the background
    activity_t **data[2] = {&head, &tail};
    uintptr_t thread = _beginthreadex(NULL, 0, check_time_thread, data, 0, NULL);
    if (thread == 0) {
        printf("Error creating thread\n");
        return -1;
    }

    int flag = 1;
    char inp[1];

    while (flag) {
        printf("\n=======================================================\n\tAI-ASSISTED ENTRY AND EXIT ALERT SYSTEM\n=======================================================\n[1] Record Entry\t[3] View Entry/Exit Log\n[2] Record Exit\t\t[4] Exit Program\n=======================================================\nPlease enter your choice: ");
        gets(inp);
        if(strcmp(inp, "1")== 0) {
            remove_old_entries(&head, t);
            add_activity(&head, &tail, 'N', time(NULL));
            printf("\nActivity detected: a person enetered the room.\n");
        }
        else if(strcmp(inp, "2")== 0) {
            add_activity(&head, &tail, 'X', time(NULL));
            printf("\nActivity detected: a person exited the room.\n");
        }
        else if(strcmp(inp, "3")== 0) {
            printf("------------------------------\n\tEntry/Exit Logs\n------------------------------\n");
            printList(head);
            printf("------------------------------");
        }
        else if(strcmp(inp, "4")== 0) {
            flag = 0;
           running = 0;
        }
        else {
            printf("Invalid choice.\n");
        }
        fflush(stdin);
    }

    // Wait for the background thread to finish before exiting the program
    WaitForSingleObject((HANDLE)thread, INFINITE);
    CloseHandle((HANDLE)thread);
    return 0;
}

void print_current_datetime() {
    time_t t = time(NULL);
    struct tm *timeinfo = localtime(&t);
    printf("\nCurrent date & time:%d/%02d/%02d %02d:%02d\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
}

void print_in_threshold(activity_t *head) {
    double threshold = calculate_in_threshold(head); // in seconds
    printf("\nIn Threshold:\t%dhrs - %dmin", (int)threshold/3600, ((int)threshold % 3600) / 60);
}

void print_out_threshold(activity_t *head) {
    double threshold = calculate_out_threshold(head); // in seconds
    printf("\nOut Threshold:\t%dhrs - %dmin", (int)threshold/3600, ((int)threshold % 3600) / 60);
}

unsigned __stdcall check_time_thread(void *data) {
    activity_t **head = ((activity_t ***)data)[0];
    activity_t **tail = ((activity_t ***)data)[1];

    while (running) {
        check_unusual_activity(head, tail);
        Sleep(5000);
    }
    return 0;
}

void check_unusual_activity(activity_t **head, activity_t **tail) {
    time_t curr_time = time(NULL);
    // for testing
    // struct tm* advance = localtime(&curr_time);
    // advance->tm_mday += 1;
    // curr_time = mktime(advance);

    struct tm* timeinfo = localtime(&((*tail)->timestamp));

    // Calculate the threshold for the longest time the person has been inside
    double inThreshold = calculate_in_threshold(*head); // in seconds
    double outThreshold = calculate_out_threshold(*head); // in seconds

    if((*tail)->direction == 'X') {
        if (difftime(curr_time, (*tail)->timestamp) > outThreshold) {
        printf("\n\nAlert: No entry detected since %d/%02d/%02d %02d:%02d\n\nPlease enter your choice: ", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
        sos();
        }
    } else if ((*tail)->direction == 'N'){
        if (difftime(curr_time, (*tail)->timestamp) > inThreshold) {
        printf("\n\nAlert: No exit detected since %d/%02d/%02d %02d:%02d\n\nPlease enter your choice: ", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
        sos();        
        }
    }
}

double calculate_out_threshold(activity_t *head) {
    activity_t *current = head;
    time_t last_exit_time = 0;
    int n = 0;
    double sum = 0;
    double sum_squared = 0;

    // Calculate the mean and standard deviation of the duration of all exit-entry events
    while (current != NULL) {
        if (current->direction == 'X') {
            // Exit
            last_exit_time = current->timestamp;
        } else if (current->direction == 'N' && last_exit_time != 0) {
            // Entry
            double duration_out = difftime(current->timestamp, last_exit_time);
            sum += duration_out;
            sum_squared += duration_out * duration_out;
            n++;
            last_exit_time = 0;
        }
        current = current->next;
    }

    double mean = sum / n;
    double variance = (sum_squared / n) - (mean * mean);
    double standard_deviation = sqrt(variance);

    // Define the threshold as some number of standard deviations away from the mean
    double threshold = mean + 2 * standard_deviation;

    return threshold;
}

double calculate_in_threshold(activity_t *head) {
    activity_t *current = head;
    time_t last_entry_time = 0;
    int n = 0;
    double sum = 0;
    double sum_squared = 0;

    // Calculate the mean and standard deviation of the duration of all entry-exit events
    while (current != NULL) {
        if (current->direction == 'N') {
            // Entry
            last_entry_time = current->timestamp;
        } else if (current->direction == 'X' && last_entry_time != 0) {
            // Exit
            double duration_in = difftime(current->timestamp, last_entry_time);
            sum += duration_in;
            sum_squared += duration_in * duration_in;
            n++;
            last_entry_time = 0;
        }
        current = current->next;
    }

    double mean = sum / n;
    double variance = (sum_squared / n) - (mean * mean);
    double standard_deviation = sqrt(variance);

    // Define the threshold as some number of standard deviations away from the mean
    double threshold = mean + 2 * standard_deviation;

    return threshold;
}

void add_activity(activity_t **head, activity_t **tail, char direction, time_t timestamp) {
    activity_t *new_entry = malloc(sizeof(activity_t));
    new_entry->direction = direction;
    new_entry->timestamp = timestamp;
    new_entry->next = NULL;

    if (*head == NULL) {
        *head = new_entry;
        *tail = new_entry;
    } else {
        (*tail)->next = new_entry;
        *tail = new_entry;
    }
}

// use tail
void remove_old_entries(activity_t **head, time_t current_time) {
    activity_t *current = *head;
    activity_t *prev = NULL;

    while (current != NULL) {
        // Check if the entry is older than three months
        if (difftime(current_time, current->timestamp) > 3 * 30 * 24 * 60 * 60) {
            // Remove the entry from the list
            if (prev == NULL) {
                // The entry is at the head of the list
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            current = prev ? prev->next : *head;
        } else {
            break;
        }
    }
}

void ensure_chronological(activity_t **head) {
    activity_t *current = *head;
    activity_t *prev = NULL;

    while (current != NULL && current->next != NULL) {
        if (difftime(current->next->timestamp, current->timestamp) < 0) {
            // The timestamps are out of order
            if (prev == NULL) {
                // The anomaly is at the head of the list
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            current = prev ? prev->next : *head;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void printList(activity_t* head) {
    activity_t* current = head;
    while(current != NULL) {
        struct tm* timeinfo = localtime(&(current->timestamp));
        printf("%s %d/%02d/%02d %02d:%02d\n", (current->direction == 'X') ? "[Exit] " : "[Entry]", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
        current = current->next;
    }
}

void sos() {
    // S: three short beeps
    Beep(750, 300);
    Sleep(100);
    Beep(750, 300);
    Sleep(100);
    Beep(750, 300);

    Sleep(300); // pause

    // O: three long beeps
    Beep(750, 900);
    Sleep(100);
    Beep(750, 900);
    Sleep(100);
    Beep(750, 900);

    Sleep(300); // pause

    // S: three short beeps
    Beep(750, 300);
    Sleep(100);
    Beep(750, 300);
    Sleep(100);
    Beep(750, 300);
}