//src/table.h: Table manupilation functions 
//  Dmitry ACHKASOV <achkasov.dmitry@live.com>
//  2024-12-23


#pragma once
#include "llist.h"


// Table.next                               
//      =
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
typedef Node Column;  // Column is a linked list of Cells (i.e. Column = LinkedList<Cell>)
typedef Node Table;   // Table is a linked list of Columns (i.e. Table = LinkedList<Column>)

//All functions return the ptr to the first column of the table


Table* table_new_empty() {
    Cell* new_cell = (Cell*)node_new_empty();
    Column* new_column = (Column*)node_new(new_cell);
    Table* tbl = (Table*)new_column;
    return tbl;
}


Table* table_col_ins(Table* tbl, uint idx) {
    Cell* empty_cell = (Cell*)node_new(NULL);
    Column* empty_column = (Column*)node_new(empty_cell);
    Table* new_tbl_head = (Table*)node_ins(tbl, empty_column, idx);
    return new_tbl_head;
}


Table* table_col_rem(Table* tbl, uint idx) {
    //First need to free() all child cells
    //TODO: Ideally each column should have its own arena so it can be freed at once
    Column* chosen_col = (Column*)node_get(tbl, idx);
    Cell* cell = (Cell*)(chosen_col->data);
    while(cell != NULL) {
        if (cell->data != NULL) {
            free(cell->data);
        }
        void* garbage = cell;
        cell = cell->next;
        free(garbage);
    }
    Table* new_tbl_head = (Table*)node_rem(tbl, idx);
    return new_tbl_head;
}


Table* table_row_ins(Table* tbl, uint idx) {
    assert(!"Unimplemented");
}


Table* table_row_rem(Table* tbl, uint idx) {
    assert(!"Unimplemented");
}


Cell* table_cell_get(Table* tbl, uint row, uint col) {
    Column* col_cur = (Column*)node_get(tbl, col);
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





