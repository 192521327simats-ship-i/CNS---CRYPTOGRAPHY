/*
 * Q36. The affine Caesar cipher is C = E([a,b], p) = (a*p + b) mod 26. The
 *      cipher must be one-to-one for decryption to be possible. It is not
 *      one-to-one for all a: for a = 2 and b = 3, E(0) = E(13) = 3.
 *
 * Answer: E is one-to-one exactly when gcd(a, 26) = 1, that is, when a is
 * coprime to 26 (a must be odd and not a multiple of 13). Then a has an
 * inverse mod 26, and decryption is p = a^-1 (C - b) mod 26.
 *
 * Usage: ./q36 [a] [b]      (defaults: a = 2, b = 3)
 * Compile: gcc -Wall -o q36 q36_affine_one_to_one.c
 */
#include <stdio.h>
#include <stdlib.h>

static int gcd(int a, int b) {
    while (b) { int t = a % b; a = b; b = t; }
    return a;
}

int main(int argc, char *argv[]) {
    int a = (argc > 1) ? atoi(argv[1]) : 2;
    int b = (argc > 2) ? atoi(argv[2]) : 3;

    printf("Affine Caesar cipher: C = (%d*p + %d) mod 26\n\n", a, b);

    int image[26];
    for (int p = 0; p < 26; p++)
        image[p] = ((a * p + b) % 26 + 26) % 26;

    int collisions = 0;
    for (int p = 0; p < 26; p++)
        for (int q = p + 1; q < 26; q++)
            if (image[p] == image[q]) {
                printf("E(%2d) = E(%2d) = %2d  (collision)\n", p, q, image[p]);
                collisions++;
            }

    if (collisions == 0)
        printf("One-to-one: yes. gcd(%d, 26) = %d.\n", a, gcd(a, 26));
    else
        printf("Not one-to-one: %d collision(s). gcd(%d, 26) = %d (must be 1).\n",
               collisions, a, gcd(a, 26));
    return 0;
}
