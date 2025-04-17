#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <iostream>
#include <math.h>
// #include <cstdint>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// byte instances
bool EByte[8] = {0,0,0,0,0,0,0,0};
bool DByte[8] = {0,0,0,0,0,0,0,0};
bool inputByte[8] = {0,0,0,0,0,0,0,0};
bool encryptByte[8] = {0,0,0,0,0,0,0,0};
bool receivedByte[8] = {0,0,0,0,0,0,0,0};
bool decryptByte[8] = {0,0,0,0,0,0,0,0};
bool EseedByte[8] = {0,1,0,1,0,0,1,1}; 
bool DseedByte[8] = {0,1,0,1,0,0,1,1}; 
bool keyByte[8] = {0,0,0,0,0,0,0,0};
bool xorByte[8] = {0,0,0,0,0,0,0,0};
bool xorBit[1] = {0};
bool exorBit[1] = {0};
bool dxorBit[1] = {0};
char Char[16] = {"HELLO BOB"};
int counter = 0;
int stage = 0;

uint64_t prime = 23;
uint64_t generator = 5;
uint64_t TXgenerator = 0;
uint64_t RXgenerator = 0;
uint64_t a = 0;
uint64_t A = 0;
uint64_t B = 0;
uint64_t Key = 0;
uint64_t TXdecimal = 0;
uint64_t RXdecimal = 0;
uint64_t decimal = 0;
uint64_t Edecimal = 0;
int size = 0;
int ASCII = 0;
int binary = 0;
int value = 0;

std::string user_input;

// QRNG 1
uint8_t alice[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0};

// string intances
char decimalString[19] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
char clearString[19] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};

// Make Rx buffer
volatile char rx_buffer[32] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
volatile int rx_index = 0;

uint64_t len = sizeof(alice) / sizeof(alice[0]);  // 64

uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base  % mod; // Ensure base is within mod
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod; // Multiply result by base when exp is odd
        }
        // if (base > 100000000) {
        //      exp *= 2;
        // }  else {
            base = (1ll * base * base) % mod; // Square base
        // }
        exp /= 2; // Divide exponent by 2
        // std::cout << "Base: " << base << ", Exp: " << exp << ", Result: " << result << std::endl;
    }
    return result;
}

void clear_buffer() {
    for (int i = 0; i < sizeof(rx_buffer)-1; i++) {
        rx_buffer[i] = '\0';
    }
}

void convert_tx_to_decimal() {
    TXdecimal = 0;
    for (int i = 0; i < len; i++) {
        uint64_t PV = (uint64_t)1 << i;
        TXdecimal += PV * (alice[len-1-i]);
        // printf("Bit: %d, Power: %llu, Decimal Now: %llu\n", QRNGBytes[len-1-i], PV, decimal);
    }
    a = TXdecimal;
}

void convert_rx_to_decimal() {
    RXdecimal = 0;
    for (int i = 0; i < sizeof(decimalString); i++) {
        decimalString[sizeof(decimalString)-1-i] = rx_buffer[sizeof(decimalString)-1-i] - 48;
        uint64_t PV = 1;
        for (int j = 0; j < i; j++) {
            PV *= 10;
        }
        RXdecimal += PV * (rx_buffer[sizeof(decimalString)-1-i] - 48);
        // printf("Bit: %d, Power: %llu, Decimal Now: %llu\n", decimalString[sizeof(decimalString)-1-i], PV, decimal);
    }
    RXgenerator = RXdecimal;
}

void generate_key1() {
    // printf("prime :%llu\n", prime);
    // printf("generator: %llu\n", generator);
    // printf("length: %llu\n", len);
    // convert_tx_to_decimal();
    a = rand() % 256;
    printf("alice's power: %llu\n", a);
    A = 1;
    for (uint64_t i = 0; i < a; i++) {
        A *= generator;
        while (A > 65535) {
            A -= prime;
        }
        printf("a: %llu\n",i);
        printf("A: %llu\n",A);
    }
    A = A % prime;
}

void generate_key2() {
    convert_rx_to_decimal();
    // printf("generatorB: %llu\n", RXgenerator);
    B = 1;
    for (uint64_t i = 0; i < a; i++) {
        B *= RXgenerator;
        while (B > 65535) {
            B -= prime;
        }
        printf("B^a mod(p): %llu\n",B);
    }
    B = B % prime;
    // printf("B^a mod(p): %llu\n",B);
    Key = B;
    printf("Key is: %llu\n", Key);
}

void clear_string() {
    for (int i = 0; i < sizeof(decimalString)-1; i++) {
        decimalString[i] = '0';
    }
}

void convert_to_char(uint64_t value) {
    if (value < 10) {
        size = 1;
    } else if (value < 100) {
        size = 2;
    } else if (value < 1000) {
        size = 3;
    } else if (value < 10000) {
        size = 4;
    } else if (value < 100000) {
        size = 5;
    } else if (value < 1000000) {
        size = 6;
    } else if (value < 10000000) {
        size = 7;
    } else if (value < 100000000) {
        size = 8;
    } else if (value < 1000000000) {
        size = 9;
    } else if (value < 10000000000) {
        size = 10;
    } else if (value < 100000000000) {
        size = 11;
    } else if (value < 1000000000000) {
        size = 12;
    } else if (value < 10000000000000) {
        size = 13;
    } else if (value < 100000000000000) {
        size = 14;
    } else if (value < 1000000000000000) {
        size = 15;
    } else if (value < 10000000000000000) {
        size = 16;
    } else if (value < 100000000000000000) {
        size = 17;
    } else if (value < 1000000000000000000) {
        size = 18;
    } //else if (value < 10000000000000000000) {
        //size = 19;
    //}
    clear_string();
    for (int i = 0; i < size; i++) {
        decimalString[sizeof(decimalString)-1-i] = value % 10 + 48;
        value -= value % 10;
        value /= 10;
    }
}

void send() {
    uart_puts(UART_ID, decimalString);
    uart_putc(UART_ID, '\n'); 
    printf("Alice Sent: %s\n", decimalString);
    sleep_ms(2000);
}

void uart_rx_interrupt_handler() {
    while (uart_is_readable(UART_ID)) {
        char rx = uart_getc(UART_ID);
        // Find linefeed
        if (rx == '\n') {
            rx_buffer[rx_index] = '\0'; // Null terminate
            rx_index = 0; // Reset index for the next message
        } else if (rx == '\0') {
            
        }
        else if (rx > 47 && rx < 58) {
            rx_buffer[rx_index] = rx;
            rx_index = (rx_index + 1) % (sizeof(rx_buffer)-1);
        }
        else {
            rx_buffer[rx_index] = rx;
            rx_index = (rx_index + 1) % (sizeof(rx_buffer)-1);
        }
    }
}

void enable_uart_rx_interrupt() {
    // Enable UART interrupt
    irq_set_exclusive_handler(UART1_IRQ, uart_rx_interrupt_handler);
    irq_set_enabled(UART1_IRQ, true);
    // Enable RX interrupt
    uart_set_irqs_enabled(UART_ID, true, false);
}

void convert_key_to_binary(uint64_t value) {
    for (int i; i < 8; i++) {
        keyByte[7-i] = value % 2;
        value /= 2;
    }
}

void convert_decimal_string_to_decimal() {
    decimal = 0;
    for (int i = 0; i < sizeof(decimalString); i++) {
        decimalString[sizeof(decimalString)-1-i] = rx_buffer[sizeof(decimalString)-1-i] - 48;
        uint64_t PV = 1;
        for (int j = 0; j < i; j++) {
            PV *= 10;
        }
        decimal += PV * (rx_buffer[sizeof(decimalString)-1-i] - 48);
        // printf("Bit: %d, Power: %llu, Decimal Now: %llu\n", decimalString[sizeof(decimalString)-1-i], PV, decimal);
    }
}

void convert_encrypted_byte_to_decimal() {
    uint64_t PV = 1;
    Edecimal = 0; 
    for (int i = 0; i < 8; i++) {
        PV = 1 << i;
        Edecimal += PV * encryptByte[7-i];
        
    } // printf("decimal: %llu\n", Edecimal);
}

void encrypt() {
        char c = user_input[0];
        ASCII = c;
        value = ASCII;
        for (int i = 7; i >= 0 ; i--){
            inputByte[i] = value % 2;
            value /= 2;
        }
        //print input Byte
        printf("Alice encrypted: %c\n", c);
        // printf("%d\n", ASCII);
        // std::cout << inputByte[0] << std::flush;
        // std::cout << inputByte[1] << std::flush;
        // std::cout << inputByte[2] << std::flush;
        // std::cout << inputByte[3] << std::flush;
        // std::cout << inputByte[4] << std::flush;
        // std::cout << inputByte[5] << std::flush;
        // std::cout << inputByte[6] << std::flush;
        // std::cout << inputByte[7] << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int i = 0; i < 8; i++) {
                xorByte[i] = EseedByte[i] ^ keyByte[i];
            }
            xorBit[0] = xorByte[0] ^ xorByte[1] ^ xorByte[2] ^ xorByte[3] ^ xorByte[4] ^ xorByte[5] ^ xorByte[6] ^ xorByte[7];
            exorBit[0] = EseedByte[7];
            for (int i = 7; i > 0; i--) {
                EseedByte[i] = EseedByte[i-1];
            }
            EseedByte[0] = xorBit[0];
            encryptByte[7-i] = inputByte[7-i] ^ exorBit[0];
            EByte[7-i] = exorBit[0];
        }    
        //print encryption xor Byte
        // std::cout << EByte[0] << std::flush;
        // std::cout << EByte[1] << std::flush;
        // std::cout << EByte[2] << std::flush;
        // std::cout << EByte[3] << std::flush;
        // std::cout << EByte[4] << std::flush;
        // std::cout << EByte[5] << std::flush;
        // std::cout << EByte[6] << std::flush;
        // std::cout << EByte[7] << std::endl;
        //print encrypted Byte
        // std::cout << encryptByte[0] << std::flush;
        // std::cout << encryptByte[1] << std::flush;
        // std::cout << encryptByte[2] << std::flush;
        // std::cout << encryptByte[3] << std::flush;
        // std::cout << encryptByte[4] << std::flush;
        // std::cout << encryptByte[5] << std::flush;
        // std::cout << encryptByte[6] << std::flush;
        // std::cout << encryptByte[7] << std::endl;

        convert_encrypted_byte_to_decimal();
        convert_to_char(Edecimal);
        send();
        counter++;
        sleep_ms(2000);
}

void convert_received_byte_to_binary(uint64_t value) {
    for (int i; i < 8; i++) {
        receivedByte[7-i] = value % 2;
        value /= 2;
    }
}

void decrypt() {  
    binary = 0;
    for(int i = 0; i < 8; i++) {
        binary += receivedByte[7-i] * pow(2,i);
    }
    int value = binary;
    for (int i = 7; i >= 0 ; i--){
        receivedByte[i] = value % 2;
        value /= 2;
    }
        // print encrypted Byte
        // std::cout << binary << std::endl;
        // std::cout << receivedByte[0] << std::flush;
        // std::cout << receivedByte[1] << std::flush;
        // std::cout << receivedByte[2] << std::flush;
        // std::cout << receivedByte[3] << std::flush;
        // std::cout << receivedByte[4] << std::flush;
        // std::cout << receivedByte[5] << std::flush;
        // std::cout << receivedByte[6] << std::flush;
        // std::cout << receivedByte[7] << std::endl;
        for (int i = 0; i < 8; i++) {
            for (int i = 0; i < 8; i++) {
                xorByte[i] = DseedByte[i] ^ keyByte[i];
            }
            xorBit[0] = xorByte[0] ^ xorByte[1] ^ xorByte[2] ^ xorByte[3] ^ xorByte[4] ^ xorByte[5] ^ xorByte[6] ^ xorByte[7];
            dxorBit[0] = DseedByte[7];
            for (int i = 7; i > 0; i--) {
                DseedByte[i] = DseedByte[i-1];
            }
            DseedByte[0] = xorBit[0];
            decryptByte[7-i] = receivedByte[7-i] ^ dxorBit[0];
            DByte[7-i] = dxorBit[0];
        }  
        // print decryption xor Byte  
        // std::cout << DByte[0] << std::flush;
        // std::cout << DByte[1] << std::flush;
        // std::cout << DByte[2] << std::flush;
        // std::cout << DByte[3] << std::flush;
        // std::cout << DByte[4] << std::flush;
        // std::cout << DByte[5] << std::flush;
        // std::cout << DByte[6] << std::flush;
        // std::cout << DByte[7] << std::endl;
        // print decrypted Byte
        // std::cout << decryptByte[0] << std::flush;
        // std::cout << decryptByte[1] << std::flush;
        // std::cout << decryptByte[2] << std::flush;
        // std::cout << decryptByte[3] << std::flush;
        // std::cout << decryptByte[4] << std::flush;
        // std::cout << decryptByte[5] << std::flush;
        // std::cout << decryptByte[6] << std::flush;
        // std::cout << decryptByte[7] << std::endl;
        //convert to char
        binary = 0;
        for(int i = 0; i < 8; i++) {
            binary += decryptByte[7-i] * pow(2,i);
        }
        printf("Alice decrypted: %c\n", binary);

}

// int main()
// {
//     stdio_init_all();
//     sleep_ms(5000);

//     // Set up our UART
//     uart_init(UART_ID, BAUD_RATE);
//     // Set the TX and RX pins by using the function select on the GPIO
//     // Set datasheet for more information on function select
//     gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

//     // clear fifos
//     uart_set_hw_flow(UART_ID, false, false);
//     uart_set_fifo_enabled(UART_ID, false);
//     uart_set_fifo_enabled(UART_ID, true);

//     enable_uart_rx_interrupt();
//     srand(6);

//     sleep_ms(20000);

//     while (true) {
//         switch (stage) {
//           case 0:
//             std::cout << "Enter a word for Alice to encrypt: " << std::flush;
//             std::getline(std::cin, user_input);
//             std::cout << "you entered: " << user_input << std::endl;
//             generate_key1();
//             // printf("g^a mod p: %llu\n", A);
//             convert_to_char(A);
//             // printf("decimal: %s\n", decimalString);
//             send();
//             clear_string();
//             sleep_ms(5000);
//             stage = 1;
//             break;
      
//           case 1:
//             // printf("Alice Received: %s\n", rx_buffer);
//             generate_key2();
//             convert_key_to_binary(Key);
//             printf("KeyByte is: ");
//             std::cout << keyByte[0] << std::flush;
//             std::cout << keyByte[1] << std::flush;
//             std::cout << keyByte[2] << std::flush;
//             std::cout << keyByte[3] << std::flush;
//             std::cout << keyByte[4] << std::flush;
//             std::cout << keyByte[5] << std::flush;
//             std::cout << keyByte[6] << std::flush;
//             std::cout << keyByte[7] << std::endl;
//             clear_buffer();
//             rx_index = 0;  // Reset index
//             sleep_ms(5000);
//             stage = 2;
//             break;
      
//           case 2:
//             irq_set_enabled(UART1_IRQ, false);
//             encrypt();
//             clear_string();
//             sleep_ms(5000);
//             stage = 3;
//             irq_set_enabled(UART1_IRQ, true);
//             break;
      
//           case 3:
//             if (decimalString != clearString) {
//                 printf("Alice Received: %s\n", rx_buffer);
//                 convert_decimal_string_to_decimal();
//                 convert_received_byte_to_binary(decimal);
//                 decrypt();
//                 sleep_ms(5000);
//                 stage = 0;
//             }
//             break;
//         }
//     }
// }
