/*
 * Q20. Write a C program for ECB and CBC modes. In ECB, a bit error in one
 *      ciphertext block affects only the corresponding plaintext block. In
 *      CBC the error propagates. For example, an error in the transmitted C1
 *      corrupts P1 and P2.
 *        a. Are any blocks beyond P2 affected?
 *        b. Suppose there is a bit error in the source version of P1. Through
 *           how many ciphertext blocks is this error propagated? What is the
 *           effect at the receiver?
 *
 * Answers:
 *   a. No. Decryption of C3 uses only C2 (and the key), so an error in C1
 *      reaches only P1 and P2. Once the chain resynchronises, later blocks
 *      decrypt correctly.
 *   b. A bit error in P1 before encryption changes C1, and because each later
 *      block is encrypted from the previous ciphertext block, it changes
 *      every ciphertext block from C1 to the last one. The receiver gets
 *      the wrong data in all of them. (The program below shows this.)
 *
 * Compile: gcc -Wall -o q20 q20_cbc_error_propagation.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

#define N 4

static void cbc_encrypt(const uint64_t *p, uint64_t iv, uint64_t key, uint64_t *c) {
    uint64_t prev = iv;
    for (int i = 0; i < N; i++) {
        c[i] = des_encrypt(p[i] ^ prev, key);
        prev = c[i];
    }
}

static void cbc_decrypt(const uint64_t *c, uint64_t iv, uint64_t key, uint64_t *p) {
    uint64_t prev = iv;
    for (int i = 0; i < N; i++) {
        p[i] = des_decrypt(c[i], key) ^ prev;
        prev = c[i];
    }
}

int main(void) {
    uint64_t key = 0x133457799BBCDFF1ULL, iv = 0x0102030405060708ULL;
    uint64_t p[N] = {0x1111111111111111ULL, 0x2222222222222222ULL,
                     0x3333333333333333ULL, 0x4444444444444444ULL};
    uint64_t c[N], c_err[N], p_rx[N];

    /* Part (a): flip one bit in C1 (index 0) and decrypt. */
    cbc_encrypt(p, iv, key, c);
    for (int i = 0; i < N; i++) c_err[i] = c[i];
    c_err[0] ^= 1ULL << 10;
    cbc_decrypt(c_err, iv, key, p_rx);
    printf("Bit error in C1 (CBC decryption):\n");
    for (int i = 0; i < N; i++)
        printf("  P%d %s\n", i + 1, p_rx[i] == p[i] ? "correct" : "CORRUPTED");

    /* Part (b): flip one bit in source P1 before encryption. */
    uint64_t p_src[N];
    for (int i = 0; i < N; i++) p_src[i] = p[i];
    p_src[0] ^= 1ULL << 10;
    cbc_encrypt(p_src, iv, key, c_err);
    printf("\nBit error in source P1, then encrypted (compare ciphertext):\n");
    for (int i = 0; i < N; i++)
        printf("  C%d %s\n", i + 1, c_err[i] == c[i] ? "unchanged" : "changed");
    return 0;
}
