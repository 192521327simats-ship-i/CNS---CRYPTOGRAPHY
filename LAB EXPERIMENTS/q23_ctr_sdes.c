/*
 * Q23. Write a C program to encrypt and decrypt in counter mode using S-DES
 *      (affine or Hill are also allowed). Test data: counter starts at
 *      0000 0000, plaintext = 0000 0001 0000 0010 0000 0100, key = 01111 11101.
 *      Expected ciphertext from the question: 0011 1000 0100 1111 0011 0010.
 *
 * Counter mode: C_i = P_i XOR E_K(counter_i), with counter_i = counter_0 + i.
 * Decryption is the same operation, since XOR is its own inverse.
 *
 * Compile: gcc -Wall -o q23 q23_ctr_sdes.c
 */
#include <stdio.h>
#include "sdes.h"

static void print_bin8(unsigned v) {
    for (int i = 7; i >= 0; i--) {
        printf("%u", (v >> i) & 1u);
        if (i == 4) printf(" ");
    }
}

/* Applies the counter-mode keystream to data (encrypts or decrypts). */
static void ctr_crypt(const unsigned *in, unsigned *out, int n,
                      unsigned counter0, unsigned key) {
    for (int i = 0; i < n; i++) {
        unsigned ks = sd_block((counter0 + (unsigned)i) & 0xFFu, key, 0);
        out[i] = in[i] ^ ks;
    }
}

int main(void) {
    unsigned key = 0x1FD;                       /* 01111 11101 */
    unsigned counter0 = 0x00;                   /* 0000 0000 */
    unsigned plain[3] = {0x01, 0x02, 0x04};     /* 0000 0001 | 0000 0010 | 0000 0100 */
    unsigned cipher[3], back[3];

    ctr_crypt(plain, cipher, 3, counter0, key);
    ctr_crypt(cipher, back, 3, counter0, key);

    printf("Plaintext : "); for (int i = 0; i < 3; i++) { print_bin8(plain[i]); printf(" "); }
    printf("\nCiphertext: "); for (int i = 0; i < 3; i++) { print_bin8(cipher[i]); printf(" "); }
    printf("\nExpected  : 0011 1000 0100 1111 0011 0010\n");
    printf("Decrypted : "); for (int i = 0; i < 3; i++) { print_bin8(back[i]); printf(" "); }
    printf("\nRound trip: %s\n",
           (back[0] == plain[0] && back[1] == plain[1] && back[2] == plain[2]) ? "ok" : "FAILED");
    return 0;
}
