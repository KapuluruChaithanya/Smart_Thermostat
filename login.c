#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERS 10
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

// Struct to represent a user
struct User {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
};

int main() {
    // Array to store user accounts
    struct User users[MAX_USERS];
    int num_users = 0;

    // Default user
    strcpy(users[num_users].username, "Thermostat");
    strcpy(users[num_users].password, "1111");
    num_users++;

    char user_name1[MAX_USERNAME_LENGTH];
    char user_password1[MAX_PASSWORD_LENGTH];

    printf("********** SMART ThermoStart ***************\n");

    while (true) {
        printf("\nOptions:\n1. Login\n2. Sign Up\n3. Quit\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        switch (choice) {
            case 1:
                printf("\nEnter Username: ");
                fgets(user_name1, sizeof(user_name1), stdin);
                user_name1[strcspn(user_name1, "\n")] = '\0'; // Remove newline

                printf("Enter Password: ");
                fgets(user_password1, sizeof(user_password1), stdin);
                user_password1[strcspn(user_password1, "\n")] = '\0'; // Remove newline

                // Check if the entered credentials match any user
                bool login_successful = false;
                for (int i = 0; i < num_users; i++) {
                    if ((strcmp(users[i].username, user_name1) == 0) && (strcmp(users[i].password, user_password1) == 0)) {
                        printf("Login successful!\n");
                        login_successful = true;
                        break;
                    }
                }

                if (!login_successful) {
                    printf("Incorrect username or password.\n");
                }
                break;

            case 2:
                if (num_users < MAX_USERS) {
                    printf("\nEnter new Username: ");
                    fgets(users[num_users].username, sizeof(users[num_users].username), stdin);
                    users[num_users].username[strcspn(users[num_users].username, "\n")] = '\0'; // Remove newline

                    printf("Enter new Password: ");
                    fgets(users[num_users].password, sizeof(users[num_users].password), stdin);
                    users[num_users].password[strcspn(users[num_users].password, "\n")] = '\0'; // Remove newline

                    printf("User signed up successfully!\n");
                    num_users++;
                } else {
                    printf("Maximum user limit reached. Cannot sign up.\n");
                }
                break;

            case 3:
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice. Please enter a number between 1 and 3.\n");
        }
    }

    return 0;
}
