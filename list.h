// list.h

#ifndef LIST_H_
#define LIST_H_

#define LIST_NODE_PLACEHOLDER_CHAR ' '
#define LIST_NAME_SZ 64

typedef enum {
    NT_UNDEF,
    NT_TEXT,
    NT_INT,
    NT_FLOAT,
    NT_DATETIME,
    NT_LIST,
    NT_ENUM_LEN // Last type
} node_type_t;

typedef struct list_t list_t;

typedef union {
    char* text;
    ssize_t i;
    double f;
    time_t dt;
    list_t* list;
} node_data_t;

typedef struct node_t {
    size_t index;
    node_type_t tag;
    node_data_t data;
    struct node_t* next;
} node_t;

typedef enum {
    sorted = 0x1
} list_flags_t;

struct list_t {
    size_t len;
    char name[LIST_NAME_SZ];
    node_t* head;
    node_t* tail;
    union {
        node_type_t child_tag;
    } meta;
    list_flags_t flags;
};

#define FOREACH(node, list)                                                    \
    for (node_t* node = (list)->head; node != NULL; node = node->next)

const node_t node_empty = {.tag = NT_UNDEF, .data = {NULL}, .next = NULL};

/// node_alloc_empty(): Returns NULL on failure
node_t* node_alloc(void)
{
    node_t* node = malloc(sizeof(node_t));
    if (node == NULL) return NULL;
    node->tag = NT_UNDEF;
    node->next = NULL;
    node->data.i = 0;
    node->index = 0;
    return node;
}

node_t* node_alloc_text(char* value)
{
    node_t* node = node_alloc();
    if (node == NULL) return NULL;
    node->tag = NT_TEXT;
    node->data.text = value;
    return node;
}

node_t* node_alloc_int(const ssize_t value)
{
    node_t* node = node_alloc();
    if (node == NULL) return NULL;
    node->tag = NT_INT;
    node->data.i = value;
    return node;
}

node_t* node_alloc_float(const ssize_t value)
{
    node_t* node = node_alloc();
    if (node == NULL) return NULL;
    node->tag = NT_FLOAT;
    node->data.f = value;
    return node;
}

node_t* node_alloc_datetime(const time_t value)
{
    node_t* node = node_alloc();
    if (node == NULL) return NULL;
    node->tag = NT_DATETIME;
    node->data.dt = value;
    return node;
}

node_t* node_alloc_list(list_t* value)
{
    node_t* node = node_alloc();
    if (node == NULL) return NULL;
    node->tag = NT_LIST;
    node->data.list = value;
    return node;
}

void node_free(node_t* node);

// Returns NULL on failure
list_t* list_alloc(void)
{
    list_t* list;

    list = malloc(sizeof(list_t));
    if (list != NULL) {
        list->name[0] = '\0';
        list->head = NULL;
        list->tail = NULL;
        list->len = 0;
    }
    return list;
}

list_t* list_alloc_named(const char* name)
{
    list_t* list = list_alloc();
    if (list != NULL) strcpy(list->name, name);
    return list;
}

list_t* list_alloc_n(node_t node_template, const size_t len)
{
    list_t* list = list_alloc();
    if (list == NULL) return NULL;

    if (len == 0) {
        return list;
    }

    // TODO: Arena would be better here
    list->head = malloc(sizeof(node_t));
    list->head->tag = node_template.tag;
    list->head->data = node_template.data;
    if (list->head == NULL) {
        free(list);
        return NULL;
    }
    list->head->next = NULL;

    node_t* cur = list->head;
    for (size_t i = 1; i < len; i += 1) {
        cur->next = malloc(sizeof(node_t));
        // TODO: replace assert with proper deallocation and return NULL
        assert(cur->next != NULL);
        cur = cur->next;
        cur->tag = node_template.tag;
        cur->data = node_template.data;
    }
    list->tail = cur;
    list->tail->next = NULL;
    list->len = len;
    return list;
}

list_t* list_clear(list_t* list)
{
    node_t* tmp;
    node_t* cur = list->head;
    for (;;) {
        if (cur == NULL) return list;
        tmp = cur;
        cur = cur->next;
        if (tmp->tag == NT_LIST) {
            list_clear(tmp->data.list);
            free(tmp->data.list);
        }
        node_free(tmp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    return list;
}

void list_free(list_t* list)
{
    list = list_clear(list);
    free(list);
}

void node_free(node_t* node) {
    if (node == NULL) return;
    switch (node->tag) {
    case NT_TEXT:
        free(node->data.text);
        break;
    case NT_LIST:
        list_free(node->data.list);
        break;
    default:
        break;
    }
    free(node);
}


size_t list_len(const list_t* list)
{
    size_t i = 0;
    FOREACH(cur, list) { i++; }
    return i;
}

node_t* list_get(const list_t* list, const size_t idx)
{
    size_t i = 0;
    FOREACH(cur, list)
    {
        if (i == idx) return cur;
        i++;
    }
    return NULL;
}

list_t* list_prepend(list_t* list, node_t* node)
{
    // TODO: calculate new tail properly
    size_t extra_len = 1;
    node_t* new_tail = node;
    for (; new_tail->next != NULL; new_tail = new_tail->next) {
        extra_len += 1;
    }

    new_tail->next = list->head;
    list->head = node;
    list->len += extra_len;
    return list;
}

list_t* list_append(list_t* list, node_t* node)
{
    size_t extra_len = 1;
    node_t* new_tail = node;
    for (; new_tail->next != NULL; new_tail = new_tail->next) {
        extra_len += 1;
    }
    // Corner case: list is emoty
    if (list->len == 0) {
        list->head = node;
        list->tail = new_tail;
        list->len = extra_len;
        return list;
    }
    list->tail->next = node;
    list->tail = new_tail;
    list->len += extra_len;
    return list;
}

list_t* list_append_n(list_t* list, const node_t node_template,
                      const size_t len)
{
    if (len == 0) return list;

    list_t* tmp_list = list_alloc_n(node_template, len);
    if (tmp_list == NULL) return NULL;

    if (list->len == 0) {
        list->head = tmp_list->head;
    } else {
        list->tail->next = tmp_list->head;
    }
    list->tail = tmp_list->tail;
    list->len = list_len(list);

    free(tmp_list);
    return list;
}

list_t* list_split_at(list_t* list1, const size_t idx)
{
    list_t* new_list1 = list1;
    list_t* new_list2 = NULL;

    // Corner case: idx == 0
    if (idx == 0) {
        new_list2 = list1;
        new_list1 = list_alloc();
        if (new_list1 == NULL) return NULL;
        list1 = new_list1;
        return new_list2;
    }

    // Corner case: idx is outside of the list len
    if (list1->len <= idx) {
        // 0123456789--  len: 10
        //            ^  idx: 11
        // 0123456789A   list1 len: 11
        //            0  list2 len: 1
        node_t node_template = {
            .tag = list1->tail->tag,
            .data = {NULL},
            .next = NULL,
        };

        // Corner case: we have a child uninitialised list
        list_t* list_node;
        if (node_template.tag == NT_LIST) {
            list_node = list_alloc_named("NewColumn");
            node_template.data.list = list_node;
        }

        new_list1 = list_append_n(list1, node_template, idx - list1->len);
        if (new_list1 == NULL) return NULL;
        new_list2 = list_alloc();

        list1 = new_list1;
        return new_list2;
    }

    // General case
    node_t* new_tail1 = list_get(list1, idx - 1);
    assert(new_tail1 != NULL);
    node_t* new_head2 = new_tail1->next;
    new_list1->tail = new_tail1;
    new_list1->len = idx;
    list1 = new_list1;

    new_list2 = list_alloc();
    new_list2 = list_append(new_list2, new_head2);
    return new_list2;
}

/// Merges `list1` and `list2`. Frees `list2`
list_t* list_concat(list_t* list1, list_t* list2)
{
    if (list1 == NULL) return list2;
    if (list2 == NULL) return list1;

    list_t* new_list = list1;

    new_list->tail->next = list2->head;
    new_list->tail = list2->tail;
    new_list->len += list2->len;

    free(list2);
    return new_list;
}

list_t* list_resize(list_t* list, const node_t node_template,
                    const size_t new_len)
{
    size_t cur_len = list->len;
    list_t* result;

    // Case 1: list does't change
    if (new_len == cur_len) {
        result = list;
    }

    // Case 2: list must grow
    if (new_len > cur_len) {
        // 0123     cur_len: 4
        //     456  to_append: 3
        // 0123456  new_len: 7
        size_t tail_len = new_len - cur_len;
        result = list_append_n(list, node_empty, tail_len);
    }

    // Case 2: list must shrink
    if (new_len < cur_len) {
        // 01234567  cur_len: 8
        // 01234     new_len: 5
        //      ^    split_at: 5
        size_t split_at = new_len;
        result = list;
        list_t* appendix = list_split_at(result, split_at);
        list_free(appendix);
    }

    return result;
}

list_t* list_insert_at(list_t* list, node_t* node, const size_t idx)
{
    // Corner case: idx = 0
    if (idx == 0) {
        return list_prepend(list, node);
    }

    // General case
    list_t* new_list1 = list;
    list_t* new_list2 = list_split_at(new_list1, idx);
    assert(new_list2 != NULL);

    new_list1 = list_append(new_list1, node);
    new_list1 = list_concat(new_list1, new_list2);

    return new_list1;
}

list_t* list_remove(list_t* list, const size_t idx)
{
    // Corner case: list is empty
    if (list->len == 0) {
        return list;
    }
    // Corner case: list is shorter than idx
    if (idx >= list->len) {
        return list;
    }
    // Corner case: removing the head
    if (idx == 0) {
        node_t* new_head = list->head->next;
        node_free(list->head);
        list->head = new_head;
        list->len -= 1;
        if (list->len == 0) {
            list->tail = NULL;
        }
        return list;
    }

    // General case
    node_t* node_pre_idx = list_get(list, idx - 1);
    node_t* node_at_idx = node_pre_idx->next;
    node_t* node_post_idx = node_at_idx->next;
    node_pre_idx->next = node_post_idx;
    list->len -= 1;
    if (node_at_idx == list->tail) {
        list->tail = node_pre_idx;
    }
    node_free(node_at_idx);
    return list;
}

void node_to_str(const node_t* node, char* buffer, const int buf_size)
{
    buffer[0] = '\0';
    struct tm* time_data;

    int len = buf_size - 1;
    switch (node->tag) {
    case NT_UNDEF:
        memset(buffer, LIST_NODE_PLACEHOLDER_CHAR, buf_size - 1);
        break;
    case NT_TEXT:
        buffer = strncpy(buffer, node->data.text, buf_size - 1);
        break;
    case NT_INT:
        len = sprintf(buffer, "%*zd", buf_size - 1, node->data.i);
        break;
    case NT_FLOAT:
        len = sprintf(buffer, "%*.2f", buf_size - 2, node->data.f);
        break;
    case NT_DATETIME:
        time_data = localtime((time_t*)node->data.dt);
        len = strftime(buffer, buf_size - 1, "%F %T", time_data);
        break;
    case NT_LIST:
        len = sprintf(buffer, "tbl[%*zd]", buf_size - 1, node->data.list->len);
        break;
    default:
        len = sprintf(buffer, "0x%*lx", buf_size - 1, (size_t)node->data.i);
        break;
    }
    if (len > buf_size) {
        // truncate if it is still too long
        buffer = strncpy(buffer, buffer, buf_size);
    }
    // TODO: dirty hack to make sure it is always \0 terminated
    buffer[buf_size - 1] = '\0';
}

#endif
