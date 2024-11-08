/*
 * ring_buffer.c
 *
 *  Created on: 12 paź 2024
 *      Author: Igor
 */
#include "ring_buffer.h"
#include "nRF24_Defs.h"
#include "nRF24.h"
//#include "stdint.h"

uint8_t tx_ring_buffer[BUFFER_SIZE];  // Ring buffer to store the large message
uint16_t tx_head = 0, tx_tail = 0;    // Head and tail for the ring buffer
uint32_t tx_size = 0;                 // Size of the message to transmit
uint8_t chunk[NRF24_PAYLOAD_SIZE];    // Buffer to hold each 32-byte chunk
uint32_t PackageTimer;

//TRANSMISSION
// Add data to the ring buffer
void buffer_add(uint8_t* data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        tx_ring_buffer[tx_head] = data[i];
        tx_head = (tx_head + 1) % BUFFER_SIZE;
        tx_size++;
    }
}

// Get 32-byte chunk from the ring buffer
uint8_t buffer_get_chunk(uint8_t* chunk) {
    if (tx_size == 0) return 0;  // No data to send

    for (uint8_t i = 0; i < NRF24_PAYLOAD_SIZE && tx_size > 0; i++) {
        chunk[i] = tx_ring_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % BUFFER_SIZE;
        tx_size--;
    }
    return 1;  // Chunk is ready to send
}

void send_message(uint32_t delay_time){
	if (tx_size > 0 && HAL_GetTick() - PackageTimer > delay_time) {
		if (buffer_get_chunk(chunk)) {
		    nRF24_WriteTXPayload(chunk);  // Send 32-byte chunk
		    nRF24_WaitTX();               // Wait until the transmission is completed
		    }
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // Toggle LED to indicate transmission
		PackageTimer = HAL_GetTick();
	}
}

