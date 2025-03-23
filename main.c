// main.c


#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <time.h>

#include "list.h"
#include "table.h"

#define PROMPT >

int main(void)
{
    list_t* tbl = list_alloc_named("Table");
    tbl->meta.child_tag = NT_LIST;

    char col_name_buf[256];
    for (int i = 0; i < 10; i += 1) {
        sprintf(col_name_buf, "Col%i", i);
        list_t* rows = list_alloc_named(col_name_buf);
        node_t* col = node_alloc_list(rows);
        tbl = list_append(tbl, col);
    }

    list_t* col1 = list_alloc_named("#");
    node_t* col1_node = node_alloc_list(col1);
    tbl = list_insert_at(tbl, col1_node, 11);
    for (int i = 0; i < 20; i += 1) {
        node_t* node = node_alloc_int((ssize_t)i);
        col1 = list_append(col1, node);
    }

    tbl_print(tbl, 5);

    list_free(tbl);
    return EXIT_SUCCESS;
}
