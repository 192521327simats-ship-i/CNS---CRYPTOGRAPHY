/*
 * Q32. In DSA the value k is chosen fresh for each signature, so signing the
 *      same message twice gives different signatures. RSA signatures on the
 *      same message are identical. What is the implication of this difference?
 *
 * Implication: DSA is randomised, so signatures on the same message differ,
 * which is what you want. The cost is that k must be secret and must never
 * repeat. If two messages are signed with the same k, the private key can be
 * recovered:
 *     k = (H1 - H2) / (s1 - s2) mod q
 *     x = (s1 * k - H1) / r mod q
 * This program shows the randomisation and then the key recovery.
 *
 * Toy parameters: p = 23, q = 11 (divides p - 1), g = 2 (order 11 mod 23).
 *
 * Compile: gcc -Wall -o q32 q32_dsa_k_reuse.c
 */
#include <stdio.h>

static long mod(long a, long m) { return ((a % m) + m) % m; }

static long powmod(long b, long e, long m) {
    long r = 1;
    b = mod(b, m);
    while (e) {
        if (e & 1) r = mod(r * b, m);
        b = mod(b * b, m);
        e >>= 1;
    }
    return r;
}

static long ext_gcd(long a, long b, long *x, long *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    long x1, y1;
    long g = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

static long inv(long a, long m) {
    long x, y;
    ext_gcd(mod(a, m), m, &x, &y);
    return mod(x, m);
}

static const long P = 23, Q = 11, G = 2;

/* DSA signing with explicit nonce k (k must be fresh and secret). */
static void dsa_sign(long H, long x, long k, long *r, long *s) {
    *r = mod(powmod(G, k, P), Q);
    *s = mod(inv(k, Q) * (H + x * *r), Q);
}

int main(void) {
    long x = 7;                         /* private key */
    long y = powmod(G, x, P);           /* public key */
    long H1 = 5, H2 = 9;                /* hashes of two messages */

    /* Randomisation: the same message, two different k values. */
    long r1, s1, r2, s2;
    dsa_sign(H1, x, 3, &r1, &s1);
    dsa_sign(H1, x, 8, &r2, &s2);
    printf("Public key y = %ld\n", y);
    printf("Same message, k=3: (r,s) = (%ld,%ld)\n", r1, s1);
    printf("Same message, k=8: (r,s) = (%ld,%ld)  %s\n", r2, s2,
           (r1 != r2 || s1 != s2) ? "(different signatures)" : "(same)");

    /* Reuse: sign two messages with the same k = 3. */
    long rr, sa, sb;
    dsa_sign(H1, x, 3, &rr, &sa);
    long rr2;
    dsa_sign(H2, x, 3, &rr2, &sb);
    printf("\nk reused (k=3) for H1=%ld and H2=%ld: r equal = %s\n",
           H1, H2, rr == rr2 ? "yes" : "no");

    /* Attacker recovers k and then x. */
    long k_rec = mod((H1 - H2) * inv(sa - sb, Q), Q);
    long x_rec = mod((sa * k_rec - H1) * inv(rr, Q), Q);
    printf("Recovered k = %ld (actual 3)\n", k_rec);
    printf("Recovered private key x = %ld (actual %ld) %s\n", x_rec, x,
           x_rec == x ? "(key exposed)" : "(MISMATCH)");
    return 0;
}
