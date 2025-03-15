#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "list.h"

#define PROMPT >


int main() {
    cell_t* head = NULL;
    cell_t* cell = NULL;
    cell_t* prev = NULL;

    cell = malloc(sizeof(cell_t));
    assert(cell != NULL);
    cell->type = CT_INT;
    cell->data = (void*)(42);
    cell->next = NULL;
    head = cell;

    cell_print(cell, 8);


    for (size_t i=0; i<10; ++i) {
        prev = cell;
        cell = NULL;
        if ((cell = malloc(sizeof(cell_t))) == NULL) exit(EXIT_FAILURE);
        cell->type = CT_INT;
        cell->data = (void*) i;
        cell->next = NULL;
        prev->next = cell;
    }


    cell = head;
    prev = NULL;
    for (;;) {
        if (cell == NULL) break;
        cell_print(cell, 8);
        printf("\n");
        cell = cell->next;
    }


    int idx = 0;
    cell = cells_get(head, idx);
    printf("cell[%d]: ", idx);
    cell_print(cell, 8);
    printf("\n");

    cells_free(head);

    return EXIT_SUCCESS;
}
