/*
 * Q17. Write a C program for DES algorithm for decryption, the 16 keys
 *      (K1, K2, ..., K16) are used in reverse order. Design a key-generation
 *      scheme with the appropriate shift schedule for the decryption process.
 *
 * Design: decryption needs K16, K15, ..., K1. Because the total left shift
 * over 16 rounds is 28 (a full turn of each 28-bit half), C16 = C0 and
 * D16 = D0. So the decryption schedule starts from C0/D0 and rotates RIGHT
 * by the shifts in reverse order (S16, S15, ..., S2):
 *     output K16 from (C0,D0); rotate right by S16 -> C15,D15
 *     output K15;              rotate right by S15 -> C14,D14
 *     ...
 *     output K1.
 *
 * Compile: gcc -Wall -o q17 q17_des_decrypt.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

/* Decryption key schedule: dk[0] = K16, ..., dk[15] = K1, using right rotations. */
static void des_decrypt_schedule(uint64_t key, uint64_t dk[16]) {
    uint64_t cd = des_permute(key, DES_PC1, 64, 56);
    uint32_t c = (uint32_t)(cd >> 28) & 0xFFFFFFFu;
    uint32_t d = (uint32_t)cd & 0xFFFFFFFu;
    for (int r = 0; r < 16; r++) {
        dk[r] = des_permute(((uint64_t)c << 28) | d, DES_PC2, 56, 48);
        int s = DES_SHIFTS[15 - r];           /* S16, S15, ..., S1 */
        c = ((c >> s) | (c << (28 - s))) & 0xFFFFFFFu;
        d = ((d >> s) | (d << (28 - s))) & 0xFFFFFFFu;
    }
}

int main(void) {
    uint64_t key   = 0x133457799BBCDFF1ULL;
    uint64_t plain = 0x0123456789ABCDEFULL;

    uint64_t cipher = des_encrypt(plain, key);

    uint64_t dk[16];
    des_decrypt_schedule(key, dk);
    uint64_t recovered = des_process(cipher, dk);

    printf("Key        : %016llX\n", (unsigned long long)key);
    printf("Plaintext  : %016llX\n", (unsigned long long)plain);
    printf("Ciphertext : %016llX\n", (unsigned long long)cipher);
    printf("Decrypted  : %016llX  %s\n", (unsigned long long)recovered,
           recovered == plain ? "(matches plaintext)" : "(MISMATCH)");
    return 0;
}
