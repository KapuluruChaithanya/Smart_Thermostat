#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"


typedef enum {
    HTTP_STATUS,
    HTTP_HEADER,
    HTTP_BODY
} http_part_type;

typedef struct {
    http_part_type type;
    char data[2048]; // Adjust the size as needed
} http_part;



// Function to set the HTTP status line
void http_set_status(http_part *part, const char *status);


// Function to set the HTTP headers
void http_set_header(http_part *part, const char *header_name, const char *header_value);

// Function to set the HTTP body
void http_set_body(http_part *part, const char *json_string);

void http_print_parts(http_part *parts, int num_parts);


// Modified http_call function to accept JSON_STRING as an argument
int http_call(char *JSON_STRING,bool display_log);