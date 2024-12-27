//src/main.c
//2024-12-25, Dmitry ACHKASOV <achkasov.dmitry+xs@live.com>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utf8.h>
#include <termios.h>
#include <unistd.h>
#include "cfg.h"
#include "llist.h"
#include "table.h"

void enable_raw_mode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Get current terminal settings
    tty.c_lflag &= ~(ICANON); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Apply new settings immediately
}

void disable_raw_mode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Get current terminal settings
    tty.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Apply new settings immediately
}

enum Mode {
    MODE_CMD,
    MODE_INSERT
};


typedef struct {
    enum Mode mode;
    uint cur_row;
    uint cur_col;
    bool modified;
    Column *tbl_head;
} State;


int main([[maybe_unused]]int argc, [[maybe_unused]]char **argv) {
    fprintf(stderr, "INFO: Initialising the file state... ");
    Column* tbl = tbl_new_empty();
    State state = {
        .mode = MODE_CMD,
        .cur_row = 0,
        .cur_col = 0,
        .modified = false,
        .tbl_head = tbl,
    };

    char cmd[256];
    char buf[1024] = {0};
    fprintf(stderr, "Success\n");


    for(;;) {
        cmd[0] = '\0';
        buf[0] = '\0';
        fprintf(stderr, "INFO: Entering switch().\n");
        switch (state.mode) {
           case MODE_CMD: {
                fprintf(stderr, "Success: Entering command mode\n");
                disable_raw_mode();
                printf(CFG_PROMPT);
                fprintf(stderr, "INFO: Starting reading the prompt... ");
                void *err = fgets(cmd, sizeof(cmd), stdin);
                if (err == NULL) {
                    fprintf(stderr, "Failure:: Unable to read string. Terminating.\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(stderr, "Success\n");

                cmd[utf8cspn(cmd, "\n")] = '\0';    //remove new line
                printf("\n");
                if (utf8cmp(cmd, "q") == 0) {
                    fprintf(stderr, "INFO: Selected 'q'. Exiting the app.\n");
                    exit(EXIT_SUCCESS);
                } 
                else if (utf8cmp(cmd, ".") == 0) {
                    fprintf(stderr, "INFO: Selected '.'. Changing mode to CMD.\n");
                    state.mode = MODE_CMD;
                } 
                else if (utf8cmp(cmd, "r") == 0) {
                    // TODO: replace current cell
                    fprintf(stderr, "INFO: Selected 'r'. Changing mode to INSERT.\n");
                    state.mode = MODE_INSERT;
                } 
                else if (utf8cmp(cmd, "a") == 0) {
                    // TODO: append a.cur_row to each.cur_column 
                    // below the current one and insert into it
                    fprintf(stderr, "INFO: Selected 'a'. inserting a new row... ");
                    state.tbl_head = tbl_row_ins(state.tbl_head, state.cur_row);
                    fprintf(stderr, "Success\n");

                    fprintf(stderr, "INFO: Incrementing the row number. Changing mode to INSERT.\n");
                    state.cur_row++;
                    state.mode = MODE_INSERT;
                }
                else if(utf8cmp(cmd, "dbg") == 0) {
                    uint col_cnt = node_len(state.tbl_head);
                    uint row_cnt = node_len(state.tbl_head->data);
                    printf("Total columns: %d, Column A length: %d", col_cnt, row_cnt);
                }
                else if (utf8cmp(cmd, "p") == 0) {
                    fprintf(stderr, "INFO: Selected 'p'.\n");
                    fprintf(stderr, "INFO: Getting current cell...\n");
                    Cell* cur_cell = tbl_cell_get_raw(state.tbl_head, state.cur_row, state.cur_col);
                    if(cur_cell != NULL) {
                        fprintf(stderr, "Success: it exists\n");
                        fprintf(stderr, "INFO: Getting current cell data... ");
                        if(cur_cell->data != NULL) {
                            fprintf(stderr, "Success: It has data\n");
                            fprintf(stderr, "INFO: cloning the text data into the temp buffer using utf8cpy()... "); 
                            utf8cpy(buf, cur_cell->data);
                            fprintf(stderr, "Success\n");
                        }
                    }
                    fprintf(stderr, "INFO: Printing the cell data.\n");
                    printf("%c%i: %s\n", state.cur_col + 'A', state.cur_row + 1, buf);
                }
                else if (utf8cmp(cmd, "Ap") == 0) {
                    fprintf(stderr, "INFO: Selected 'Ap'.\n");
                    fprintf(stderr, "INFO: Getting current cell...\n");
                    Column* col_A = state.tbl_head;
                    if(col_A == NULL) {
                        printf("\t-NULL-\n");
                    } else {
                        printf("\t%c\n", 0 + 'A');
                    }
                    Cell* cur_cell = col_A->data;
                    uint idx = 0;
                    while(cur_cell != NULL) {
                        if(cur_cell->data != NULL) {
                            utf8cpy(buf, cur_cell->data);
                        }
                        printf("%7d\t%.8s\n", idx + 1, buf);
                        idx++;
                        cur_cell = cur_cell->next;
                    }
                    if(cur_cell == NULL) {
                        printf("\t-NULL-\n");
                    }
                }
                else  {
                    fprintf(stderr, "WARN: Unknown command entered.\n");
                    printf("?\n");
                }
                break;
            }

            case MODE_INSERT: {
                fprintf(stderr, "Success: Entering insert mode\n");
                enable_raw_mode();
                uint buf_idx = 0;
                uint col_offset = 0;
                buf[0] = '\0';
                char c = '\0';
                char prev_c = '\0';
                bool end_of_input = false;
                while(!end_of_input) {
                    prev_c = c;
                    c = getchar();
                    fprintf(stderr, "INFO: getchar(): '%c'\n", c);
                    if ((prev_c == CFG_EOL_TOKEN) && (c == '\n')) {
                        fprintf(stderr, "INFO: EOL identified. Current buf[]: ");
                        buf[buf_idx] = '\0';      // replace '\n' with '\0'
                        fprintf(stderr, "%s\n", buf);
                        buf_idx--;
                        end_of_input = true;
                        fflush(stdout);
                        disable_raw_mode();
                    } else
                    if ((c == '\n') || (c == '\t')) {
                        //End of row
                        fprintf(stderr, "INFO: End of cell [%d,%d] identified.\n", state.cur_row, state.cur_col + col_offset);
                        buf[buf_idx] = '\0';
                        char* cell_data = (char*)calloc(buf_idx, sizeof(char));
                        fprintf(stderr, "INFO: calloc() succesfull\n");
                        fprintf(stderr, "INFO: utfncpy(\"%s\") to cell_data\n", buf);
                        utf8ncpy(cell_data, buf, buf_idx);
                        fprintf(stderr, "INFO: utfncpy() succesfull\n");
                        tbl_cell_data_set(state.tbl_head, state.cur_row, state.cur_col + col_offset, cell_data);
                        fprintf(stderr, "INFO: tbl_cell_data_set(): TABLE[%d,%d] <= \"%s\"\n", state.cur_row, state.cur_col + col_offset, cell_data);
                        buf_idx = 0;

                        switch(c) {
                            case '\n': {
                                
                                col_offset = 0;
                                state.cur_row++;
                                break;
                            }

                            case '\t': {
                                col_offset++;
                                break;
                            }

                            default: { 
                                unreachable();
                                break;
                            }
                        }
                        buf_idx = 0;
                        col_offset = 0;
                        state.cur_row++;
                    }
                    else {
                        fprintf(stderr, "INFO: Setting buf[%d]: %c\n", buf_idx, c);
                        buf[buf_idx] = c;
                        buf_idx++;
                    }
                }
                state.modified = true;
                state.mode = MODE_CMD;
                fprintf(stderr, "INFO: Exiting insert mode.\n");
                disable_raw_mode();
                break;
            }
        }
    }
    printf("\n");
    fprintf(stderr, "INFO: Exiting the app\n");
    exit(EXIT_SUCCESS);
} 


