/*
 * Q25. Write a C program for a set of blocks encoded with RSA when we do not
 *      have the private key. n = pq and e is public. Someone says one of the
 *      plaintext blocks has a common factor with n. Does this help?
 *
 * Answer: Yes, it breaks the system. If a plaintext block m shares a factor
 * with n, then gcd(m, n) is a nontrivial factor p or q. Dividing n by it
 * gives the other prime, and phi(n) and then d follow directly. The program
 * below finds a factor with one gcd call and recovers d.
 *
 * Compile: gcc -Wall -o q25 q25_rsa_common_factor.c
 */
#include <stdio.h>

static long gcd(long a, long b) {
    while (b) { long t = a % b; a = b; b = t; }
    return a;
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
    /* Example: n = 59 * 61. Block m = 118 = 2 * 59 shares the factor 59. */
    long n = 3599, e = 31;
    long m = 118;

    long g = gcd(m, n);
    printf("gcd(m=%ld, n=%ld) = %ld\n", m, n, g);

    if (g > 1 && g < n) {
        long p = g, q = n / g;
        long phi = (p - 1) * (q - 1), x, y;
        ext_gcd(e, phi, &x, &y);
        long d = ((x % phi) + phi) % phi;
        printf("Factored n: p=%ld, q=%ld\n", p, q);
        printf("Recovered private key d=%ld\n", d);
    } else {
        printf("No common factor: the block gives no information about n's factors.\n");
    }
    return 0;
}
