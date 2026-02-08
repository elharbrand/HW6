#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PEOPLE 5000
#define FIELD_SIZE 256

typedef struct {
    char first[FIELD_SIZE];
    char last[FIELD_SIZE];
    char fp[FIELD_SIZE];
    char pos[FIELD_SIZE];
} Person;

void trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    int len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = 0;
}

int value_len_until_label(char *start) {
    char *labels[] = {
        "First Name:",
        "Second Name:",
        "Fingerprint:",
        "Position:"
    };
    int min = strlen(start);

    for (int i = 0; i < 4; i++) {
        char *p = strstr(start, labels[i]);
        if (p && p > start) {
            int d = (int)(p - start);
            if (d < min) min = d;
        }
    }
    return min;
}

int priority(char *s) {
    if (strstr(s, "Boss")) return 1;
    if (strstr(s, "Right Hand")) return 2;
    if (strstr(s, "Left Hand")) return 3;
    if (strstr(s, "Support_Right") || strstr(s, "Support Right")) return 4;
    if (strstr(s, "Support_Left") || strstr(s, "Support Left")) return 5;
    return 6;
}

int main(int argc, char **argv) {
    if (argc != 3) return 0;

    FILE *in = fopen(argv[1], "r");
    if (!in) return 0;

    fseek(in, 0, SEEK_END);
    long sz = ftell(in);
    fseek(in, 0, SEEK_SET);

    char *clean = malloc(sz + 1);
    if (!clean) { fclose(in); return 0; }

    int c, idx = 0;
    while ((c = fgetc(in)) != EOF) {
        if (!strchr("#?!&$@!", c) && c != '\n' && c != '\r')
            clean[idx++] = (char)c;
    }
    clean[idx] = 0;
    fclose(in);

    Person *list = malloc(sizeof(Person) * MAX_PEOPLE);
    if (!list) { free(clean); return 0; }

    int count = 0;
    char *p = clean;

    while ((p = strstr(p, "First Name:"))) {
        Person t;

        char *f = p + 11;
        char *s = strstr(f, "Second Name:");
        char *fi = strstr(f, "Fingerprint:");
        char *po = strstr(f, "Position:");

        if (!s || !fi || !po) break;

        /* First Name */
        int flen = (int)(s - f);
        if (flen >= FIELD_SIZE) flen = FIELD_SIZE - 1;
        strncpy(t.first, f, flen);
        t.first[flen] = 0;

        /* Second Name */
        char *sl = s + 12;
        int slen = (int)(fi - sl);
        if (slen >= FIELD_SIZE) slen = FIELD_SIZE - 1;
        strncpy(t.last, sl, slen);
        t.last[slen] = 0;

        
        char *fp_start = fi + 12;
        int filen = value_len_until_label(fp_start);
        if (filen >= FIELD_SIZE) filen = FIELD_SIZE - 1;
        strncpy(t.fp, fp_start, filen);
        t.fp[filen] = 0;

        char *ps = po + 9;
        int plen = value_len_until_label(ps);
        if (plen >= FIELD_SIZE) plen = FIELD_SIZE - 1;
        strncpy(t.pos, ps, plen);
        t.pos[plen] = 0;

        trim(t.first);
        trim(t.last);
        trim(t.fp);
        trim(t.pos);

        int dup = 0;
        for (int i = 0; i < count; i++) {
            if (!strcmp(list[i].fp, t.fp)) {
                dup = 1;
                break;
            }
        }

        if (!dup && count < MAX_PEOPLE)
            list[count++] = t;

        p = ps;
    }

    FILE *out = fopen(argv[2], "w");
    if (out) {
        for (int r = 1; r <= 5; r++) {
            for (int i = 0; i < count; i++) {
                if (priority(list[i].pos) == r) {
                    fprintf(out,
                        "First Name: %s\n"
                        "Second Name: %s\n"
                        "Fingerprint: %s\n"
                        "Position: %s\n\n",
                        list[i].first,
                        list[i].last,
                        list[i].fp,
                        list[i].pos);
                }
            }
        }
        fclose(out);
    }

    free(clean);
    free(list);
    return 0;
}