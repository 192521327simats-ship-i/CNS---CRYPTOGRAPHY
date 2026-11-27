/*
 * Q39. Write a C program that can perform a letter frequency attack on an
 *      additive cipher without human intervention. Your software should
 *      produce possible plaintexts in rough order of likelihood. The user
 *      interface should allow the user to specify "give me the top 10
 *      possible plaintexts."
 *
 * Approach: an additive cipher has only 26 keys. Try every shift, score each
 * candidate by chi-squared distance from English letter frequencies, and
 * print the best N (default 10, or given as the first argument).
 *
 * Compile: gcc -Wall -o q39 q39_additive_freq_attack.c
 * Usage  : ./q39 [N]     then type the ciphertext and press Enter.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const double ENG_FREQ[26] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
    0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056,
    2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

/* Chi-squared distance of the letters in text from English. Lower is better. */
static double chi_squared(const char *text) {
    int count[26] = {0}, total = 0;
    for (const char *p = text; *p; p++)
        if (isalpha((unsigned char)*p)) {
            count[toupper((unsigned char)*p) - 'A']++;
            total++;
        }
    if (total == 0)
        return 1e18;
    double chi = 0.0;
    for (int i = 0; i < 26; i++) {
        double expected = total * ENG_FREQ[i] / 100.0;
        double d = count[i] - expected;
        chi += d * d / expected;
    }
    return chi;
}

typedef struct { int shift; double score; char text[1024]; } Candidate;

static int by_score(const void *a, const void *b) {
    double x = ((const Candidate *)a)->score, y = ((const Candidate *)b)->score;
    return (x > y) - (x < y);   /* ascending: lower chi-squared first */
}

int main(int argc, char *argv[]) {
    int top = (argc > 1) ? atoi(argv[1]) : 10;
    if (top < 1) top = 10;
    if (top > 26) top = 26;

    char cipher[1024];
    printf("Enter ciphertext: ");
    if (!fgets(cipher, sizeof cipher, stdin)) return 1;
    cipher[strcspn(cipher, "\n")] = '\0';

    Candidate c[26];
    for (int s = 0; s < 26; s++) {
        c[s].shift = s;
        size_t j = 0;
        for (const char *p = cipher; *p && j < sizeof c[s].text - 1; p++) {
            if (isalpha((unsigned char)*p)) {
                char u = (char)toupper((unsigned char)*p);
                c[s].text[j++] = (char)('A' + ((u - 'A' - s + 26) % 26));
            } else {
                c[s].text[j++] = *p;
            }
        }
        c[s].text[j] = '\0';
        c[s].score = chi_squared(c[s].text);
    }

    qsort(c, 26, sizeof c[0], by_score);

    printf("\nTop %d candidate plaintexts (most likely first):\n", top);
    for (int i = 0; i < top; i++)
        printf("%2d. shift %2d  chi2=%8.2f  %s\n", i + 1, c[i].shift, c[i].score, c[i].text);
    return 0;
}
