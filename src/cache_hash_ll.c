#define CACHE_NAME "Hash Table + Linked List"

#define cache_init(content) do {} while(0)

typedef struct {
    ptrdiff_t prev;
    ptrdiff_t next;
    Word key;
} Node;

typedef struct {
    Word key;
    size_t value;
    ptrdiff_t queue_idx;
} Item;

Node *queue = NULL;
Item *table = NULL;

bool cache_get(Word key, size_t *value)
{
    ptrdiff_t index = hmgeti(table, key);
    if (index >= 0) {
        if (value) *value = table[index].value;
        llmovefront(queue, table[index].queue_idx);
        return true;
    }
    return false;
}

void cache_put(Word key, size_t value)
{
    if (llcount(queue) < CACHE_CAP) {
        llpushfront(queue);
    } else {
        hmdel(table, queue[llback(queue)].key);
        llmovefront(queue, llback(queue));
    }

    queue[llfront(queue)].key = key;

    Item item = {
        .key = key,
        .value = value,
        .queue_idx = llfront(queue),
    };

    hmputs(table, item);
}

void cache_cleanup(void)
{
    llfree(queue);
    hmfree(table);
}
