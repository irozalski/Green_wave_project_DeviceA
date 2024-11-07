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

void buffer_add(uint8_t* data, uint16_t length);
uint8_t buffer_get_chunk(uint8_t* chunk);
void send_message(uint32_t delay_time);
#endif /* INC_RING_BUFFER_H_ */
