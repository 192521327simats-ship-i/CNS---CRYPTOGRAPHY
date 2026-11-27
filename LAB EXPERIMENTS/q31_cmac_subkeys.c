/*
 * Q31. Subkey generation in CMAC (NIST SP 800-38B):
 *   L = E_K(0^b)                        (cipher applied to an all-zero block)
 *   K1 = L << 1, XOR Rb if MSB(L) = 1   (left shift, then conditional XOR)
 *   K2 = K1 << 1, XOR Rb if MSB(K1) = 1 (same operation on K1)
 *
 *   a. Constants: Rb = 0x1B for 64-bit blocks (x^64 + x^4 + x^3 + x + 1)
 *                 Rb = 0x87 for 128-bit blocks (x^128 + x^7 + x^2 + x + 1)
 *   b. Why the shift and XOR work: a left shift multiplies the block by x in
 *      GF(2^b). When the bit shifted out is 1, the result has degree b, which
 *      is reduced by the field polynomial. XOR-ing with Rb (the low-order terms
 *      of that polynomial) performs that reduction. The result is a subkey
 *      that is a different, non-trivial function of K, and the doubling is
 *      repeated for K2 so the two subkeys are distinct.
 *
 * This program uses DES (64-bit block) to compute K1 and K2 and prints the
 * constants for both block sizes.
 *
 * Compile: gcc -Wall -o q31 q31_cmac_subkeys.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

#define RB64  0x1BULL
#define RB128 0x87ULL

/* Doubling in GF(2^64): shift left, XOR the constant if the top bit was set. */
static uint64_t dbl64(uint64_t v) {
    uint64_t out = v << 1;
    if (v >> 63) out ^= RB64;
    return out;
}

int main(void) {
    uint64_t key = 0x133457799BBCDFF1ULL;

    uint64_t L  = des_encrypt(0x0000000000000000ULL, key);
    uint64_t K1 = dbl64(L);
    uint64_t K2 = dbl64(K1);

    printf("64-bit block (DES), key %016llX\n", (unsigned long long)key);
    printf("  L  = E_K(0)  = %016llX\n", (unsigned long long)L);
    printf("  K1 = L<<1    = %016llX\n", (unsigned long long)K1);
    printf("  K2 = K1<<1   = %016llX\n", (unsigned long long)K2);

    printf("\nConstants:\n");
    printf("  64-bit block:  Rb = 0x%02llX\n", (unsigned long long)RB64);
    printf("  128-bit block: Rb = 0x%02llX\n", (unsigned long long)RB128);
    return 0;
}
