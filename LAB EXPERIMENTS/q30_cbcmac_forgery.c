/*
 * Q30. For CBC-MAC of a one-block message X, let T = MAC(K, X). The adversary
 *      immediately knows the CBC-MAC of the two-block message X || (X XOR T),
 *      because it is T again.
 *
 * CBC-MAC with a one-block message: MAC = E_K(X), so T = E_K(X).
 * For the two-block message M = X || (X XOR T):
 *   block 1: Y1 = E_K(X)              = T
 *   block 2: Y2 = E_K(Y1 XOR (X XOR T)) = E_K(T XOR X XOR T) = E_K(X) = T
 * So MAC(M) = T. The attacker forges a valid MAC for a message they never
 * had a MAC for, which is why CBC-MAC must only be used for fixed-length
 * messages (or with a length-prefix or a CMAC-style fix).
 *
 * Compile: gcc -Wall -o q30 q30_cbcmac_forgery.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

static uint64_t cbc_mac(const uint64_t *blocks, int n, uint64_t key) {
    uint64_t y = 0;                       /* zero IV */
    for (int i = 0; i < n; i++)
        y = des_encrypt(blocks[i] ^ y, key);
    return y;
}

int main(void) {
    uint64_t key = 0x133457799BBCDFF1ULL;
    uint64_t X = 0x0123456789ABCDEFULL;

    uint64_t T = cbc_mac(&X, 1, key);     /* MAC of the one-block message X */

    uint64_t forged[2] = {X, X ^ T};      /* the two-block message X || (X XOR T) */
    uint64_t mac2 = cbc_mac(forged, 2, key);

    printf("T = MAC(K, X)              = %016llX\n", (unsigned long long)T);
    printf("MAC(K, X || (X XOR T))     = %016llX\n", (unsigned long long)mac2);
    printf("Equal to T?                %s\n", mac2 == T ? "yes" : "no");
    return 0;
}
