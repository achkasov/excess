//src/llist: Linked list implementation
//We're reïnventing wheels here
//2024-12-25, Dmitry ACHKASOV <achkasov.dmitry+xs@live.com>

#pragma once
#include <stdio.h>
#include <assert.h>


typedef struct Node {
    void* data;
    struct Node* next;
} Node;


///Returns ptr to a Node
Node* node_new(void* data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

///Returns ptr to a Node
Node* node_new_empty(void) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = NULL;
    new_node->next = NULL;
    return new_node;
}


///Returns pointer to the last element
Node* node_last(Node* head) {
    Node* node = head;
    assert(head != NULL);
    while (node->next != NULL) {
        node = node->next;
    }
    return node;
}


///Returns element pointer at the specified index
Node* node_get(Node* head, uint idx) {
    Node* node = head;
    for (uint i=0; i<idx; i++) {
        if(node == NULL) {
            return NULL;
        }
        node = node->next;
    }
    return node;
}


///Returns length of the list
uint node_len(Node* head) {
    Node* node = head;
    uint len = 0;
    while (node->next != NULL) {
        node = node->next;
        len++;
    }
    return len;
}


///Returns inserted element pointer
Node* node_append(Node* head, Node* new_node) {
    Node* last = node_last(head);
    last->next = new_node;
    new_node->next = NULL;
    return new_node;
}


///Returns last element pointer
Node* node_append_empty_n(Node *head, uint count) {
    Node* last = node_last(head);
    while(count > 0) {
        Node* new_empty_node = node_new_empty();
        last = node_append(last, new_empty_node);
        count--;
    }
    return last;
}


///Returns the head to the updated list
Node* node_rem(Node *head, uint idx) {
    // [head... before <deletion> after...]
    uint last_idx = node_len(head) - 1;
    int overrun = idx - last_idx;
    if (overrun > 0) {
        //Nothing to delete
        return head;
    }

    if (idx == 0) {
        Node* new_head = head->next;
        assert(head!=NULL);
        free(head);
        head = NULL;
        return new_head;
    }

    Node* before = node_get(head, idx-1);
    assert(before != NULL);
    Node* deletion = before->next;
    if (deletion == NULL) {
        return head;
    }

    Node* after = deletion->next;
    before->next = after;
    return head;
}


///Returns the head to the updated list
Node* node_ins(Node* head, Node* new_node, uint idx) {
    // [head... before <insertion> after...]
    uint last_idx = node_len(head) - 1;
    int overrun = idx - last_idx;
    if (overrun > 0) {
        node_append_empty_n(head, overrun);
    }

    if (idx == 0) {
        Node* new_head = new_node;
        new_head->next = head;
        return new_head;
    }
    else {
        Node* before = node_get(head, idx-1);
        assert(before != NULL);
        Node* after = before->next;
        Node* inserted = node_append(before, new_node);
        inserted->next = after;
        return head;
    }
}



