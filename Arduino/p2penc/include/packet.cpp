#include <Vector.h>
#include <prng.cpp>
//#include <encryption.h>
#define TEXT_SIZE 10

struct packet {
    char encryptedText[BITS];
    int sequence[BITS];
    char text[TEXT_SIZE];
};

void makeIntoPacket(char arr[TEXT_SIZE], packet &user) {
    int count = 0;
    char c[4];

    // fill text
    // for (int i = 0 ; i < TEXT_SIZE; i++) {
    //     user.text[i] = arr[i];
    // }
    
    // fill encrypted array with binary
    for (int i = 0; i < TEXT_SIZE; i++) {
        itoa(user.text[i], c, 2);
        for (int j = 0; j < 4; j++) {
            user.encryptedText[count] = c[j] - '0';
            count++;
        }
    }
}

void encrypt(int (&b)[BITS], int (&p)[BITS], packet &user) {
    for (int i = 0; i < TEXT_SIZE * 4; i++) {
      user.encryptedText[i] ^= b[BITS - 1];
      user.sequence[i] = b[BITS - 1];
      shiftRegister(b, p);
    }
}

void decToBinaryPrint(int n)
{
    // Size of an integer is assumed to be 32 bits
    for (int i = 7; i >= 0; i--) {
        int k = n >> i; // right shift
        if (k & 1) // helps us know the state of first bit
              Serial.print("1");
        else Serial.print("0");
    }
    Serial.println();
}

void decToBinary(int n, int(&arr)[8])
{
    // Size of an integer is assumed to be 32 bits
    for (int i = 7; i >= 0; i--) {
        int k = n >> i; // right shift
        if (k & 1) arr[i] = 1;
        else arr[i] = 0;
    }
    //Serial.println();
}

int readBinaryString(char *s) {
  int result = 0;
  while(*s) {
    result <<= 1;
    if(*s++ == '1') result |= 1;
  }
  return result;
}