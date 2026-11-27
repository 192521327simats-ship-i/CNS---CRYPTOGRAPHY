/*
 * Q29. SHA-3 with a 1024-bit block (rate r = 1024, capacity c = 576). Assume
 *      every lane of the first message block P0 has at least one nonzero bit.
 *      The capacity lanes start at zero. How long until all of them have a
 *      nonzero bit? Ignore the permutation (pi) step: keep track of the
 *      original capacity lanes even when they move.
 *
 * Model: the 1600-bit state is 25 lanes of 64 bits, A[x][y]. Lane index is
 * i = x + 5y. The rate has 16 lanes (indices 0..15), the capacity has the
 * other 9 (indices 16..24). The program:
 *   - sets the 16 rate lanes to nonzero values (as the first block does),
 *   - applies the Keccak round steps theta and chi (rho, pi and iota are
 *     skipped; the question says to ignore the permutation, and iota only
 *     changes lane (0,0), which is a rate lane),
 *   - counts rounds until all 9 original capacity lanes are nonzero.
 * The rounds are counted on the original capacity lanes, not on their
 * positions in the matrix.
 *
 * Compile: gcc -Wall -o q29 q29_sha3_lanes.c
 */
#include <stdio.h>
#include <stdint.h>

typedef uint64_t u64;
static inline u64 rotl(u64 v, int n) { return n == 0 ? v : (v << n) | (v >> (64 - n)); }

static void theta(u64 A[5][5]) {
    u64 C[5], D[5];
    for (int x = 0; x < 5; x++)
        C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
    for (int x = 0; x < 5; x++)
        D[x] = C[(x + 4) % 5] ^ rotl(C[(x + 1) % 5], 1);
    for (int x = 0; x < 5; x++)
        for (int y = 0; y < 5; y++)
            A[x][y] ^= D[x];
}

static void chi(u64 A[5][5]) {
    for (int y = 0; y < 5; y++) {
        u64 row[5];
        for (int x = 0; x < 5; x++) row[x] = A[x][y];
        for (int x = 0; x < 5; x++)
            A[x][y] = row[x] ^ ((~row[(x + 1) % 5]) & row[(x + 2) % 5]);
    }
}

int main(void) {
    u64 A[5][5] = {{0}};

    /* First block: 16 rate lanes (index = x + 5y) get nonzero values. */
    for (int i = 0; i < 16; i++)
        A[i % 5][i / 5] = 0x0123456789ABCDEFULL ^ ((u64)(i + 1) * 0x9E3779B97F4A7C15ULL);

    int capacity_idx[9];
    for (int i = 0; i < 9; i++) capacity_idx[i] = 16 + i;

    printf("Start: 16 rate lanes nonzero, 9 capacity lanes zero.\n");

    int rounds = 0;
    for (;;) {
        int all_nonzero = 1;
        for (int i = 0; i < 9; i++) {
            int idx = capacity_idx[i];
            if (A[idx % 5][idx / 5] == 0) { all_nonzero = 0; break; }
        }
        if (all_nonzero) break;
        if (rounds >= 24) { printf("Not reached within 24 rounds.\n"); return 1; }

        theta(A);
        chi(A);
        rounds++;
        printf("After round %2d: ", rounds);
        int nz = 0;
        for (int i = 0; i < 9; i++) {
            int idx = capacity_idx[i];
            if (A[idx % 5][idx / 5] != 0) nz++;
        }
        printf("%d of 9 original capacity lanes nonzero\n", nz);
    }

    printf("\nAll 9 original capacity lanes are nonzero after %d round(s).\n", rounds);
    return 0;
}
