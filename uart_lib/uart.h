/*
 * uart.h
 *
 * Created: 9.01.2021 15:26:29
 *  Author: haluk
 */ 
#ifndef UART_H_
#define UART_H_


//#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/////////////////////////////////
#define UART_Rx_Boyut 128
#define UART_Tx_Boyut 128
#define UART_Rx_Mask (UART_Rx_Boyut-1)
#define UART_Tx_Mask (UART_Tx_Boyut-1)
#define UART_Bos_On UCSR0B|=(1<<UDRIE0)// uart UDR register boþ kesmesi
#define UART_Bos_Off UCSR0B&=~(1<<UDRIE0)// uart UDR register boþ kesmesi
////////////////////////////////////////////////////
void uart_basla(uint32_t baud);
uint8_t uart_oku();
void uart_gonder(uint8_t uData);
void uart_dizi(const char *str);
uint8_t uart_gelen();
void uart_dizi_al( char *stri);

#endif