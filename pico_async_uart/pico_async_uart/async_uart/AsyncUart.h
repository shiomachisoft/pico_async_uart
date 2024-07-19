#ifndef ASUART_H
#define ASUART_H

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/uart.h"

#include "Type.h"
#include "Common.h"
#include "Timer.h"

// [define]
// [UART Config]
#define ASUART_ID  uart0      // UARTのID: uart0 or uart1
#define ASUART_IRQ UART0_IRQ  // UARTのIRQ: UART0_IRQ or UART1_IRQ
#define ASUART_TX  0          // GP0: GP number of UART TX pin
#define ASUART_RX  1          // GP1: GP number of UART RX pin

#define ASUART_BAUD_RATE 9600             // baud rate
#define ASUART_DATA_BITS 8                // data bits
#define ASUART_STOP_BITS 1                // stop bits
#define ASUART_PARITY    UART_PARITY_NONE // parity: UART_PARITY_NONE / UART_PARITY_EVEN / UART_PARITY_ODD

// Any uart received data during this time from power-on will be discarded.
#define ASUART_STABILIZATION_WAIT_TIME 50 // ms

// UART transmit/receive queue size(byte)
#define ASUART_QUE_DATA_MAX_UART 1024

// [Function Prototype Declaration]
int ASUART_SendBuf(PVOID pBuf, int size);
int ASUART_RecvBuf(PVOID pBuf, int size);
ULONG ASUART_GetLastErrorBits();
void ASUART_ClearLastErrorBits();
void ASUART_Main();
void ASUART_Init();

#endif

