/*
 * Q13. Write a C program for Hill cipher succumbs to a known plaintext
 *      attack if sufficient plaintext-ciphertext pairs are provided. It is
 *      even easier to solve the Hill cipher if a chosen plaintext attack
 *      can be mounted.
 *
 * Approach: for a 2x2 Hill cipher, C = K * P (mod 26). Two plaintext
 * blocks (4 letters) and their ciphertext blocks give the matrices P and C.
 * If P is invertible mod 26, then K = C * P^-1 (mod 26).
 *
 * Compile: gcc -Wall -o q13 q13_hill_kpa.c
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int mod26(int x) { return ((x % 26) + 26) % 26; }

static int inv26(int a) {
    a = mod26(a);
    for (int x = 1; x < 26; x++)
        if ((a * x) % 26 == 1)
            return x;
    return -1;
}

static void matmul2(const int A[2][2], const int B[2][2], int R[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            int s = 0;
            for (int k = 0; k < 2; k++)
                s += A[i][k] * B[k][j];
            R[i][j] = mod26(s);
        }
}

int main(void) {
    char pt[16], ct[16];
    int p[4], c[4];

    printf("Enter 4 plaintext letters: ");
    if (scanf("%15s", pt) != 1 || strlen(pt) != 4) { printf("Need 4 letters.\n"); return 1; }
    printf("Enter the matching 4 ciphertext letters: ");
    if (scanf("%15s", ct) != 1 || strlen(ct) != 4) { printf("Need 4 letters.\n"); return 1; }

    for (int i = 0; i < 4; i++) {
        p[i] = toupper((unsigned char)pt[i]) - 'A';
        c[i] = toupper((unsigned char)ct[i]) - 'A';
        if (p[i] < 0 || p[i] > 25 || c[i] < 0 || c[i] > 25) {
            printf("Letters only.\n");
            return 1;
        }
    }

    /* Columns are the blocks (p0,p1) and (p2,p3). */
    int P[2][2] = {{p[0], p[2]}, {p[1], p[3]}};
    int C[2][2] = {{c[0], c[2]}, {c[1], c[3]}};

    int det = mod26(P[0][0] * P[1][1] - P[0][1] * P[1][0]);
    int det_inv = inv26(det);
    if (det_inv < 0) {
        printf("P is not invertible mod 26 (det=%d). Use independent blocks.\n", det);
        return 1;
    }

    int Pinv[2][2];
    Pinv[0][0] = mod26(det_inv * P[1][1]);
    Pinv[0][1] = mod26(-det_inv * P[0][1]);
    Pinv[1][0] = mod26(-det_inv * P[1][0]);
    Pinv[1][1] = mod26(det_inv * P[0][0]);

    int K[2][2], check[2][2];
    matmul2(C, Pinv, K);

    printf("Recovered key matrix K (mod 26):\n");
    printf("  | %2d %2d |\n  | %2d %2d |\n", K[0][0], K[0][1], K[1][0], K[1][1]);

    matmul2(K, P, check);
    int ok = (memcmp(check, C, sizeof(C)) == 0);
    printf("Verification: %s\n", ok ? "K*P == C (mod 26)" : "FAILED");
    return 0;
}
