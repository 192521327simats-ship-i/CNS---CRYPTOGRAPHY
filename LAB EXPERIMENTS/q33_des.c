/*
 * Q33. Implement the Data Encryption Standard (DES). DES encrypts 64-bit
 *      blocks with a 56-bit key (64 bits with parity). The same algorithm and
 *      key are used for encryption and decryption, with the subkeys in reverse
 *      order for decryption.
 *
 * Usage:
 *   ./q33                       runs the standard test vector
 *   ./q33 KEY_HEX PLAIN_HEX     encrypts PLAIN with KEY and decrypts again
 *
 * Standard test vector (FIPS / Stallings):
 *   key        133457799BBCDFF1
 *   plaintext  0123456789ABCDEF
 *   ciphertext 85E813540F0AB405
 *
 * Compile: gcc -Wall -o q33 q33_des.c
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "des.h"

int main(int argc, char *argv[]) {
    uint64_t key   = 0x133457799BBCDFF1ULL;
    uint64_t plain = 0x0123456789ABCDEFULL;

    if (argc == 3) {
        key   = strtoull(argv[1], NULL, 16);
        plain = strtoull(argv[2], NULL, 16);
    }

    uint64_t cipher = des_encrypt(plain, key);
    uint64_t back   = des_decrypt(cipher, key);

    printf("Key        : %016llX\n", (unsigned long long)key);
    printf("Plaintext  : %016llX\n", (unsigned long long)plain);
    printf("Ciphertext : %016llX\n", (unsigned long long)cipher);
    printf("Decrypted  : %016llX  %s\n", (unsigned long long)back,
           back == plain ? "(ok)" : "(MISMATCH)");

    if (argc == 1)
        printf("Expected   : 85E813540F0AB405  %s\n",
               cipher == 0x85E813540F0AB405ULL ? "(matches)" : "(MISMATCH)");
    return 0;
}
