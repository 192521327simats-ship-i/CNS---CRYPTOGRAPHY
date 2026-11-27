/*
 * Q22. Write a C program to encrypt and decrypt in cipher block chaining
 *      (CBC) mode using S-DES (affine, Hill, or DES are also allowed).
 *      Test data: IV = 1010 1010, plaintext = 0000 0001 0010 0011,
 *      key = 01111 11101. The expected ciphertext in the question is
 *      1111 0100 0000 1011. Decryption must reverse it.
 *
 * CBC: C1 = E(P1 XOR IV), C2 = E(P2 XOR C1). Decrypt: P1 = D(C1) XOR IV,
 * P2 = D(C2) XOR C1.
 *
 * Compile: gcc -Wall -o q22 q22_cbc_sdes.c
 */
#include <stdio.h>
#include "sdes.h"

static void print_bin8(unsigned v) {
    for (int i = 7; i >= 0; i--) {
        printf("%u", (v >> i) & 1u);
        if (i == 4) printf(" ");
    }
}

int main(void) {
    unsigned key = 0x1FD;                      /* 01111 11101 */
    unsigned iv  = 0xAA;                       /* 1010 1010 */
    unsigned plain[2] = {0x01, 0x23};          /* 0000 0001 | 0010 0011 */
    unsigned cipher[2], back[2];

    /* Encrypt */
    unsigned prev = iv;
    for (int i = 0; i < 2; i++) {
        cipher[i] = sd_block(plain[i] ^ prev, key, 0);
        prev = cipher[i];
    }

    /* Decrypt */
    prev = iv;
    for (int i = 0; i < 2; i++) {
        back[i] = sd_block(cipher[i], key, 1) ^ prev;
        prev = cipher[i];
    }

    printf("Plaintext : "); print_bin8(plain[0]); printf(" | "); print_bin8(plain[1]); printf("\n");
    printf("Ciphertext: "); print_bin8(cipher[0]); printf(" | "); print_bin8(cipher[1]); printf("\n");
    printf("Expected  : 1111 0100 | 0000 1011\n");
    printf("Decrypted : "); print_bin8(back[0]); printf(" | "); print_bin8(back[1]); printf("\n");
    printf("Round trip: %s\n", (back[0] == plain[0] && back[1] == plain[1]) ? "ok" : "FAILED");
    return 0;
}
