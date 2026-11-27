/*
 * Q28. In Diffie-Hellman each participant picks a secret x and sends a^x mod q
 *      for a public a. What happens if they send x^a instead? Give a method
 *      Alice and Bob could use to agree on a key. Can Eve break the system
 *      without the secrets? Can Eve find the secrets?
 *
 * Answers:
 *   - Correct protocol: Alice sends A = g^x mod p, Bob sends B = g^y mod p.
 *     Both compute the shared key g^(xy) mod p. Eve sees g, p, A and B. To
 *     get the key she must solve the discrete logarithm problem, which is hard
 *     for large p.
 *   - Variant x^a: the public values are not based on exponentiation of a
 *     fixed base, so there is no discrete-log hardness. If the values are sent
 *     without reduction, Eve takes the a-th root of what she sees and gets x.
 *     Even with reduction mod q, x^a mod q has few values for a small a, and
 *     Eve can search for x directly. Either way, Eve can find the secrets.
 *   - The program below runs the correct protocol and then shows Eve breaking
 *     the x^a variant with an integer root.
 *
 * Compile: gcc -Wall -o q28 q28_diffie_hellman.c
 */
#include <stdio.h>

typedef unsigned long long ull;

static ull powmod(ull b, ull e, ull m) {
    ull r = 1;
    b %= m;
    while (e) {
        if (e & 1) r = (r * b) % m;
        b = (b * b) % m;
        e >>= 1;
    }
    return r;
}

/* Integer a-th root of y by search (exact for the small demo values). */
static ull int_root(ull y, ull a) {
    for (ull x = 0; x <= y; x++) {
        ull v = 1;
        for (ull i = 0; i < a && v <= y; i++) v *= x;
        if (v == y) return x;
        if (v > y) break;
    }
    return 0;
}

int main(void) {
    const ull p = 23, g = 5;      /* public prime and generator */
    ull x = 6, y = 15;            /* Alice's and Bob's secrets */

    /* Correct Diffie-Hellman. */
    ull A = powmod(g, x, p);      /* Alice sends */
    ull B = powmod(g, y, p);      /* Bob sends */
    ull k_alice = powmod(B, x, p);
    ull k_bob   = powmod(A, y, p);
    printf("Correct protocol:\n");
    printf("  A = g^x mod p = %llu, B = g^y mod p = %llu\n", A, B);
    printf("  Alice key %llu, Bob key %llu %s\n", k_alice, k_bob,
           k_alice == k_bob ? "(agree)" : "(MISMATCH)");

    /* Broken variant: Alice sends x^a with a public a = 5, no reduction. */
    ull a = 5;
    ull sent = 1;
    for (ull i = 0; i < a; i++) sent *= x;   /* 6^5 = 7776 */
    printf("\nVariant x^a: Alice sends %llu\n", sent);
    printf("  Eve takes the %llu-th root: x = %llu (secret recovered)\n",
           a, int_root(sent, a));
    return 0;
}
