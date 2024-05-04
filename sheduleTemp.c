#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define FILENAME "file.txt"

struct Schedule {
    int temperature;
    int hour;
    int minute;
};

void scheduleTemperature(struct Schedule *schedule) {
    printf("Enter temperature and time (HH:MM) to schedule: ");
    scanf("%d %d:%d", &schedule->temperature, &schedule->hour, &schedule->minute);
}


int calculateDelay(struct Schedule *schedule) {
    time_t now;
    struct tm *current_time;
    time(&now);
    current_time = localtime(&now);

    int current_hour = current_time->tm_hour;
    int current_minute = current_time->tm_min;
    int current_second = current_time->tm_sec;

    int delay_seconds = ((schedule->hour - current_hour) * 3600) +
                        ((schedule->minute - current_minute) * 60) - current_second;

    if (delay_seconds < 0) {
        delay_seconds += 24 * 3600; 
    }

    return delay_seconds;
}

void createJSON(struct Schedule *schedule, char *json_string, int size) {
    snprintf(json_string, size, "{\"temperature\":%d}", schedule->temperature);
}


void writeToFile(const char *json_string) {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        fprintf(file, "%s\n", json_string);
        fclose(file);
        printf("JSON data written to %s\n", FILENAME);
    } else {
        printf("Error opening file for writing\n");
    }
}


void *scheduleThread(void *arg) {
    struct Schedule *schedule = (struct Schedule *)arg;

    int delay_seconds = calculateDelay(schedule);

    printf("Waiting for scheduled time...\n");
    sleep(delay_seconds);

    char json_string[100];
    createJSON(schedule, json_string, sizeof(json_string));

    writeToFile(json_string);

    return NULL;
}
