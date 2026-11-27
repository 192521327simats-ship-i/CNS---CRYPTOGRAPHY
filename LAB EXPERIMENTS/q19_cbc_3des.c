/*
 * Q19. Write a C program for encryption in cipher block chaining (CBC) mode
 *      using an algorithm stronger than DES. 3DES is a good candidate.
 *      Which of the two would you choose:
 *        a. For security?
 *        b. For performance?
 *
 * Answers:
 *   a. For security, 3DES. A single DES key (56 bits) can be brute-forced
 *      and 3DES has an effective strength of about 112 bits, so 3DES is
 *      much safer. (Even so, NIST has deprecated 3DES; AES is preferred.)
 *   b. For performance, plain DES. It runs about three times faster, but
 *      its key is too short to be secure today.
 *
 * This program uses two-key EDE 3DES: C = E_K1(D_K2(E_K1(P))) in CBC mode.
 *
 * Compile: gcc -Wall -o q19 q19_cbc_3des.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

static uint64_t tdes_enc(uint64_t p, uint64_t k1, uint64_t k2) {
    return des_encrypt(des_decrypt(des_encrypt(p, k1), k2), k1);
}

static uint64_t tdes_dec(uint64_t c, uint64_t k1, uint64_t k2) {
    return des_decrypt(des_encrypt(des_decrypt(c, k1), k2), k1);
}

int main(void) {
    uint64_t k1 = 0x0123456789ABCDEFULL;
    uint64_t k2 = 0x23456789ABCDEF01ULL;
    uint64_t iv = 0x1122334455667788ULL;
    uint64_t msg[3] = {0x0011223344556677ULL,
                       0x8899AABBCCDDEEFFULL,
                       0x0F0E0D0C0B0A0908ULL};
    uint64_t enc[3], dec[3];

    /* CBC encryption: C_i = 3DES(P_i XOR C_{i-1}), C_0 = IV */
    uint64_t prev = iv;
    for (int i = 0; i < 3; i++) {
        enc[i] = tdes_enc(msg[i] ^ prev, k1, k2);
        prev = enc[i];
    }

    /* CBC decryption: P_i = 3DES^-1(C_i) XOR C_{i-1} */
    prev = iv;
    for (int i = 0; i < 3; i++) {
        dec[i] = tdes_dec(enc[i], k1, k2) ^ prev;
        prev = enc[i];
    }

    for (int i = 0; i < 3; i++)
        printf("Block %d: plain %016llX  cipher %016llX  decrypted %016llX %s\n", i,
               (unsigned long long)msg[i], (unsigned long long)enc[i],
               (unsigned long long)dec[i], dec[i] == msg[i] ? "ok" : "MISMATCH");
    return 0;
}
