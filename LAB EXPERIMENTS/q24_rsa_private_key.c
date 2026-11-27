/*
 * Q24. Write a C program for the RSA system. The public key of a user is
 *      e = 31, n = 3599. What is the private key?
 *      Hint: find p and q by trial division, then use the extended Euclidean
 *      algorithm to find the inverse of 31 modulo phi(n).
 *
 * Result: 3599 = 59 * 61, so phi(n) = 58 * 60 = 3480, and d = 31^-1 mod 3480
 * = 3031 (check: 31 * 3031 = 93961 = 27 * 3480 + 1).
 *
 * Compile: gcc -Wall -o q24 q24_rsa_private_key.c
 */
#include <stdio.h>

/* Extended Euclid: returns gcd(a, b) and sets x, y with a*x + b*y = gcd. */
static long ext_gcd(long a, long b, long *x, long *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    long x1, y1;
    long g = ext_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

static long mod_inverse(long a, long m) {
    long x, y;
    long g = ext_gcd(a, m, &x, &y);
    if (g != 1) return -1;
    return ((x % m) + m) % m;
}

int main(void) {
    long n = 3599, e = 31;
    long p = 0, q = 0;

    /* Trial division to factor n. */
    for (long t = 2; t * t <= n; t++)
        if (n % t == 0) { p = t; q = n / t; break; }

    if (p == 0) { printf("n is prime; cannot factor.\n"); return 1; }

    long phi = (p - 1) * (q - 1);
    long d = mod_inverse(e, phi);

    printf("n = %ld = %ld * %ld\n", n, p, q);
    printf("phi(n) = %ld\n", phi);
    if (d < 0) { printf("e is not invertible mod phi(n).\n"); return 1; }
    printf("Public key : (e=%ld, n=%ld)\n", e, n);
    printf("Private key: d=%ld  (check: e*d mod phi = %ld)\n", d, (e * d) % phi);
    return 0;
}
