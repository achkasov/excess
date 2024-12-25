//src/main.c
//  Dmitry ACHKASOV <achkasov.dmitry@live.com>
//  2024-12-23

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
    Table *table;
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
    Table* tbl = table_new_empty();

    struct State state = {
        .mode = MODE_CMD,
        .cur_row = 0,
        .cur_col = 0,
        .modified = false,
        .table = tbl,
    };

    char cmd[256];
    uint cmd_idx = 0;
    char buf[1024];
    uint buf_idx = 0;


    for(;;) {
        switch (state.mode) {
            case MODE_CMD: {
                printf(CFG_PROMPT);
                void *err = fgets(cmd, sizeof(cmd), stdin);
                if (err == NULL) {
                    printf("ERROR: Unable to read string\n");
                    exit(EXIT_FAILURE);
                }
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
                    state.table = table_row_ins(state.table, state.cur_row);
                    state.cur_row++;
                    state.mode = MODE_INSERT;
                } 
                else if (utf8cmp(cmd, "p") == 0) {
                    printf("%c%i\n", state.cur_col + 'A', state.cur_row + 1);
                } 
                else  {
                    printf("?\n");
                }
                break;
            }
            case MODE_INSERT: {  
                int buf_len = read_input(buf);
                char* text = (char*)calloc(sizeof(char) * buf_len);
                utf8ncpy(text, buf, buf_len);


                state.modified = true;
                state.cur_row++;
                state.mode = MODE_CMD;
                break;
            }
        }
    }
    printf("\n");
    exit(EXIT_SUCCESS);
} 


