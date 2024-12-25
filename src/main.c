//src/main.c
//  2024-12-25, Dmitry ACHKASOV <achkasov.dmitry+xs@live.com>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utf8.h>
#include "cfg.h"
#include "llist.h"
#include "table.h"

enum Mode {
    MODE_CMD,
    MODE_INSERT
};


struct State {
    enum Mode mode;
    uint cur_row;
    uint cur_col;
    bool modified;
    Column *tbl_head;
};


int read_input(char *buf) {
    uint i = 0;
    for(;;) {
        char c = getchar();
        if (c == '\n') {
            putchar('\n');
            if (i > 0 &&buf[i-1] == '.') {
                buf[i-1] = '\0';
                return i;
            };
        }
        buf[i] = c;
        i++;
    }
}


int main([[maybe_unused]]int argc, [[maybe_unused]]char **argv) {
    Column* tbl = tbl_new_empty();

    struct State state = {
        .mode = MODE_CMD,
        .cur_row = 0,
        .cur_col = 0,
        .modified = false,
        .tbl_head = tbl,
    };

    char cmd[256];
    uint cmd_idx = 0;
    char buf[1024];
    uint buf_idx = 0;


    for(;;) {
        printf("INFO: Entering switch()\n");
        switch (state.mode) {
            case MODE_CMD: {
                printf("INFO: Entering command mode.\n");

                printf(CFG_PROMPT);
                printf("INFO: Starting reading the prompt\n");
                void *err = fgets(cmd, sizeof(cmd), stdin);
                if (err == NULL) {
                    printf("ERROR: Unable to read string\n");
                    exit(EXIT_FAILURE);
                }
                printf("INFO: Success\n");

                
                cmd[utf8cspn(cmd, "\n")] = '\0';    //remove new line

                printf("\n");
                if (utf8cmp(cmd, "q") == 0) {
                    exit(EXIT_SUCCESS);
                } 
                else if (utf8cmp(cmd, ".") == 0) {
                     state.mode = MODE_CMD;
                } 
                else if (utf8cmp(cmd, "i") == 0) {
                    // TODO: insert into current cell
                    state.mode = MODE_INSERT;
                } 
                else if (utf8cmp(cmd, "a") == 0) {
                    // TODO: append a.cur_row to each.cur_column 
                    // below the current one and insert into it
                    state.tbl_head = tbl_row_ins(state.tbl_head, state.cur_row);
                    state.cur_row++;
                    state.mode = MODE_INSERT;
                }
                else if(utf8cmp(cmd, "dbg") == 0) {
                    uint col_cnt = node_len(state.tbl_head);
                    uint row_cnt = node_len(state.tbl_head->data);
                    printf("Total columns: %d, Column A length: %d", col_cnt, row_cnt);
                }
                else if (utf8cmp(cmd, "p") == 0) {
                    Cell* cur_cell = tbl_cell_get_raw(state.tbl_head, state.cur_row, state.cur_col);
                    if(cur_cell != NULL) {
                        if(cur_cell->data != NULL)
                        {
                            utf8cpy(buf, cur_cell->data);
                        }
                    }
                    printf("%c%i: %s\n", state.cur_col + 'A', state.cur_row + 1, buf);
                } 
                else  {
                    printf("?\n");
                }
                break;
            }
            case MODE_INSERT: { 
                // printf("INFO: Entering insert mode.\n");

                // printf("INFO: Reading input...\n");
                int buf_len = read_input(buf);
                // printf("INFO: Success\n");

                // printf("INFO: Calling calloc()...\n");
                char* text = (char*)calloc(buf_len, sizeof(char));
                // printf("INFO: Success\n");

                // printf("INFO: utf8ncpy()...\n");
                utf8ncpy(text, buf, buf_len);
                // printf("INFO: Success\n");

                // printf("INFO: Getting cell (potentially allocating)...\n");
                Cell* cur_cell = tbl_cell_get(state.tbl_head, state.cur_row, state.cur_col);
                assert(cur_cell != NULL);
                printf("INFO: Success\n");
               
                printf("INFO: assigning cur_cell->data...\n");
                cur_cell->data = (void*)text;
                printf("INFO: Success\n");

                printf("INFO: assigning state.modified...\n");
                state.modified = true;
                printf("INFO: Success\n");

              
                printf("INFO: incrementing state.cur_row...\n");
                state.cur_row++;
                printf("INFO: Success\n");

                printf("INFO: assigning state.mode...\n");
                state.mode = MODE_CMD;
                printf("INFO: Success\n");

                printf("INFO: Exiting insert mode.\n");
                break;
            }
        }
    }
    printf("\n");
    exit(EXIT_SUCCESS);
} 


