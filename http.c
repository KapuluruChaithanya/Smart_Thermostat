#include "http.h"




void http_set_status(http_part *part, const char *status) {
    part->type = HTTP_STATUS;
    snprintf(part->data, sizeof(part->data), "HTTP/1.1 %s\r\n", status);
}

void http_set_header(http_part *part, const char *header_name, const char *header_value) {
    part->type = HTTP_HEADER;
    snprintf(part->data, sizeof(part->data), "%s: %s\r\n", header_name, header_value);
}

void http_set_body(http_part *part, const char *json_string) {
    part->type = HTTP_BODY;
    snprintf(part->data, sizeof(part->data), "%s", json_string);
}

void http_print_parts(http_part *parts, int num_parts) {
     printf("\nhttp Packet:\n");
    for (int i = 0; i < num_parts; i++) {
        printf("%s", parts[i].data);
    }
    printf("\nparts printed \n");
}

int http_call(char *JSON_STRING , bool display_log) {
    http_part parts[3];
    memset(parts, 0, sizeof(parts));

if (display_log){
    printf("\nCreating http Packet...\n");
}
    http_set_status(&parts[0], "200 OK");
    http_set_header(&parts[1], "Content-Type", "application/json");
    http_set_header(&parts[2], "Content-Length", "0");
    http_set_body(&parts[2], JSON_STRING);

    
    // http_print_parts(parts, 3);

    // Concatenate parts into a single string
    char http_packet[1024];
    memset(http_packet, 0, sizeof(http_packet));
    // strcat(http_packet, parts[0].data);
    // strcat(http_packet, parts[1].data);
    strcat(http_packet, parts[2].data);

    // printf("\nHTTP Packet:\n%s\n", http_packet);

    int uart_fd;

    uart_fd = uart_init();
    if (uart_fd < 0) {
        fprintf(stderr, "Failed to initialize UART\n");
        return 1;
    }
if (display_log){
    printf("\nSending Data through uart..\n");
}
    int bytes_sent = uart_send(uart_fd,  http_packet);
    if (bytes_sent < 0) {
        fprintf(stderr, "Failed to send JSON data over UART\n");
        // close(uart_fd);
        return 1;
    }

uart_close(uart_fd);

    sleep(2);

if (display_log){
    printf("\nData trasmited to Thermostat :) \n");
}



    // sleep(5);
    // system("clear");
    return 0;
}
