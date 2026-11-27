/*
 * Q27. Bob uses RSA with a very large modulus n that cannot be factored in
 *      practice. Alice encodes each letter as an integer 0..25 (A=0 ... Z=25)
 *      and encrypts each number separately with RSA. Is this secure? If not,
 *      what is the most efficient attack?
 *
 * Answer: No. RSA is deterministic, and only 26 messages are possible. Anyone
 * who knows Bob's public key (e, n) can encrypt all 26 values in advance and
 * build a table. Each intercepted block is then matched against the table in
 * one lookup. No factoring is needed. The fix is to use padding (such as
 * OAEP) or a symmetric cipher with random IVs.
 *
 * This demo uses a small modulus (n = 61 * 53 = 3233, e = 17, d = 2753) so the
 * numbers stay readable.
 *
 * Compile: gcc -Wall -o q27 q27_rsa_char_attack.c
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

int main(void) {
    const ull n = 3233, e = 17, d = 2753;   /* Bob's public and private key */
    const char *message = "HELLO";

    /* Alice encrypts each letter. */
    ull cipher[64];
    int len = (int)strlen(message);
    for (int i = 0; i < len; i++) {
        ull m = (ull)(toupper((unsigned char)message[i]) - 'A');
        cipher[i] = powmod(m, e, n);
    }

    /* Attacker: precompute E(0..25) with the public key, then look up. */
    ull table[26];
    for (int v = 0; v < 26; v++)
        table[v] = powmod((ull)v, e, n);

    printf("Ciphertext blocks: ");
    for (int i = 0; i < len; i++) printf("%llu ", cipher[i]);
    printf("\n\nAttacker recovers:  ");
    for (int i = 0; i < len; i++) {
        char rec = '?';
        for (int v = 0; v < 26; v++)
            if (table[v] == cipher[i]) { rec = (char)('A' + v); break; }
        printf("%c", rec);
    }
    printf("\n");

    /* Bob decrypts to confirm. */
    printf("Bob decrypts:       ");
    for (int i = 0; i < len; i++)
        printf("%c", (char)('A' + powmod(cipher[i], d, n)));
    printf("\n");
    return 0;
}
