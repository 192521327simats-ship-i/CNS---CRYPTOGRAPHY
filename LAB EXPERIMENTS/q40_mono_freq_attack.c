/*
 * Q40. Write a C program that can perform a letter frequency attack on any
 *      monoalphabetic substitution cipher without human intervention. Produce
 *      possible plaintexts in rough order of likelihood, and let the user ask
 *      for the top N (e.g., 10).
 *
 * Approach:
 *   1. Start from a guess: cipher letters sorted by frequency are mapped to
 *      English letters sorted by frequency (ETAOIN...).
 *   2. Hill-climb: swap the plaintext letters assigned to two cipher letters
 *      and keep the swap if the unigram log-likelihood improves.
 *   3. Repeat from random starting keys, collect distinct results, and print
 *      the best N sorted by score.
 *
 * Note: scoring uses single-letter frequencies only. Adding bigram or
 * quadgram statistics would give more reliable results on short texts.
 *
 * Compile: gcc -Wall -o q40 q40_mono_freq_attack.c -lm
 * Usage  : ./q40 [N]     then type the ciphertext and press Enter.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_TEXT 2048
#define MAX_RESULTS 500
#define RESTARTS 60
#define PATIENCE 3000

static const double ENG_FREQ[26] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
    0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056,
    2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

typedef struct { double score; char text[MAX_TEXT]; } Result;

static char clean[MAX_TEXT];     /* ciphertext letters only, uppercase */
static int clen;

/* key[c] = plaintext letter assigned to cipher letter c (0..25). */
static void decode(const int key[26], char *out) {
    for (int i = 0; i < clen; i++)
        out[i] = (char)('A' + key[clean[i] - 'A']);
    out[clen] = '\0';
}

/* Unigram log-likelihood of decoded text. Higher is more English-like. */
static double score_key(const int key[26]) {
    double s = 0.0;
    for (int i = 0; i < clen; i++)
        s += log(ENG_FREQ[key[clean[i] - 'A']] / 100.0);
    return s;
}

static int by_score_desc(const void *a, const void *b) {
    double x = ((const Result *)a)->score, y = ((const Result *)b)->score;
    return (x < y) - (x > y);
}

int main(int argc, char *argv[]) {
    int top = (argc > 1) ? atoi(argv[1]) : 10;
    if (top < 1) top = 10;

    static char input[MAX_TEXT];
    printf("Enter ciphertext: ");
    if (!fgets(input, sizeof input, stdin)) return 1;

    clen = 0;
    for (const char *p = input; *p; p++)
        if (isalpha((unsigned char)*p))
            clean[clen++] = (char)toupper((unsigned char)*p);
    clean[clen] = '\0';
    if (clen == 0) { printf("No letters found.\n"); return 1; }

    /* Frequency rank of cipher letters, mapped to English letters by rank. */
    const char eng_order[] = "ETAOINSHRDLCUMWFGYPBVKJXQZ";
    int count[26] = {0};
    for (int i = 0; i < clen; i++) count[clean[i] - 'A']++;
    int rank[26];
    for (int i = 0; i < 26; i++) rank[i] = i;
    for (int i = 0; i < 26; i++)
        for (int j = i + 1; j < 26; j++)
            if (count[rank[j]] > count[rank[i]]) {
                int t = rank[i]; rank[i] = rank[j]; rank[j] = t;
            }
    int base_key[26];
    for (int i = 0; i < 26; i++)
        base_key[rank[i]] = eng_order[i] - 'A';

    static Result results[MAX_RESULTS];
    int nres = 0;
    srand(12345);

    for (int r = 0; r < RESTARTS; r++) {
        int key[26];
        memcpy(key, base_key, sizeof key);
        if (r > 0)   /* random restarts shuffle the starting key */
            for (int i = 25; i > 0; i--) {
                int j = rand() % (i + 1), t = key[i];
                key[i] = key[j]; key[j] = t;
            }

        double best = score_key(key);
        int stale = 0;
        while (stale < PATIENCE) {
            int a = rand() % 26, b = rand() % 26;
            if (a == b) { stale++; continue; }
            int t = key[a]; key[a] = key[b]; key[b] = t;
            double s = score_key(key);
            if (s > best) { best = s; stale = 0; }
            else { t = key[a]; key[a] = key[b]; key[b] = t; stale++; }
        }

        Result res;
        res.score = best;
        decode(key, res.text);

        int dup = 0;
        for (int i = 0; i < nres; i++)
            if (strcmp(results[i].text, res.text) == 0) { dup = 1; break; }
        if (!dup && nres < MAX_RESULTS)
            results[nres++] = res;
    }

    qsort(results, nres, sizeof results[0], by_score_desc);

    if (top > nres) top = nres;
    printf("\nTop %d candidate plaintexts (most likely first):\n", top);
    for (int i = 0; i < top; i++)
        printf("%2d. score=%9.2f  %s\n", i + 1, results[i].score, results[i].text);
    return 0;
}
