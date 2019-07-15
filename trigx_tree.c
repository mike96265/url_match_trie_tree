#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <pcre.h>
#include "trigx_tree.h"

#ifndef DEFAULT_STACK_SIZE
#define DEFAULT_STACK_SIZE 30
#endif

#ifndef DEFAULT_STACK_RESIZE
#define DEFAULT_STACK_RESIZE 20
#endif


#define MEMORY_ERROR_HANDLER  { perror("no memory available.");exit(1);}

#define CREATE_STACK(TYPE, ITEM_TYPE) \
{ \
    TYPE *stack = (TYPE *) malloc(sizeof(TYPE)); \
    if (stack == NULL) { \
        goto MEMORY_ERROR; \
    } \
    ITEM_TYPE *items = (ITEM_TYPE *) malloc(sizeof(ITEM_TYPE) * DEFAULT_STACK_SIZE); \
    if  (items == NULL) { \
        goto MEMORY_ERROR; \
    } \
    stack->items = items; \
    stack->top = -1; \
    stack->size = DEFAULT_STACK_SIZE; \
    return stack; \
    MEMORY_ERROR: \
    MEMORY_ERROR_HANDLER \
}

#define STACK_RESIZE(TYPE, ITEM_TYPE) \
{ \
    int current_size = stack->size; \
    int new_size = inc ? (current_size + DEFAULT_STACK_RESIZE) : (current_size - DEFAULT_STACK_RESIZE); \
    ITEM_TYPE *new_items = (ITEM_TYPE *) malloc(sizeof(ITEM_TYPE) * new_size); \
    if (new_items == NULL) { \
        goto MEMORY_ERROR; \
    } \
    memcpy(new_items, stack->items, inc ? current_size : new_size); \
    free(stack->items); \
    stack->items = new_items; \
    stack->size = new_size; \
    return; \
    MEMORY_ERROR: \
    MEMORY_ERROR_HANDLER \
}

#define STACK_PUSH(TYPE, ITEM_TYPE, RESIZE) \
{ \
    int next_idx = stack->top + 1; \
    if (next_idx == stack->size) { \
        RESIZE(stack, true); \
    } \
    stack->items[next_idx] = item; \
    stack->top += 1; \
}

#define STACK_POP(TYPE, ITEM_TYPE, RESIZE) \
{ \
    int top = stack->top; \
    if (top == -1) { \
        goto STACK_ERROR; \
    } \
    ITEM_TYPE pop_item = stack->items[top]; \
    stack->top -= 1; \
    if (stack->size - stack->top == 30) { \
        RESIZE(stack, false); \
    } \
    return pop_item; \
    STACK_ERROR: \
    { \
        perror("pop from an empty stack."); \
        exit(1); \
    } \
}

// stack and istack declare here
typedef struct _Stack {
    int top;
    int size;
    void **items;
} Stack;

typedef struct _IStack {
    int top;
    int size;
    int *items;
} IStack;


// stack related method
static Stack *create_stack();

static void stack_push(Stack *stack, void *item);

static void *stack_pop(Stack *stack);

static void stack_free(Stack *stack);

// stack related method implementation
static Stack *create_stack() CREATE_STACK(Stack, void *)

static void stack_resize(Stack *stack, bool inc) STACK_RESIZE(Stack, void *)

static void stack_push(Stack *stack, void *item) STACK_PUSH(Stack, void *, stack_resize)

static void *stack_pop(Stack *stack) STACK_POP(Stack, void *, stack_resize);

static void stack_free(Stack *stack) {
    free(stack->items);
    free(stack);
}

// istack related method
static IStack *create_istack();

static void istack_push(IStack *stack, int item);

static int istack_pop(IStack *stack);

static void istack_free(IStack *stack);

//istack related method implementation
static IStack *create_istack() CREATE_STACK(IStack, int)

static void istack_resize(IStack *stack, bool inc) STACK_RESIZE(IStack, int)

static void istack_push(IStack *stack, int item) STACK_PUSH(IStack, int, istack_resize)

static int istack_pop(IStack *stack) STACK_POP(IStack, int, istack_resize);

static void istack_free(IStack *stack) {
    free(stack->items);
    free(stack);
}


// index of alphabet 0-9 .. a-z .. A-Z .. - .. /
int index_alphabet_char(char c) {
    int index = 0;
    if ('0' <= c && c <= '9') {
        return c - '0' + index;

    }
    index += 10;
    if ('a' <= c && c <= 'z') {
        return c - 'a' + index;
    }
    index += 26;
    if ('A' <= c && c <= 'Z') {
        return c - 'A' + index;
    }
    index += 1;
    if (c == '-') {
        return index;
    }
    index += 1;
    if (c == '/') {
        return index;
    }
    return -1;
}

TrigxNode *create_trigx_node() {
    TrigxNode *node = (TrigxNode *) malloc(sizeof(TrigxNode));
    node->rgx_raw_len = 0;
    node->rgx = NULL;
    node->val = -1;
    node->rgx_next = NULL;
    node->re = NULL;
    return node;
}

void trigx_insert(TrigxNode *root, const char *word, int len_word, int val) {
    TrigxNode *current = root;
    int idx_word = 0;
    while (idx_word < len_word) {
        char current_char = word[idx_word];
        if (current_char == '<') {
            int rgx_start_idx = idx_word + 1;
            int rgx_end_idx = rgx_start_idx;
            bool find = false;
            while (rgx_end_idx < len_word) {
                if (word[rgx_end_idx] == '>') {
                    find = true;
                    break;
                }
                rgx_end_idx += 1;
            }
            if (!find) {
                perror("invalid insert word.");
                return;
            }
            int rgx_raw_len = rgx_end_idx - rgx_start_idx + 1;
            char *rgx = (char *) malloc(sizeof(char) * rgx_raw_len);
            memcpy(rgx, word + rgx_start_idx, (rgx_raw_len - 1) * sizeof(char));
            rgx[rgx_raw_len - 1] = '\0';
            TrigxNode *rgx_node = NULL;
            bool exist = false;
            while (current->rgx_next) {
                if (current->rgx_next->rgx_raw_len == rgx_raw_len) {
                    int i = 0;
                    for (i = 0; i < rgx_raw_len; i++) {
                        if (current->rgx_next->rgx[i] != rgx[i]) {
                            break;
                        }
                    }
                    if (i == rgx_raw_len) {
                        exist = true;
                    }
                }
                if (exist) {
                    rgx_node = current->rgx_next;
                    break;
                }
                current = current->rgx_next;
            }
            if (!exist) {
                rgx_node = create_trigx_node();
                rgx_node->rgx = rgx;
                rgx_node->rgx_next = current->rgx_next;
                const char *error;
                int error_offset;
                rgx_node->re = pcre_compile(rgx_node->rgx, 0, &error, &error_offset, NULL);
                current->rgx_next = rgx_node;
            }
            current = rgx_node;
            idx_word = rgx_end_idx + 1;
        } else if (index_alphabet_char(current_char) != -1) {
            char char_idx = index_alphabet_char(current_char);
            TrigxNode *next_node = current->char_nodes[char_idx];
            if (next_node == NULL) {
                next_node = create_trigx_node();
                current->char_nodes[char_idx] = next_node;
            }
            current = next_node;
            idx_word += 1;
        } else {
            perror("invalid character.");
            exit(1);
        }
    }
    current->val = val;
}

int trigx_search(TrigxNode *root, const char *word, int word_len) {
    IStack *istack = create_istack();
    Stack *stack = create_stack();
    istack_push(istack, 0);
    stack_push(stack, root);
    while (stack->top != -1) {
        TrigxNode *node = (TrigxNode *) stack_pop(stack);
        int idx_word = istack_pop(istack);
        if (idx_word == word_len) {
            if (node->val != -1) {
                return node->val;
            } else {
                continue;
            }
        }
        char current_char = word[idx_word];
        int current_char_idx = index_alphabet_char(current_char);
        if (current_char_idx == -1) {
            perror("invalid character.");
            exit(1);
        }
        if (node->char_nodes[current_char_idx] != NULL) {
            stack_push(stack, node->char_nodes[current_char_idx]);
            istack_push(istack, idx_word + 1);
        }
        TrigxNode *current_rgx_node = node->rgx_next;
        while (current_rgx_node != NULL) {
            int vector[3];
            int rc = pcre_exec(current_rgx_node->re, NULL, word, word_len, idx_word, 0, vector, 3);
            if (rc < 0) {
                current_rgx_node = current_rgx_node->rgx_next;
            } else {
                if (vector[0] == idx_word) {
                    stack_push(stack, current_rgx_node);
                    istack_push(istack, vector[1]);
                    break;
                } else {
                    current_rgx_node = current_rgx_node->rgx_next;
                }
            }
        }
    }
    istack_free(istack);
    stack_free(stack);
    return -1;
}

void trigx_free(TrigxNode *root) {
    Stack *stack = create_stack();
    stack_push(stack, root);
    while (stack->top != -1) {
        TrigxNode *node = (TrigxNode *) stack_pop(stack);
        for (int i = 0; i < 64; i++) {
            if (node->char_nodes[i] != NULL) {
                stack_push(stack, node->char_nodes[i]);
            }
        }
        if (node->rgx_next != NULL) {
            stack_push(stack, node->rgx_next);
        }
        if (node->rgx_raw_len != 0) {
            free(node->rgx);
            pcre_free(node->re);
        }
    }
    stack_free(stack);
}
