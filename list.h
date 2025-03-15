// list.h
#ifndef LIST_H_
#define LIST_H_


typedef enum {
    CT_EMPTY,
    CT_TEXT,
    CT_INT,
    CT_FLOAT,
    CT_DATETIME,
    CT_LAST       // Last type
} cell_type_t;


typedef struct cell_struct {
    cell_type_t type;
    void* data;
    void* next;
} cell_t;


void cell_print(const cell_t *c, const size_t width) {
    char *buffer;

    buffer = malloc((width+1) * sizeof(char));
    assert(buffer != NULL);
    buffer[0] = 0;

    size_t len = width;
    switch (c->type) {
    case CT_EMPTY:
        buffer[0] = "\0";
        break;
    case CT_TEXT:
        buffer = strncpy(buffer, c->data, width);
        break;
    case CT_INT:
        len = sprintf(buffer, "%*zd", (int)width, (ssize_t)c->data);
        break;
    case CT_FLOAT:
        len = sprintf(buffer, "%*.2f", (int)width, *(double*)c->data);
        break;
    case CT_DATETIME:
        struct tm *time_data = localtime((time_t*)c->data);
        len = strftime(buffer, width, "%F %T", time_data);
        break;
    default:
        len = sprintf(buffer, "0x%*lx", (int)width, (size_t)c->data);
        break;
    }
    if (len < width) {
        // truncate if it is still too long
        buffer = strncpy(buffer, buffer, width);
    }
    // FIX: dirty hack to make sure it is always \0 terminated
    buffer[width+1] = '\0';
    printf("%s", buffer);
    free(buffer);
}


ssize_t cells_len(cell_t* head)
{
    cell_t* cur = head;
    size_t i = 0;

    for (;;) {
        if (cur == NULL) return i;
        cur = cur->next;
        // Make sure that's not a cycle
        if (cur == head) return -1; 
        i++;
    }
}


cell_t* cells_insert(cell_t* head, const size_t idx)
{
    cell_t* cur = head;
    size_t i = 0;

    for (;;) {
        if (i == idx) return cur;
        if (cur == NULL) return NULL;
        cur = cur->next;
        // Make sure that's not a cycle
        if (cur == head) return -1; 
        i++;
    }
}


cell_t* cells_get(cell_t* head, const size_t idx)
{
    cell_t* cur = head;
    size_t i = 0;

    for (;;) {
        if (i == idx) return cur;
        if (cur == NULL) return NULL;
        cur = cur->next;
        // Make sure that's not a cycle
        if (cur == head) return -1; 
        i++;
    }
}


void cells_free(cell_t* head)
{
    cell_t* tmp;

    for (;;) {
        if (head == NULL) return;
        tmp = head;
        head = head->next;
        free(tmp);
    }
}




#endif
