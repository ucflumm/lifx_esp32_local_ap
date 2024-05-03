#include <stdlib.h>
#include <stdint.h>
#include "include/lifx_struct_def.h"
#include "include/main.h"

lifx_device_t *add_or_update_device(lifx_device_t **head, const char *ip, uint8_t service, uint32_t port,
                                    const char *label, const uint8_t *location, const char *location_label,
                                    uint64_t location_updated_at, const char *echoing, uint16_t unhandled_type,
                                    uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin,
                                    uint16_t light_power, const char *light_label, const uint16_t *relay_powers,
                                    size_t relay_count) {
    if (!ip) {
        ESP_LOGE("add_update_device", "Invalid IP address provided.");
        return NULL;
    }

    lifx_device_t *device = find_device_by_ip(*head, ip);
    if (!device) {
        device = (lifx_device_t *)malloc(sizeof(lifx_device_t));
        if (!device) {
            perror("Failed to allocate memory for new device");
            return NULL;
        }
        memset(device, 0, sizeof(lifx_device_t)); // Safe practice to initialize memory
        strcpy(device->ip, ip);
        device->next = *head;
        *head = device;
    }

    // Updating fields, checking for NULL pointers where necessary
    device->service = service;
    device->port = port;

    if (label) {
        strcpy(device->label, label);
    } else {
        device->label[0] = '\0'; // Set empty string if NULL
    }

    if (location) {
        memcpy(device->location, location, 16);
    } else {
        memset(device->location, 0, 16); // Set default value if NULL
    }

    if (location_label) {
        strcpy(device->location_label, location_label);
    } else {
        device->location_label[0] = '\0'; // Set empty string if NULL
    }

    device->location_updated_at = location_updated_at;

    if (echoing) {
        strcpy(device->echoing, echoing);
    } else {
        device->echoing[0] = '\0'; // Set empty string if NULL
    }

    device->unhandled_type = unhandled_type;

    device->hue = hue;
    device->saturation = saturation;
    device->brightness = brightness;
    device->kelvin = kelvin;
    device->light_power = light_power;

    if (light_label) {
        strcpy(device->light_label, light_label);
    } else {
        device->light_label[0] = '\0'; // Set empty string if NULL
    }

    if (relay_powers && relay_count > 0) {
        memcpy(device->relay_powers, relay_powers, sizeof(uint16_t) * relay_count);
    } else {
        memset(device->relay_powers, 0, sizeof(uint16_t) * 4); // Set all to zero if NULL or count is zero
    }

    return device;
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

lifx_device_t *find_device_by_ip(lifx_device_t *head, const char *ip) {
    lifx_device_t *current = head;
    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void cleanup_devices(lifx_device_t *head) {
    lifx_device_t *current = head;
    while (current != NULL) {
        lifx_device_t *next = current->next;
        free(current);
        current = next;
    }
}