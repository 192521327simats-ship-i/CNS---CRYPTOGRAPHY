/*
 * Q26. In RSA, each user has public key e and private key d. Bob leaks his
 *      private key. Rather than generating a new modulus, he generates a new
 *      public and private key pair. Is this safe?
 *
 * Answer: No. Knowing e, d and n lets anyone factor n. Since e*d - 1 is a
 * multiple of phi(n), a random witness g gives a nontrivial square root of 1
 * modulo n with good probability, and gcd(x - 1, n) reveals p or q. Once n is
 * factored, the attacker can compute phi(n) and the private key for any new
 * exponent. Bob must generate a new modulus n, not just new exponents.
 *
 * This program uses the leaked (n, e, d) from a toy key (p=59, q=61) and
 * factors n, then derives Bob's new private key for a new e.
 *
 * Compile: gcc -Wall -o q26 q26_rsa_key_leak.c
 */
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long ull;

static ull mulmod(ull a, ull b, ull m) { return (a * b) % m; }

static ull powmod(ull b, ull e, ull m) {
    ull r = 1;
    b %= m;
    while (e) {
        if (e & 1) r = mulmod(r, b, m);
        b = mulmod(b, b, m);
        e >>= 1;
    }
    return r;
}

static ull gcd_u(ull a, ull b) {
    while (b) { ull t = a % b; a = b; b = t; }
    return a;
}

/* Finds a nontrivial factor of n using e and d. Returns 0 on failure. */
static ull factor_from_ed(ull n, ull e, ull d) {
    ull k = e * d - 1;
    ull r = k;
    int t = 0;
    while ((r & 1) == 0) { r >>= 1; t++; }

    for (ull g = 2; g < n; g++) {
        ull x = powmod(g, r, n);
        if (x == 1 || x == n - 1) continue;
        for (int i = 0; i < t; i++) {
            ull y = mulmod(x, x, n);
            if (y == 1) {                       /* x is a nontrivial sqrt of 1 */
                ull f = gcd_u(x - 1, n);
                if (f > 1 && f < n) return f;
                break;
            }
            if (y == n - 1) break;
            x = y;
        }
    }
    return 0;
}

static long ext_gcd(long a, long b, long *x, long *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    long x1, y1;
    long g = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

int main(void) {
    ull n = 3599, e = 31, d = 3031;     /* Bob's leaked key */
    printf("Leaked key: n=%llu e=%llu d=%llu\n", n, e, d);

    ull p = factor_from_ed(n, e, d);
    if (!p) { printf("Could not factor n from this key.\n"); return 1; }
    ull q = n / p;
    printf("Factored n: p=%llu q=%llu\n", p, q);

    /* Bob's "new" key pair on the same modulus. */
    long phi = (long)((p - 1) * (q - 1)), x, y;
    long new_e = 17;
    ext_gcd(new_e, phi, &x, &y);
    long new_d = ((x % phi) + phi) % phi;
    printf("Attacker computes new private key for e=%ld: d=%ld\n", new_e, new_d);
    printf("The 'new' key pair is not safe on the same n.\n");
    return 0;
}
