//
// Created by luvjo on 2019/7/13.
//

#include <pcre.h>

#ifndef TRIGX_TREE_TRIGX_TREE_H
#define TRIGX_TREE_TRIGX_TREE_H
typedef struct _TrigxNode {
    int val;
    struct _TrigxNode *char_nodes[64];
    struct _TrigxNode *rgx_next;
    int rgx_raw_len;
    char *rgx;
} TrigxNode;

TrigxNode *create_trigx_node();

void trigx_insert(TrigxNode *root, const char *word, int len_word, int val);

int trigx_search(TrigxNode *root, const char *word, int len_word);

void trigx_free(TrigxNode *root);


#endif //TRIGX_TREE_TRIGX_TREE_H
