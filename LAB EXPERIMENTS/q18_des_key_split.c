/*
 * Q18. Write a C program for DES where the first 24 bits of each subkey come
 *      from the same subset of 28 bits of the initial key, and the second 24
 *      bits of each subkey come from a disjoint subset of 28 bits of the
 *      initial key.
 *
 * In DES the 56-bit key is split by PC-1 into C (28 bits) and D (28 bits).
 * PC-2 picks subkey bits 1..24 from C and bits 25..48 from D, in every round.
 * This program builds the key schedule and checks that property for all 16
 * subkeys, then prints which initial-key bits feed each half.
 *
 * Compile: gcc -Wall -o q18 q18_des_key_split.c
 */
#include <stdio.h>
#include <stdint.h>
#include "des.h"

int main(void) {
    uint64_t key = 0x133457799BBCDFF1ULL;
    uint64_t sk[16];
    des_key_schedule(key, sk);

    /* Which PC-2 source positions are in C (1..28) and D (29..56)? */
    int first_from_C = 1, last_from_D = 1;
    for (int i = 0; i < 24; i++)
        if (DES_PC2[i] > 28) first_from_C = 0;
    for (int i = 24; i < 48; i++)
        if (DES_PC2[i] <= 28) last_from_D = 0;

    printf("First 24 subkey bits come from C-half positions: %s\n",
           first_from_C ? "yes" : "no");
    printf("Last 24 subkey bits come from D-half positions:  %s\n",
           last_from_D ? "yes" : "no");

    /* Initial-key bits feeding C and D (through PC-1). */
    printf("\nInitial-key bits feeding C (the 28 bits used for subkey bits 1-24):\n  ");
    for (int i = 0; i < 28; i++)
        printf("%d ", DES_PC1[i]);
    printf("\nInitial-key bits feeding D (the 28 bits used for subkey bits 25-48):\n  ");
    for (int i = 28; i < 56; i++)
        printf("%d ", DES_PC1[i]);
    printf("\n");

    /* Each round's subkey: its first 24 bits are the C-derived part. */
    printf("\nSubkey K1 = %012llX\n", (unsigned long long)sk[0]);
    return 0;
}
