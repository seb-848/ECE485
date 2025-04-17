#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <iostream>
#include <math.h>
//#include <cstdint>
//#include <uart.h>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Hacking defines
#include <cmath>
#include <vector>
#include <bitset>
#include <pstl/execution_defs.h>

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
char Char[16] = {"hello alice"};
bool hackingAlgoArr[8] = {0,0,0,0,0,0,0,0};
int counter = 0;
int stage = 0;

uint64_t prime = 23;
uint64_t generator = 5;
uint64_t TXgenerator = 0;
uint64_t RXgenerator = 0;
uint64_t b = 0;
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

// QRNG 2
uint8_t bob[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1};

// string intances
char decimalString[19] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
char clearString[19] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};

// Make Rx buffer
volatile char rx_buffer[32] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
volatile int rx_index = 0;

uint64_t len = sizeof(bob) / sizeof(bob[0]);  // 64

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
        TXdecimal += PV * (bob[len-1-i]);
        // printf("Bit: %d, Power: %u, Decimal Now: %u\n", QRNGBytes[len-1-i], PV, decimal);
    }
    b = TXdecimal;
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
    b = rand() % 256;
    printf("bob's power: %llu\n", b);
    B = 1;
    for (uint64_t i = 0; i < b; i++) {
        B *= generator;
        while (B > 65535) {
            B -= prime;
        }
        printf("b: %llu\n",i);
        printf("B: %llu\n",B);
    }
    B = B % prime;
}

void generate_key2() {
    convert_rx_to_decimal();
    // printf("generatorA: %llu\n", RXgenerator);
    A = 1;
    for (uint64_t i = 0; i < b; i++) {
        A *= RXgenerator;
        while (A > 65535) {
            A -= prime;
        }
        printf("A^b mod(p): %llu\n", A);
    }
    A = A % prime;
    // printf("A^b mod(p): %llu\n", A);
    Key = A;
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
    //     size = 19;
    // }
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
    printf("Bob Sent: %s\n", decimalString);
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
    //uart_set_irqs_enabled
}

void convert_key_to_binary(uint64_t value) {
    for (int i; i < 8; i++) {
        keyByte[7-i] = value % 2;
        value /= 2;
    }
}

// for hacking algo below
void convert_key_to_binary_hacking(uint64_t value) {
    for (int i; i < 8; i++) {
        hackingAlgoArr[7-i] = value % 2;
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
        printf("Bob encrypted: %c\n", c);
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
        printf("Bob decrypted: %c\n", binary);

}

//Hacking Code -------------------------------------------------------------------
std::vector<std::string> generateParityBitSequences(int seedLength);
std::vector<std::vector<bool>> splitIntoBytes(const std::vector<bool>& bitSequence, size_t offset);
int binaryToDecimal(const std::vector<int>& binary);
void initializeMatrix(const std::vector<int> &binarySeq, int length, const std::vector<std::string> &PSeq,
                      std::vector<bool> data, std::vector<bool> &final, int u, std::vector<std::vector<char>>& character);
void printSelectedRows(const std::vector<std::vector<char>>& text, const std::vector<bool>& PB_checker);


// Function Definitions

std::vector<std::string> generateParityBitSequences(int seedLength) {
    std::vector<std::string> sequences; // Stores the generated sequences

    if (seedLength <= 0) {
        std::cerr << "Error: Seed length must be at least 1.\n";
        return sequences; // Return empty vector
    }
    int totalSequences = (1 << seedLength) - 1; // 2^n - 1 (exclude 0)
    for (int i = 1; i <= totalSequences; ++i) {
        std::string binaryString = std::bitset<32>(i).to_string().substr(32 - seedLength);
        sequences.push_back(binaryString); // Save the sequence
    }
    return sequences;
}

std::vector<std::vector<bool>> splitIntoBytes(const std::vector<bool>& bitSequence, size_t offset) {
    std::vector<std::vector<bool>> result;

    // Ensure offset is within bounds
    if (offset >= bitSequence.size()) {
        return result; // Return empty if offset is out of range
    }

    // Start from the offset and process in chunks of 8
    for (size_t i = offset; i + 8 <= bitSequence.size(); i += 8) {
        result.emplace_back(bitSequence.begin() + i, bitSequence.begin() + i + 8);
    }

    return result;
}

int binaryToDecimal(const std::vector<int>& binary) {
    int decimal = 0;
    int n = 8;
    for (int i = 0; i < n; ++i) {
        decimal += binary[i] * std::pow(2, n - 1 - i);
    }
    return decimal;
}

void initializeMatrix(const std::vector<int> &binarySeq, int length, const std::vector<std::string> &PSeq,
    std::vector<bool> data, std::vector<bool> &final, int u, std::vector<std::vector<char>>& character) {
    
    int rows = length + 8;
    int cols = length + 1;
    std::vector<int> P(length);
    std::vector<int> Bn(length);
    std::vector<int> decrypted_byte(7);
    int B_n;
    std::vector<int> Xn(length);
    int X_n;


    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols, 0));

    // Fill in the template matrix with seed bits
    for (int j = 0; j < length; ++j) {
        for (int i = 0; i < length - j; ++i) {
            matrix[i][j] = binarySeq[length - 1 - i - j];  // Corrected reverse order
        }
    }


    // For statement to go through parity bit sequence
    for (int m =0 ; m < (pow(2, length)-1) ; ++m) {
        if (final[m] == 1) {
            for (int b =0 ; b < length ; ++b) {
                unsigned num = PSeq[m][b];
                P[b] = num - 48;
            }

            for (int i = 0 ; i < (length + 7); ++i) {

                //Error withing this section
                for (int d = 0 ; d < length ; ++d) {
                    std::cout << i;
                    Bn[d] = (matrix[i + length - 2][length - 1 - d]) * P[d];
                    if (d == 0) {
                        B_n = Bn[d];
                    }
                    if (d != 0) {
                        B_n = B_n ^ Bn[d];
                    }
}
for (int j = 0; j < length; ++j) {
  if (length + i + j - 1 < length + 8) {
      matrix[length + i + j - 1][length - 1 - j] = B_n;
  }
}
if (i > length-2) {
  for (int d = 0 ; d < length ; ++d) {
      Xn[d] = (matrix[i + length - 2][length - 1 - d]) * P[length - 1 - d];
      if (d == 0) {
          X_n = Xn[d];
      }
      if (d != 0) {
          X_n = X_n ^ Xn[d];
      }
  }
  X_n = X_n ^ data[i-1];
  matrix[i+1][length] = X_n;
}
}
//End error section

// Display the matrix
std::cout << "Initialized " << rows << "x" << cols << " matrix:\n";
for (const auto& row : matrix) {
for (int val : row) {
  std::cout << val << " ";
}
std::cout << "\n";
}
std::cout << "Decrypted Byte: ";
for (int h=0; h<8; ++h) {
decrypted_byte[h] = matrix[length + h][length];
std::cout << decrypted_byte[h];
}
std::cout << "\n";
int decimal_Value = binaryToDecimal(decrypted_byte);
std::cout << "Decimal Value: " << decimal_Value << std::endl;
if (decimal_Value >= 32 && decimal_Value <= 127) {
char asciiChar = static_cast<char>(decimal_Value);
std::cout << "The ASCII character for " << decimal_Value << " is: " << asciiChar << std::endl;
character[m][u] = asciiChar;
} else {
std::cout << "Invalid ASCII value." << std::endl;
final[m] = false;
}
std::cout << "\n \n";
}
}
}

void printSelectedRows(const std::vector<std::vector<char>>& text, const std::vector<bool>& PB_checker) {
    for (size_t row = 0; row < text.size(); ++row) {
        if (row < PB_checker.size() && PB_checker[row] == 1) {  // Ensure row index is within 'result'
            std::cout << "Possible Decrypted Text: ";
            for (char c : text[row]) {
                std::cout << c;
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
}

//_____________________________________________________________________________
// int main()
// {
//     //hacking --------------------------------------------------------------
//     std::string user_input;
//     std::vector<int> seed_vec;
//     seed_vec.push_back(0);//1st
//     seed_vec.push_back(1);//2nd
//     seed_vec.push_back(0);//3rd
//     int count = 0;
//     //std::cout << "Please enter the seed bit sequence: ";
//     //std::cin >> user_input;
//     //----------------------------------------------------------------------

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
//     srand(15);

//     sleep_ms(15000);

//     while (true) {
//         switch (stage) {
//           case 0:
//             std::cout << "Enter a word for Alice to encrypt: " << std::flush;
//             std::getline(std::cin, user_input);
//             std::cout << "you entered: " << user_input << std::endl;
//             generate_key1();
//             // printf("g^b mod p: %llu\n", B);
//             convert_to_char(B);
//             // printf("decimal: %s\n", decimalString);
//             send();
//             clear_string();
//             sleep_ms(5000);
//             stage = 1;
//             break;
        
//           case 1:
//             // printf("Bob Received: %s\n", rx_buffer);
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
//                 printf("Bob Received: %s\n", rx_buffer);
//                 convert_decimal_string_to_decimal();
//                 convert_received_byte_to_binary(decimal);
//                 //---------------------------------------
//                 // function for ascii to binary here
//                 // push to back of vector
//                 convert_key_to_binary_hacking(decimal);
//                 for (int i = 0; i < 8; i++) {
//                     user_input.push_back(hackingAlgoArr[i]);
//                 }
//                 count++;
//                 //---------------------------------------
//                 decrypt();
//                 sleep_ms(5000);
//                 if (count >= 5) {
//                     stage = 4;
//                 }
//                 else {
//                     stage = 0;
//                 }
                
//             }
//             break;

//           case 4:
//           // hacking state----------------------------------------------
//             // for (int i =0; i<user_input.size();i++) {
//             //     printf ("%d", user_input.at(i));
//             // }

//                 for (int num : seed_vec) {
//                     std::cout << num << " ";
//                 }
//                 std::cout << "}\n";
//                 std::vector<bool> bitSequence;

//                 // fills bit sequence vector
//                 for (int i = 0; i < user_input.size(); i++) {
//                     bitSequence.push_back(user_input.at(i));
//                 }

//     // End replacement section

//     // Splitting decrypted bytes into 8 after given offset
//     size_t offset = 0;
//     auto result = splitIntoBytes(bitSequence, offset);

//     // Determines seed length
//     int seed_length = seed_vec.size();
//     std::cout << "The length is: " << seed_length << std::endl;
//     std::cout << "The number of parity bit sequences is: " << pow(2, seed_length)-1 << std::endl;

//     // Generating parity bit sequences
//     std::vector<std::string> sequences = generateParityBitSequences(seed_length);

//     // Printing saved parity bit sequences
//     std::cout << "Generated sequences:\n";
//     for (const auto& seq : sequences) {
//         std::cout << seq << std::endl;
//     }

//     std::vector<std::vector<char>> characters((pow(2, seed_length)-1), std::vector<char>(result.size(), 'X'));
//     std::vector<bool> final((pow(2, seed_length)-1), true);
//     for (int u = 0; u < result.size(); ++u) {
//         // Creates matrix templates, find unencrypted bit sequence
//         initializeMatrix(seed_vec, seed_length, sequences, result[u],final, u, characters);
//     }

//     printSelectedRows(characters, final);

//             stage = 5;
//             // end hacking state and send to non existent state---------------------------
//             break;
//         }
//     }
// }