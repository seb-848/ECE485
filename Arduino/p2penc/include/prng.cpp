//
// Created by sebas on 1/29/2025.
//

// static bit and seed
int const BITS = 8;
//int const seedB = 0b10101111;

void textToBinChar(char c, char (&arr)[8])
{
    for (int i = 7; i >= 0; i--) arr[i] = ((c >> i) & 1) ? '1': '0';
}

void textToBinInt(char c, char (&arr)[8])
{
    for (int i = 7; i >= 0; --i) arr[i] = (c & (1 << i)) ? '1': '0';
}

/**
 * takes the B and P to and then xor together for B n-1
 * @param b[] - array of seed
 * @param p[] - array of parity
 * @return result xor value
 */
int parityXOR(int (&b)[BITS], int (&p)[BITS]) {
    int a[BITS];

    for(int i = 0; i < BITS; i++) {
        a[i] = b[i] & p[i];
    }
    int result = a[0];
    for (int i = 1; i < BITS; i++) {
        result ^= a[i];
    }
    return result;
}

/**
 * shifts array to the right
 * @param b array of B bits being shifted
 */
void rotate(int (&b)[BITS]) {
    for (int i = BITS - 1; i > 0; i++) {
        b[i] = b[i - 1];
    }
}

void rotateRight(int shifts, char (&b)[BITS]) {
    for (int i = 0; i < shifts; i++) {
        int temp = b[BITS - 1];
        for (int j = (BITS - 2); j >= 0; j--) {
            b[j + 1] = b[j];
        }
        b[0] = temp;
    }
}

/**
 * shifts the array to the right and adds new value from xor
 * use after encrypting bit
 * @param b array of B bits
 * @param newValue xor value
 * @return new array
 */
void shiftRegister(int (&b)[BITS], int (&p)[BITS]) {
    int xorValue = parityXOR(b, p);
    rotate(b);
    b[0] = xorValue;
}

/**
 * take the last bit in b and xor with the info to be encrypted
 * use before shifting
 * @param b array of B
 * @param infoToEncrypt info to be encrypted
 * @return encryptedValue
 */
int encryption(int (&b)[BITS], int infoToEncrypt) {
    int encryptionValue = b[BITS - 1];
    int encryptedValue = infoToEncrypt ^ encryptionValue;
    return encryptedValue;
}