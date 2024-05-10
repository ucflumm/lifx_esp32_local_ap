#include "include/lifx_struct_def.h"
#include <stdlib.h>
#include "include/main.h"

lifx_device_t *head = NULL;

lifx_device_t* find_or_create_device(struct in_addr ip) {
    lifx_device_t *current = head;
    while (current) {
        if (current->ip.s_addr == ip.s_addr) {
            return current; // Device found
        }
        current = current->next;
    }

    // Device not found, create a new one
    lifx_device_t *new_device = (lifx_device_t *)malloc(sizeof(lifx_device_t));
    if (!new_device) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    new_device->ip = ip;
    new_device->next = head;
    head = new_device;
    return new_device;
}

void update_device_data(lifx_device_t *device, const MessageData *data, MessageType type) {
    if (!device) return;

    // Based on the type of data, update the corresponding field in the device
    switch (type) {
        case TYPE_SERVICE:
            device->data.service = data->service; // Assuming direct assignment is suitable
            break;
        case TYPE_LIGHT_STATE:
            device->data.lightState = data->lightState;
            break;
        // Add other cases as necessary
    }
}

void handle_message(struct in_addr ip, const MessageData *data, MessageType type) {
    lifx_device_t *device = find_or_create_device(ip);
    if (device) {
        update_device_data(device, data, type);
    } else {
        printf("Error: Failed to create or find a device entry\n");
    }
}

void cleanup_devices() {
    lifx_device_t *current = head;
    while (current != NULL) {
        lifx_device_t *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}