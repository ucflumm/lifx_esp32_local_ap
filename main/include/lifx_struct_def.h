#ifndef LIFX_STRUCT_DEF_H
#define LIFX_STRUCT_DEF_H
#include <stdint.h>

#define PROTOCOL_NUMBER 1024
#define ADDRESSABLE true
#define TAGGED false
#define LIFX_STATE_SERVICE_MESSAGE_TYPE 3
#define LIFX_PACKET_ACKNOWLEDGEMENT 45
#define LIFX_PACKET_STATE_SERVICE 3
#define LIFX_PACKET_STATE_POWER 22
#define LIFX_PACKET_STATE_LABEL 25
#define LIFX_PACKET_STATE_LOCATION 50
#define LIFX_PACKET_ECHO_RESPONSE 59
#define LIFX_PACKET_STATE_UNHANDLED 223
#define LIFX_PACKET_LIGHT_STATE 107
#define LIFX_PACKET_STATE_LIGHT_POWER 118
#define LIFX_PACKET_STATE_RPOWER 818

typedef enum {
    MSG_SET_COLOR = 102,  // Example message type
    MSG_SET_POWER = 117
} lifx_message_type_t;

// LIFX Frame Header
typedef struct {
    uint16_t size;            // 16-bit size of the entire message
    uint16_t protocol:12;     // 12-bit protocol number (should be 1024 for LIFX)
    uint8_t  addressable:1;   // 1-bit flag for addressable (should be 1)
    uint8_t  tagged:1;        // 1-bit flag for whether the frame is tagged
    uint8_t  origin:2;        // 2-bit origin, must be 0b00
    uint32_t source;          // 32-bit source identifier
} lifx_frame_header_t;

// LIFX Frame Address
typedef struct {
    uint8_t  target[8];       // 64-bit target address (MAC address)
    uint8_t  reserved[6];     // 48-bit reserved
    uint8_t  res_required:1;  // Response required flag
    uint8_t  ack_required:1;  // Acknowledgment required flag
    uint8_t  :6;              // 6 bits reserved
    uint8_t  sequence;        // 8-bit sequence number
} lifx_frame_address_t;

// LIFX Protocol Header
typedef struct {
    uint64_t reserved;        // 64-bit reserved
    uint16_t type;            // 16-bit message type
    uint16_t reserved2;       // 16-bit reserved
} lifx_protocol_header_t;

// Complete LIFX Message
typedef struct {
    lifx_frame_header_t frame;
    lifx_frame_address_t frame_address;
    lifx_protocol_header_t protocol_header;
    uint8_t payload[];  // Variable length payload based on message type
} lifx_message_t;

typedef struct {
    uint16_t level; // 0-65535 (0-100%)
    uint32_t duration;
} lifx_set_power_t;

typedef struct {
    uint8_t service;  // Service type
    uint32_t port;    // Port the service is on
} lifx_payload_state_service_t;

typedef struct lifx_device {
    char ip[16];                // IP address as a string
    uint8_t service;            // Service type
    uint32_t port;              // Port number

    uint16_t power_level;       // Current power level
    char label[32];             // Device label
    uint8_t location[16];       // UUID for location
    char location_label[32];    // Location label
    uint64_t location_updated_at;  // Timestamp for location update

    char echoing[64];           // Echo response data
    uint16_t unhandled_type;    // Type of unhandled packet

    // Light State
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
    uint16_t light_power;
    char light_label[32];

    // Relay Power (for switches)
    uint16_t relay_powers[4];   // Array to hold power states for multiple relays

    struct lifx_device *next;   // Pointer to the next node
} lifx_device_t;

typedef struct {
    uint16_t level;  // Power level
} lifx_payload_state_power_t;

typedef struct {
    char label[32];  // Device label
} lifx_payload_state_label_t;

typedef struct {
    uint8_t location[16];  // UUID for location
    char label[32];        // Location label
    uint64_t updated_at;   // Timestamp
} lifx_payload_state_location_t;

typedef struct {
    uint8_t echoing[64];
} lifx_payload_echo_response_t;

typedef struct {
    uint16_t unhandled_type;
} lifx_payload_state_unhandled_t;

typedef struct {
    uint16_t hue;
    uint16_t saturation;
    uint16_t brightness;
    uint16_t kelvin;
    uint16_t power;
    char label[32];
    uint8_t reserved[8];
} lifx_payload_light_state_t;

typedef struct {
    uint16_t level;
} lifx_payload_state_light_power_t;

typedef struct {
    uint8_t relay_index;
    uint16_t level;
} lifx_payload_state_rpower_t;

typedef struct {
    uint16_t level;    // Power level (0 for off, 65535 for on)
    uint32_t duration; // Duration for power level transition in milliseconds
} lifx_payload_set_power_t;

#endif