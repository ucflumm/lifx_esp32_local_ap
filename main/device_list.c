#include <stdlib.h>
#include <stdint.h>
#include "include/lifx_struct_def.h"
#include "include/main.h "

lifx_device_t *add_device_to_list(lifx_device_t **head, const char *ip, uint32_t port, uint8_t service) {
    lifx_device_t *current = *head;
    lifx_device_t *prev = NULL;

    // Check if the device already exists and update it
    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0) {
            current->service = service;
            current->port = port;
            return *head;  // Return the list head
        }
        prev = current;
        current = current->next;
    }

    // If not found, add new device
    lifx_device_t *new_device = (lifx_device_t *)malloc(sizeof(lifx_device_t));
    if (new_device == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    strcpy(new_device->ip, ip);
    new_device->service = service;
    new_device->port = port;
    new_device->next = *head;
    *head = new_device;

    return new_device;  // Return the new node
}

void remove_device(lifx_device_t **head, const char *ip) {
    lifx_device_t *current = *head;
    lifx_device_t *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0) {
            if (prev == NULL) {
                *head = current->next;  // Remove the head
            } else {
                prev->next = current->next;  // Bypass the current node
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

lifx_device_t *find_device(lifx_device_t *head, const char *ip) {
    lifx_device_t *current = head;
    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}