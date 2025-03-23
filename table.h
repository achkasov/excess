// table.h

#ifndef TABLE_H_
#define TABLE_H_

#include <err.h>
#include <errno.h>
#include "list.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define TBL_MAX_TXT_BUF_SZ 256
#define TBL_COLS_MAX 1024
#define TBL_COL_SPACER " "
#define TBL_COL_PLACEHOLDER "NewCol"

/// Make all columns having the same number of rows
list_t* tbl_rows_align(list_t* tbl)
{
    size_t max_len = 0;
    size_t cur_len;

    FOREACH(cur, tbl)
    {
        cur_len = cur->data.list->len;
        max_len = MAX(max_len, cur_len);
    }

    list_t* cur_col;
    FOREACH(cur, tbl)
    {
        cur_col = cur->data.list;
        cur_col = list_resize(cur_col, node_empty, max_len);
    }

    return tbl;
}

/// r and c must be >0
list_t* tbl_alloc(const size_t r, const size_t c)
{
    assert((c > 0) && (r > 0));
    node_t node_template = {.tag = NT_LIST, .data.list = NULL, .next = NULL};

    list_t* tbl = list_alloc_n(node_template, c);
    if (tbl == NULL) return NULL;

    tbl->head->data.list = list_alloc_n(node_empty, r);
    if (tbl->head->data.list == NULL) return NULL;

    tbl = tbl_rows_align(tbl);
    if (tbl == NULL) return NULL;

    int i = 0;
    FOREACH(cur_tbl_col, tbl)
    {
        list_t* cur_col_list = cur_tbl_col->data.list;
        sprintf(cur_col_list->name, "C%i", i);
        i += 1;
    }

    return tbl;
}

list_t* tbl_insert_row_empty(list_t* tbl, const size_t row_idx)
{
    FOREACH(tbl_col, tbl)
    {
        list_t* col_list = tbl_col->data.list;
        col_list = list_insert_at(col_list, node_alloc(), row_idx);
        if (col_list == NULL) err(ENOMEM, "list_insert_at() failed");
    }
    return tbl;
}

list_t* tbl_insert_col_empty(list_t* tbl, const size_t col_idx)
{
    list_t* col_list = list_alloc_named(TBL_COL_PLACEHOLDER);
    if (col_list == NULL) err(ENOMEM, "list_alloc_named() failed");

    node_t* new_tbl_col = node_alloc_list(col_list);
    if (new_tbl_col == NULL) err(ENOMEM, "node_alloc_list() failed");

    tbl = list_insert_at(tbl, new_tbl_col, col_idx);
    if (tbl == NULL) err(ENOMEM, "list_insert_at() failed");

    tbl = tbl_rows_align(tbl);
    if (tbl == NULL) err(ENOMEM, "tbl_rows_align() failed");

    return tbl;
}

list_t* tbl_remove_row(list_t* tbl, const size_t row_idx)
{
    FOREACH(tbl_col, tbl)
    {
        list_t* col_list = tbl_col->data.list;
        col_list = list_remove(col_list, row_idx);
        if (col_list == NULL) err(EFAULT, "list_remove() failed");
    }
    return tbl;
}

list_t* tbl_remove_col(list_t* tbl, const size_t col_idx)
{
    tbl = list_remove(tbl, col_idx);
    if (tbl == NULL) err(ENOMEM, "tbl_list_remove() failed");

    tbl = tbl_rows_align(tbl);
    if (tbl == NULL) err(ENOMEM, "tbl_rows_align() failed");

    return tbl;
}

node_t* tbl_get_cell_ptr(list_t* tbl, const size_t row, const size_t col) 
{
    node_t* tbl_col = list_get(tbl, col);
    list_t* col_list = tbl_col->data.list;
    node_t* cell = list_get(col_list, row);
    return cell;
}

list_t* tbl_set_cell(list_t* tbl, node_data_t data, node_type_t nt, const size_t row, const size_t col)
{
    // TODO: check for NULLs
    node_t* cell = tbl_get_cell_ptr(tbl, row, col);
    cell->data = data;
    cell->tag = nt;
    return tbl;
}

void tbl_print(list_t* tbl, const size_t col_width)
{
    char tmp_buf[TBL_MAX_TXT_BUF_SZ];
    assert(col_width + 1 < TBL_MAX_TXT_BUF_SZ);
    memset(tmp_buf, ' ', TBL_MAX_TXT_BUF_SZ);

    node_t* cur_row[TBL_COLS_MAX] = {NULL};
    size_t col_count = tbl->len;
    tbl = tbl_rows_align(tbl);

    // Print headers
    node_t* cur_col = tbl->head;
    for (int i = 0; i < col_count; i += 1) {
        // Populate the rows lookup array to make iteration over lines easier in
        // the next loop
        cur_row[i] = cur_col->data.list->head;

        char* name = cur_col->data.list->name;
        snprintf(tmp_buf, col_width + 1, "%-*s", (int)col_width, name);
        tmp_buf[col_width + 1] = '\0';
        printf("%s%s", tmp_buf, TBL_COL_SPACER);
        cur_col = cur_col->next;
    }
    printf("\n");

    // Print rows
    size_t row_count = tbl->head->data.list->len;
    for (int r = 0; r < row_count; r += 1) {
        // Iterate over columns
        for (int c = 0; c < col_count; c += 1) {
            node_to_str(cur_row[c], tmp_buf, col_width + 1);
            cur_row[c] = cur_row[c]->next;
            printf("%s%s", tmp_buf, TBL_COL_SPACER);
        }
        printf("\n");
    }
}

#endif
