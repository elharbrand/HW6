#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "org_tree.h"

#define FP_LEN 9


static void print_success(int mask, char *op, char* fingerprint, char* First_Name, char* Second_Name)
{
    printf("Successful Decrypt! The Mask used was mask_%d of type (%s) and The fingerprint was %.*s belonging to %s %s\n",
                       mask, op, FP_LEN, fingerprint, First_Name, Second_Name);
}

static void print_unsuccess()
{
    printf("Unsuccesful decrypt, Looks like he got away\n");
}

int main(int argc, char **argv) {
    FILE *f;
    char line[32];
    unsigned char cipher[FP_LEN];
    int start_mask;
    int i, mask;
    Org org;
    Node *n, *s;

    if (argc != 4) {
        printf("Usage: %s <clean_file.txt> <cipher_bits.txt> <mask_start_s>\n", argv[0]);
        return 0;
    }

    f = fopen(argv[2], "r");
    if (!f) {
    printf("Error opening file: %s\n", argv[2]);
    return 0;
    }

    for (i = 0; i < FP_LEN; i++) {
        unsigned char val = 0;
        if (!fgets(line, sizeof(line), f)) {
            fclose(f);
            return 0;
        }
        for (int b = 0; b < 8; b++) {
            val = (val << 1) | (line[b] == '1');
        }
        cipher[i] = val;
    }
    fclose(f);

    start_mask = atoi(argv[3]);
    org = build_org_from_clean_file(argv[1]);

    for (mask = start_mask; mask <= start_mask + 10; mask++) {
        for (int op = 0; op < 2; op++) {
            Node *people[3] = { org.boss, org.left_hand, org.right_hand };

            for (int p = 0; p < 3; p++) {
                n = people[p];
                if (!n) continue;

                int ok = 1;
                for (i = 0; i < FP_LEN; i++) {
                    unsigned char plain = (unsigned char)n->fingerprint[i];
                    unsigned char enc = op ? (plain ^ mask) : (plain & mask);
                    if (enc != cipher[i]) {
                        ok = 0;
                        break;
                    }
                }
                if (ok) {
                    print_success(mask, op ? "XOR" : "AND",
                                  n->fingerprint, n->first, n->second);
                    free_org(&org);
                    return 0;
                }
                
                s = n->supports_head;
                while (s) {
                    ok = 1;
                    for (i = 0; i < FP_LEN; i++) {
                        unsigned char plain = (unsigned char)s->fingerprint[i];
                        unsigned char enc = op ? (plain ^ mask) : (plain & mask);
                        if (enc != cipher[i]) {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok) {
                        print_success(mask, op ? "XOR" : "AND",
                                      s->fingerprint, s->first, s->second);
                        free_org(&org);
                        return 0;
                    }
                    s = s->next;
                }
            }
        }
    }

    print_unsuccess();
    free_org(&org);
    return 0;
}
