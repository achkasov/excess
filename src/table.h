//src/tbl.h: Column manupilation functions 
//2024-12-25, Dmitry ACHKASOV <achkasov.dmitry+xs@live.com>


#pragma once
#include "llist.h"


// [ [Col_A->next] = [Col_B->next] = [Col_C->next] > ... > NULL ]
// [ [     ->data]   [     ->data]   [     ->data]              ]
//            =               =               =                 
//   [CelA1->next]   [CelB1->next]           NULL               
//            =               =                           
//   [CelA2->next]   [CelB2->next]                        
//            =               =
//           ...             NULL
//            =
//           NULL

typedef Node Cell;    // A cell is just a node that holds text data (i.e. Cell = Node)
typedef Node Column;  // Column's data is a linked list of Cells (i.e. Column = LinkedList<Cell>)

//All functions return the ptr to the first column of the tbl

///Creates new empty table. Return pointer to the first column
Column* tbl_new_empty() {
    Column* tbl_head = (Column*)node_new(NULL);
    return tbl_head;
}


Column* tbl_col_ins(Column* tbl_head, uint idx) {
    Column* new_col = (Column*)node_new(NULL);
    Column* new_tbl_head = (Column*)node_ins(tbl_head, new_col, idx);
    return new_tbl_head;
}


Column* tbl_col_rem(Column* tbl_head, uint idx) {
    Column* chosen_col = (Column*)node_get(tbl_head, idx);
    Cell* cell = (Cell*)(chosen_col->data);
    //First need to free() all child cells
    //TODO: Ideally each column should have its own arena so it can be freed at once
    while(cell != NULL) {
        if (cell->data != NULL) {
            free(cell->data);
        }
        void* garbage = cell;
        cell = cell->next;
        free(garbage);
    }
    Column* new_tbl_head = (Column*)node_rem(tbl_head, idx);
    return new_tbl_head;
}


//Grows the tbl count if there is not enough column. Returns the first column ptr
Column* tbl_col_grow(Column* tbl_head, uint new_size) {
    //The table must exist.
    if(tbl_head == NULL) {
        tbl_head = tbl_new_empty();
    }

    int extra_col_cnt = (int)new_size - (int)node_len(tbl_head);
    if(extra_col_cnt <= 0) {
        return tbl_head;
    }

    node_append_empty_n(tbl_head, extra_col_cnt);
    return tbl_head;
}


//Grows the cells count in a column if there is not enough. Returns the head cell
Cell* tbl_col_row_grow(Column* col, uint new_size) {
    assert(col != NULL);    //The column must exist.
    Cell* cell_head = col->data;
    if(new_size == 0) {
        assert(cell_head != NULL);
        return cell_head; 
    }

    //The column doesn't have rows? Make a the head row first!
    if(cell_head == NULL) {
        cell_head = (Cell*)node_new(NULL);
        assert(cell_head != NULL);
        col->data = cell_head;
    }

    int extra_row_cnt = (int)new_size - (int)node_len(cell_head);
    if(extra_row_cnt <= 0) {
        assert(cell_head != NULL);
        return cell_head;
    }

    assert(cell_head != NULL);
    node_append_empty_n(cell_head, extra_row_cnt);
    assert(cell_head != NULL);
    return cell_head;
}


Column* tbl_row_ins(Column* tbl, uint idx) {
    assert(!"Unimplemented");
}


Column* tbl_row_rem(Column* tbl, uint idx) {
    assert(!"Unimplemented");
}


//Returns the Column ptr or NULL
Column* tbl_col_get_raw(Column* tbl, uint col) {
    return (Column*)node_get(tbl, col);
}


//Returns the column ptr and initialises it if needed.
Column* tbl_col_get(Column* tbl_head, uint col) {
    Column* new_tbl_head = tbl_col_grow(tbl_head, col - 1);
    Column* selected_col = tbl_col_get_raw(new_tbl_head, col);
    assert(selected_col != NULL);
    return selected_col;
}


//Returns the Cell ptr or NULL
Cell* tbl_cell_get_raw(Column* tbl_head, uint row, uint col) {
    Column* col_cur = (Column*)node_get(tbl_head, col);
    if(col_cur == NULL) {
        return NULL;
    }

    Cell* cell_head = col_cur->data;
    if(cell_head == NULL) {
        return NULL;
    }

    Cell* cell_cur = (Cell*)node_get(cell_head, row);
    return cell_cur;
}


//Returns the cell ptr, if needed initialises it.
Cell* tbl_cell_get(Column* tbl_head, uint row, uint col) {
    Column* selected_col = tbl_col_get(tbl_head, col);
    assert(selected_col != NULL);

    Cell* cell_head = tbl_col_row_grow(selected_col, row + 1);
    assert(cell_head != NULL);

    Cell* selected_cell = tbl_cell_get_raw(tbl_head, row, col);
    assert(selected_cell != NULL);
    return selected_cell;
}


//Returns newly edited cell
Cell* tbl_cell_data_set(Column* tbl_head, uint row, uint col, void* data) {
    Cell* selected_cell = tbl_cell_get(tbl_head, row, col);
    selected_cell->data = data;
    return selected_cell;
}




