#define CACHE_NAME "Just Linked List"

#define cache_init(content) do {} while(0)

typedef struct {
    ptrdiff_t prev;
    ptrdiff_t next;
    Word key;
    size_t value;
} Node;

Node *queue = NULL;

bool cache_get(Word key, size_t *value)
{
    for (ptrdiff_t i = llfront(queue); i >= 0; i = queue[i].next) {
        if (strcmp(queue[i].key.data, key.data) == 0) {
            if (value) *value = queue[i].value;
            llmovefront(queue, i);
            return true;
        }
    }

    return false;
}

void cache_put(Word key, size_t value)
{
    if (llcount(queue) < CACHE_CAP) {
        llpushfront(queue);
    } else {
        llmovefront(queue, llback(queue));
    }

    queue[llfront(queue)].key = key;
    queue[llfront(queue)].value = value;
}

void cache_cleanup(void)
{
    llfree(queue);
}
