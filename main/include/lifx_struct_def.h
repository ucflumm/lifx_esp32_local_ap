#ifndef LIFX_STRUCT_DEF_H
#define LIFX_STRUCT_DEF_H
#include <stdint.h>

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
    lifx_frame_header_t     frame;
    lifx_frame_address_t    frame_address;
    lifx_protocol_header_t  protocol_header;
    char                    payload[];  // Variable-length payload
} lifx_message_t;

typedef struct {
    uint8_t service;
    uint16_t port;
} lifx_state_service_t;

typedef struct {
    uint16_t level;      // 0 for off, 65535 for on
    uint32_t duration;   // Duration to perform the power transition in milliseconds
} lifx_set_power_payload_t;

typedef struct {
    lifx_frame_header_t header;
    uint8_t target[8];       // Target device address
    uint8_t reserved[6];     // Reserved bytes set to zero
    uint8_t res_required;    // 1 if response is required
    uint8_t ack_required;    // 1 if acknowledgment is required
    uint8_t sequence;        // Sequence number to match request with response
    lifx_set_power_payload_t payload;
} lifx_set_power_message_t;

#endif