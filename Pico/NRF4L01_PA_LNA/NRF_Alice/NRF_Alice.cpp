#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "NRF24.cpp"
#include "Alice.cpp"
#include "hardware/irq.h"
#include <cstring>
#include <iostream>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 4
#define PIN_CS   14
#define PIN_SCK  6
#define PIN_MOSI 7
#define PIN_CE   17

#define INPUT 0
#define SEND  1
#define WAIT  2
#define DH_1  3
#define DH_2  4

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

int power(int base, unsigned int exp) {
    int i, result = 1;
    for (i = 0; i < exp; i++)
        result *= base;
    return result;
 }

int main(){
    stdio_init_all();

    NRF24 nrf(spi0, PIN_CS, PIN_CE);
    nrf.config();
    nrf.modeTX();
    // int p = 23;
    // int g = 5;
    // int a = 4;
    // int A = pow(5, 4);
    // A = A % 23;

    // int b = 3;
    // int s = pow(5,3);
    // s = s % 23;
    // int S = pow(s,a);
    // S = S % 23;

    char tx_buffer[32] = "A: ";
    char rx_buffer[32];
    char c;
    tx_buffer[31] = '\n';

    int count = 3;
    int state = 0;
    int longer_mess_flag = 0;

    sleep_ms(15000);
    printf("start\n");
    sleep_ms(1000);
    //printf("Shared key: %d\n", S);

    while(1){
        switch (state) {//TODO: add diffie hellman
            case INPUT:
            c = getchar();
            // if the message is longer than 32 characters
            // the program raises a flag and sends the first portion
            // that fits and loops back to the INPUT case etc
            if (count == 31 && c != '\n' && c!= '\r') {
                longer_mess_flag = 1;
                state = SEND;
                break;
            }
            if (c == '\n' || c == '\r') {
                fflush(stdin);
                state = SEND;
            }
            else {
                tx_buffer[count] = c;
                count++;
            }
            break;

            case SEND:
            // message is sent to reciever and printed on this side
            nrf.sendMessage(tx_buffer);
            sleep_ms(100);
            for (int i = 0; i < 32; i++) {
                printf("%c", tx_buffer[i]);
            }

            // clear buffer
            for (int i = 3; i < 31; i++) tx_buffer[i] = 0;
            tx_buffer[31] = '\n';//TODO: potentially take out
            state = WAIT;
            break;

            case WAIT:
            count = 3;
            state = INPUT;
            sleep_ms(3000);
            break;
        }
    }

    return 0;
}