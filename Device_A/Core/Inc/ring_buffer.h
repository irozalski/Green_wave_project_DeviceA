/*
 * ring_buffer.h
 *
 *  Created on: 12 paź 2024
 *      Author: Igor
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include "main.h"

#define BUFFER_SIZE 1000  // Size of the ring buffer (must be greater than the message size)

//TRANSMITTER
void buffer_add(uint8_t* data, uint16_t length);
uint8_t buffer_get_chunk(uint8_t* chunk);
int32_t send_message(uint32_t delay_time);

//RECEIVER
void buffer_add_rx(uint8_t* data, uint16_t length);
void buffer_get_full_message(uint8_t* message, uint16_t length);
void flush_rx_buffer();
int32_t receive_message();

#endif /* INC_RING_BUFFER_H_ */
