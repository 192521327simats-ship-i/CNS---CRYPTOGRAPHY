/*
 * Q35. Write a C program for one-time pad version of the Vigenere cipher. The
 *      key is a stream of random numbers between 0 and 26. For example, if the
 *      key is 3 19 5 ..., the first letter is shifted by 3, the second by 19,
 *      the third by 5, and so on.
 *
 * Notes:
 *   - Key values 0..26 are reduced mod 26 when used, so 26 acts like 0.
 *   - Security of a one-time pad needs a truly random, secret, never-reused
 *     key as long as the message. rand() is used here for demonstration only.
 *
 * Compile: gcc -Wall -o q35 q35_otp_vigenere_random.c
 * Usage  : ./q35 [message]     (default message: "SENDMOREMONEY")
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXMSG 512

int main(int argc, char *argv[]) {
    const char *raw = (argc > 1) ? argv[1] : "SENDMOREMONEY";
    char msg[MAXMSG], cipher[MAXMSG], back[MAXMSG];
    int key[MAXMSG];
    int n = 0;

    for (const char *p = raw; *p && n < MAXMSG - 1; p++)
        if (isalpha((unsigned char)*p))
            msg[n++] = (char)toupper((unsigned char)*p);
    msg[n] = '\0';
    if (n == 0) { printf("No letters in message.\n"); return 1; }

    /* Random key stream: values 0..26. */
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++)
        key[i] = rand() % 27;

    /* Encrypt: c_i = (p_i + k_i) mod 26 */
    for (int i = 0; i < n; i++)
        cipher[i] = (char)('A' + (msg[i] - 'A' + key[i]) % 26);
    cipher[n] = '\0';

    /* Decrypt: p_i = (c_i - k_i) mod 26 */
    for (int i = 0; i < n; i++)
        back[i] = (char)('A' + (((cipher[i] - 'A') - key[i]) % 26 + 26) % 26);
    back[n] = '\0';

    printf("Plaintext : %s\n", msg);
    printf("Key stream: ");
    for (int i = 0; i < n; i++) printf("%d ", key[i]);
    printf("\nCiphertext: %s\n", cipher);
    printf("Decrypted : %s %s\n", back, strcmp(back, msg) == 0 ? "(ok)" : "(MISMATCH)");
    return 0;
}
