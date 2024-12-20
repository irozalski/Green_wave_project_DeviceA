/*
 * ring_buffer.c
 *
 *  Created on: 12 paź 2024
 *      Author: Igor
 */
#include "ring_buffer.h"
#include "nRF24_Defs.h"
#include "nRF24.h"
#include "rsa_driver.h"
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

int32_t send_message(uint32_t delay_time){
	if(tx_size == 0){
			return 1;
		}
	if (tx_size > 0 && HAL_GetTick() - PackageTimer > delay_time) {
		if (buffer_get_chunk(chunk)) {
		    nRF24_WriteTXPayload(chunk);  // Send 32-byte chunk
		    nRF24_WaitTX();               // Wait until the transmission is completed
		    }
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // Toggle LED to indicate transmission
		PackageTimer = HAL_GetTick();
	}
	return 0;
}

//---------------------------------------------------------RECEIVER----------------------------------------------------------------//
uint16_t expected_message_size = 256;


uint8_t rx_ring_buffer[BUFFER_SIZE];  // Ring buffer to store received data
uint16_t rx_head = 0, rx_tail = 0;    // Head and tail for the ring buffer
//uint8_t chunk[NRF24_PAYLOAD_SIZE];    // Buffer to hold each 32-byte chunk

// Add received chunk to the ring buffer
void buffer_add_rx(uint8_t* data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        rx_ring_buffer[rx_head] = data[i];
        rx_head = (rx_head + 1) % BUFFER_SIZE;
    }
}

// Extract full message from the ring buffer
void buffer_get_full_message(uint8_t* message, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        message[i] = rx_ring_buffer[rx_tail];
        rx_tail = (rx_tail + 1) % BUFFER_SIZE;
    }
}

// Function to flush the transmission ring buffer
void flush_rx_buffer() {
    // Reset the head, tail, and size to flush the buffer
    rx_head = 0;
    rx_tail = 0;
    //rx_size = 0;

    // Optionally clear the buffer content
    memset(rx_ring_buffer, 0, sizeof(rx_ring_buffer));
}

int32_t receive_message(){
	if (nRF24_RXAvailible()) {
		    nRF24_ReadRXPaylaod(chunk);  // Receive 32-byte chunk
		    buffer_add_rx(chunk, NRF24_PAYLOAD_SIZE);  // Store received chunk in the buffer
		    //MessageLength = sprintf(Message, "%s\n\r", chunk);
		    //HAL_UART_Transmit(&huart1, Message, MessageLength, 1000);

		    // Optionally, you can check if the entire message is received and process it
		    if (rx_head >= expected_message_size) {
		    uint8_t received_message[expected_message_size];
		    buffer_get_full_message(received_message, expected_message_size);  // Extract full message
//		    HAL_UART_Transmit(&huart1, received_message, expected_message_size, 1000);  // Transmit message via UART
//
		    Decrypt_RSA_Message_And_Add_It(0, received_message);

		     flush_rx_buffer();
		     return 1;
		     }

		     HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);  // Toggle LED to indicate reception
		}
	return 0;
}

//--------------------------------------------------------------||----------------------------------------------------------------//
