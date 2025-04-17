//#include <stdio.h>
//#include "pico/stdlib.h"
#include "hardware/spi.h"
//#include "hardware/uart.h"
//#include "hardware/irq.h"
//#include "hardware/dma.h"
#include "NRF24.cpp"
#include "Bob.cpp"

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

int main(){
    stdio_init_all();

    // enable spi0 irq
    // irq_set_enabled(31, true);
    // irq_handler_t irq


    NRF24 nrf(spi0, PIN_CS, PIN_CE);
    nrf.config();
    nrf.modeRX();
    int state = 0;
    // int p = 23;
    // int g = 5;
    // int b = 3;
    // int B = pow(5,3);
    // B = B % 23;
    // int a = 4;
    // int s = pow(5, 4);
    // s = s % 23;
    // int S = pow(s,b);
    // S = S % 23;
    //printf("Shared key: %d\n", S);

    char rx_buffer[32] = {0};
    sleep_ms(16000);
    while(1){
        //if (nrf.newMessage()) {
            nrf.receiveMessage(rx_buffer);
            for (int i = 0; i < 32; i++) {
                printf("%c", rx_buffer[i]);
            }
        //}
        sleep_ms(8000);
    }

    return 0;
}