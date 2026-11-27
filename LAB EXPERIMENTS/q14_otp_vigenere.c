/*
 * Q14. Write a C program for one-time pad version of the Vigenere cipher.
 *      The key is a stream of random numbers between 0 and 26. For example,
 *      if the key is 3 19 5 ..., the first letter is shifted by 3, the second
 *      by 19, the third by 5, and so on.
 *      a. Encrypt "send more money" with the key stream
 *         9 0 1 7 23 15 21 14 11 11 2 8 9
 *      b. Using the ciphertext from (a), find a key so that the ciphertext
 *         decrypts to "cash not needed".
 *
 * Compile: gcc -Wall -o q14 q14_otp_vigenere.c
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int mod26(int x) { return ((x % 26) + 26) % 26; }

static void normalize(const char *in, char *out) {
    int j = 0;
    for (int i = 0; in[i]; i++)
        if (isalpha((unsigned char)in[i]))
            out[j++] = (char)toupper((unsigned char)in[i]);
    out[j] = '\0';
}

static void print_key(const int *key, int n) {
    for (int i = 0; i < n; i++)
        printf("%d%s", key[i], i < n - 1 ? " " : "\n");
}

int main(void) {
    char plain[256], cipher[256], target[256], check[256];
    const char plain_raw[] = "send more money";
    const char target_raw[] = "cash not needed";
    int key_a[] = {9, 0, 1, 7, 23, 15, 21, 14, 11, 11, 2, 8, 9};
    int n = (int)(sizeof key_a / sizeof key_a[0]);
    int key_b[256];

    /* (a) */
    normalize(plain_raw, plain);
    for (int i = 0; i < n; i++)
        cipher[i] = (char)('A' + mod26((plain[i] - 'A') + key_a[i]));
    cipher[n] = '\0';
    printf("(a) Plaintext : %s\n", plain);
    printf("    Key stream: "); print_key(key_a, n);
    printf("    Ciphertext: %s\n\n", cipher);

    /* (b) */
    normalize(target_raw, target);
    if ((int)strlen(target) != n) { printf("Length mismatch.\n"); return 1; }
    for (int i = 0; i < n; i++)
        key_b[i] = mod26((cipher[i] - 'A') - (target[i] - 'A'));
    printf("(b) Target plaintext: %s\n", target);
    printf("    Key that decrypts to target: "); print_key(key_b, n);

    for (int i = 0; i < n; i++)
        check[i] = (char)('A' + mod26((target[i] - 'A') + key_b[i]));
    check[n] = '\0';
    printf("    Verification: %s %s\n", check,
           strcmp(check, cipher) == 0 ? "(matches)" : "(MISMATCH)");
    return 0;
}
