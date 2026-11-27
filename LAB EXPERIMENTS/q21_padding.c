/*
 * Q21. Write a C program for ECB, CBC, and CFB modes. The plaintext must be
 *      a whole number of blocks (or segments). Padding consists of a 1 bit
 *      followed by as many zero bits as needed to fill the final block. The
 *      sender pads every message, even when the last block is already full.
 *      What is the motivation for padding when padding is not needed?
 *
 * Answer: If padding were only added when needed, a receiver who sees a
 * message whose length is already a multiple of the block size could not tell
 * whether the last block is data or padding. Always adding padding makes the
 * padding removable without ambiguity: the receiver finds the last 1 bit
 * and discards it and the zeros after it. Padding is optional only when the
 * message length is carried separately.
 *
 * This program works on whole bytes: the padding byte is 0x80 (bit 1 then 7
 * zeros), followed by 0x00 bytes up to the block boundary. A full block is
 * added when the message is already aligned.
 *
 * Compile: gcc -Wall -o q21 q21_padding.c
 */
#include <stdio.h>
#include <string.h>

#define BLOCK 8   /* bytes per block for the demo */

/* Pads msg of length len into out. Returns the padded length. */
static size_t pad(const unsigned char *msg, size_t len, unsigned char *out) {
    memcpy(out, msg, len);
    size_t padded = (len / BLOCK + 1) * BLOCK;   /* always at least one pad byte */
    out[len] = 0x80;                              /* the 1 bit */
    for (size_t i = len + 1; i < padded; i++)
        out[i] = 0x00;                            /* the zero bits */
    return padded;
}

/* Removes padding. Returns the original length, or 0 if padding is invalid. */
static size_t unpad(const unsigned char *in, size_t len) {
    for (size_t i = len; i-- > 0; ) {
        if (in[i] == 0x80) return i;
        if (in[i] != 0x00) return 0;
    }
    return 0;
}

int main(void) {
    const char *tests[] = {"HELLO", "EIGHTBYT", "A"};
    for (int t = 0; t < 3; t++) {
        const unsigned char *msg = (const unsigned char *)tests[t];
        size_t len = strlen(tests[t]);
        unsigned char out[32];
        size_t plen = pad(msg, len, out);
        size_t orig = unpad(out, plen);

        printf("Message (%zu bytes): \"%s\"\n", len, tests[t]);
        printf("  padded to %zu bytes: ", plen);
        for (size_t i = 0; i < plen; i++) printf("%02X ", out[i]);
        printf("\n  recovered length: %zu %s\n\n", orig, orig == len ? "(ok)" : "(ERROR)");
    }
    return 0;
}
