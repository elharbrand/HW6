#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "org_tree.h"

Org build_org_from_clean_file(const char *path)
{
    Org org = { NULL, NULL, NULL };
    FILE *f = fopen(path, "r");
    
    if (!f) {
        printf("Error opening file: %s\n", path);
        Org empty = { NULL, NULL, NULL };
        return empty;
    }

    while (1) {
        Node *n = malloc(sizeof(Node));
        if (!n) break;

        if (fscanf(f,
            "First Name: %127s\n"
            "Second Name: %127s\n"
            "Fingerprint: %127s\n"
            "Position: %31[^\n]\n\n",
            n->first, n->second, n->fingerprint, n->position) != 4)
        {
            free(n);
            break;
        }

        n->left = NULL;
        n->right = NULL;
        n->supports_head = NULL;
        n->next = NULL;

        if (strcmp(n->position, "Boss") == 0) {
            org.boss = n;
        }
        else if (strcmp(n->position, "Left Hand") == 0) {
            org.left_hand = n;
            org.boss->left = n;
        }
        else if (strcmp(n->position, "Right Hand") == 0) {
            org.right_hand = n;
            org.boss->right = n;
        }
        else if (strcmp(n->position, "Support Left") == 0) {
            Node **p = &org.left_hand->supports_head;
            while (*p) p = &(*p)->next;
            *p = n;
        }
        else if (strcmp(n->position, "Support Right") == 0) {
            Node **p = &org.right_hand->supports_head;
            while (*p) p = &(*p)->next;
            *p = n;
        }
        else {
            free(n);
        }
    }
    fclose(f);
    return org;
}

static void print_node(const Node *n)
{
    printf("First Name: %s\n", n->first);
    printf("Second Name: %s\n", n->second);
    printf("Fingerprint: %s\n", n->fingerprint);
    printf("Position: %s\n\n", n->position);
}


void print_tree_order(const Org *org)
{
    Node *s;

    if (!org || !org->boss) return;

    print_node(org->boss);

    print_node(org->left_hand);
    for (s = org->left_hand->supports_head; s; s = s->next)
        print_node(s);

    print_node(org->right_hand);
    for (s = org->right_hand->supports_head; s; s = s->next)
        print_node(s);
}

void free_org(Org *org)
{
    Node *s, *tmp;

    if (org->left_hand) {
        for (s = org->left_hand->supports_head; s; s = tmp) {
            tmp = s->next;
            free(s);
        }
        free(org->left_hand);
    }

    if (org->right_hand) {
        for (s = org->right_hand->supports_head; s; s = tmp) {
            tmp = s->next;
            free(s);
        }
        free(org->right_hand);
    }

    free(org->boss);
}