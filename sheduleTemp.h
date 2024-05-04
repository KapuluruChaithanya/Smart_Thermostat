#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>



void scheduleTemperature(struct Schedule *schedule);

int calculateDelay(struct Schedule *schedule);

void createJSON(struct Schedule *schedule, char *json_string, int size);


void writeToFile(const char *json_string);


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
