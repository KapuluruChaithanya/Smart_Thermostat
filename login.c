#include "http.h"
#include "jsmn.h"
#include "uart.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_USERS 10
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

#define MAX_JSON_LENGTH 1000
char JSON_STRING[MAX_JSON_LENGTH];

struct User {
  char username[MAX_USERNAME_LENGTH];
  char password[MAX_PASSWORD_LENGTH];
};

#define DEVICE_NAME "file.txt"

char user_name1[MAX_USERNAME_LENGTH];
char user_password1[MAX_PASSWORD_LENGTH];

struct Schedule {
  int temperature;
  int hour;
  int minute;
};

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void getPassword(char *password, int maxLength) {
  int i = 0;
  while (i < maxLength - 1) {
    char c = getchar();
    if (c == '\n' || c == EOF)
      break;
    password[i++] = c;
    printf("*");
    fflush(stdout);
  }
  password[i] = '\0';
}

void displayOptionsAfterLogin() {
  printf("\n         Options:\n");
  printf("         1. Set Temperature\n");
  printf("         2. Get Temperature\n");
  printf("         3. Schedule Time\n");
  printf("         4. Logout\n");
  printf("\nEnter your choice: ");
}

void scheduleTemperature(struct Schedule *schedule) {
  printf("Enter time (HH:MM) to schedule: ");
  scanf("%d:%d", &schedule->hour, &schedule->minute);

  printf("Enter temperature  to schedule: ");
  scanf("%d", &schedule->temperature);
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
                      ((schedule->minute - current_minute) * 60) -
                      current_second;

  if (delay_seconds < 0) {
    delay_seconds += 24 * 3600;
  }

  return delay_seconds;
}

void createJSON(struct Schedule *schedule, char *json_string, int size) {
  snprintf(json_string, sizeof(json_string),
           "{\"username\":\"%s\",\"password\":\"%s\","
           "\"temperature\":%d}",
           user_name1, user_password1, schedule->temperature);
  // snprintf(json_string, size, "{\"temperature\":%d}", schedule->temperature);
}

void writeToFile(const char *json_string) {
  FILE *file = fopen(DEVICE_NAME, "w");
  if (file != NULL) {
    fprintf(file, "%s\n", json_string);
    fclose(file);
    // printf("JSON data written to %s\n", DEVICE_NAME);
  } else {
    printf("Error opening file for writing\n");
  }
}

void *scheduleThread(void *arg) {
  struct Schedule *schedule = (struct Schedule *)arg;

  int delay_seconds = calculateDelay(schedule);

  // printf("Waiting for scheduled time...\n");
  sleep(delay_seconds);

  char json_string[100];
  // createJSON(schedule, json_string, sizeof(json_string));
  snprintf(json_string, sizeof(json_string),
           "{\"username\":\"%s\",\"password\":\"%s\","
           "\"temperature\":%d}",
           user_name1, user_password1, schedule->temperature);

  // writeToFile(json_string);
  http_call(json_string,false);

  return NULL;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  // printf("\ns==%s|strlen(s)==%d|tok->end - tok->start==%d\n",
  // s,(int)strlen(s),tok->end - tok->start);
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

void jasmin_Parser(char *JSON_STRING) {
  int i;
  int r;
  jsmn_parser p;
  jsmntok_t t[500];

  jsmn_init(&p);
  r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                 sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    printf("Failed to parse JSON: %d\n", r);
    return;
  }

  /* Assume the top-level element is an object */
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    return;
  }

#ifndef print_json
  // printf("Number of tookens found==%d",r);
  /* Loop over all keys of the root object */
  for (i = 1; i < r; i++) {
    if (jsoneq(JSON_STRING, &t[i], "username") == 0) {
      /* We may use strndup() to fetch string value */
      // printf("\n-Username: %.*s\n", t[i + 1].end - t[i + 1].start,
      //        JSON_STRING + t[i + 1].start);
      i++;
    } else if (jsoneq(JSON_STRING, &t[i], "password") == 0) {
      /* We may additionally check if the value is either "true" or "false" */
      // printf("\n-Password: %.*s\n", t[i + 1].end - t[i + 1].start,
      //        JSON_STRING + t[i + 1].start);
      i++;
    } else if (jsoneq(JSON_STRING, &t[i], "temperature") == 0) {
      /* We may want to do strtol() here to get numeric value */

      // printf("\n Current Temperature is :%.*s\n", t[i + 1].end - t[i +
      // 1].start,
      //        JSON_STRING + t[i + 1].start);

      printf("\033[1;36m"); // ANSI escape sequence to set text color to cyan
                            // (bright)
      printf(" ╔════════════════════════╗\n");
      printf(" ║                        ║\n");
      printf(" ║  Current Temperature   ║\n");
      printf(" ║                        ║\n");
      printf(" ║           %.*s           ║\n", t[i + 1].end - t[i + 1].start,
             JSON_STRING + t[i + 1].start);
      printf(" ║                        ║\n");
      printf(" ╚════════════════════════╝\n");
      printf("\033[0m"); // Reset text color to default

      i++;

    } else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
      // int j;
      // printf("- Groups:\n");
      // if (t[i + 1].type != JSMN_ARRAY) {
      //   continue; /* We expect groups to be an array of strings */
      // }
      // for (j = 0; j < t[i + 1].size; j++) {
      //   jsmntok_t *g = &t[i + j + 2];
      //   printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
      // }
      // i += t[i + 1].size + 1;
    } else {
      printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
             JSON_STRING + t[i].start);
    }
  }
#endif
}

int main() {
  struct User users[MAX_USERS];
  int num_users = 0;

  pthread_t tid;
  struct Schedule schedule;

  int operation;
  strcpy(users[num_users].username, "usr");
  strcpy(users[num_users].password, "111");
  num_users++;

  while (true) {
    system("clear");
    printf("====================================\n");
    printf("         SMART THERMOSTAT           \n");
    printf("====================================\n");
    printf("\n         Options:\n");
    printf("         1. Login\n");
    printf("         2. Sign Up\n");
    printf("         3. Quit\n");
    printf("\nEnter your choice: ");

    int choice;
    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Please enter a number between 1 and 3.\n");
      continue;
    }

    clearInputBuffer();

    switch (choice) {
    case 1:
      system("clear");
      printf("\nEnter Username: ");
      fgets(user_name1, sizeof(user_name1), stdin);
      user_name1[strcspn(user_name1, "\n")] = '\0';

      printf("Enter Password: ");
      getPassword(user_password1, sizeof(user_password1));
      printf("\n");

      bool login_successful = false;
      for (int i = 0; i < num_users; i++) {
        if ((strcmp(users[i].username, user_name1) == 0) &&
            (strcmp(users[i].password, user_password1) == 0)) {
          printf("Login successful!\n");
          login_successful = true;
          do {
            displayOptionsAfterLogin();

            if (scanf("%d", &operation) != 1) {
              printf("Invalid input. Please enter a number between 1 and 4.\n");
              continue;
            }

            clearInputBuffer();

            switch (operation) {
            case 1:
              printf("\nSet the Temperature Value:");
              int temp;
              if (scanf("%d", &temp) != 1) {
                printf("Invalid input. Please enter a proper Temp.\n");
                continue;
              }
              snprintf(JSON_STRING, sizeof(JSON_STRING),
                       "{\"username\":\"%s\",\"password\":\"%s\","
                       "\"temperature\":%d}",
                       user_name1, user_password1, temp);
              // printf("JSON string: %s\n", JSON_STRING);
              getchar();
              http_call(JSON_STRING,true);
              printf("\nPress Enter key to go back.\n");
              getchar();
              system("clear");
              break;
            case 2:
              printf("\nGetting the Temperature Value...\n");
              sleep(2);
              printf("\nTemperature data requested successfully.\n");
              sleep(2);
              // Wait for response and parse temperature data
              char uart_response[512];
              int uart_fd = open("thermostat.txt", O_RDONLY);
              int bytes_received =
                  uart_receive(uart_fd, uart_response, sizeof(uart_response));
              if (bytes_received < 0) {
                printf("Error receiving response from UART.\n");
                // Handle error
              } else {
                // Parse the response JSON to extract temperature data
                // printf("data read==%s", uart_response);

                jasmin_Parser(uart_response);
                printf("\nPress Enter key to go back.\n");
                getchar();
                system("clear");
              }
              break;
            case 3:

              printf(
                  "\n----------Shedule the Time and Temperature----------\n");

              // Get user input and schedule temperature
              scheduleTemperature(&schedule);

              // Create thread for scheduling and waiting
              if (pthread_create(&tid, NULL, scheduleThread,
                                 (void *)&schedule) != 0) {
                perror("Error creating thread");
                return 1;
              }

              printf("\nTemperature sucessfully Sheduled :)\n");

              // Continue running main program while waiting
              // Add your main program logic here
              // For example, handle user input and perform other action
              /// Clear input buffer to remove newline character
              clearInputBuffer();
              printf("\nPress Enter key to go back.\n");
              getchar();
              system("clear");
              break;
            case 4:
              printf("\nLogging out...\n");
              break;
            }
          } while (operation != 4);

          break;
        }
      }

      if (!login_successful) {
        printf("Incorrect username or password.\n");
      }
      break;

    case 2:
      if (num_users < MAX_USERS) {
        system("clear");
        printf("\nEnter new Username: ");
        fgets(users[num_users].username, sizeof(users[num_users].username),
              stdin);
        users[num_users].username[strcspn(users[num_users].username, "\n")] =
            '\0';

        printf("Enter new Password: ");
        getPassword(users[num_users].password,
                    sizeof(users[num_users].password));
        printf("\n");

        printf("User signed up successfully!\n");
        num_users++;
      } else {
        printf("Maximum user limit reached. Cannot sign up.\n");
      }
      break;

    case 3:
      pthread_join(tid, NULL); // Wait for thread to finish
      printf("Powering off...\n");
      sleep(2);
      return 0;

    default:
      printf("Invalid choice. Please enter a number between 1 and 3.\n");
    }

    printf("Press Enter to continue...");
    getchar(); // Wait for Enter key
  }

  return 0;
}
