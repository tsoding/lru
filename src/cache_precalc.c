#define CACHE_NAME "Precalculated Hash Table"

typedef struct {
    Word key;
    size_t value;
} Item;

Item *table = NULL;

void cache_init(String_View content)
{
    while (content.count > 0) {
        String_View line = sv_chop_by_delim(&content, '\n');
        while (line.count > 0) {
            String_View word_orig = sv_trim(sv_chop_by_delim(&line, ' '));
            if (word_orig.count > 0) {
                Word word = word_norm(sv_as_word(word_orig));
                ptrdiff_t index = hmgeti(table, word);
                size_t value = index < 0 ? 0 : table[index].value;
                hmput(table, word, value + 1);
            }
        }
    }
}

bool cache_get(Word key, size_t *value)
{
    ptrdiff_t index = hmgeti(table, key);
    assert(index >= 0 && "The key should be always available in the cache. Guaranteed by the cache_init()");
    if (value) *value = table[index].value;
    return true;
}

void cache_put(Word key, size_t value)
{
    (void) key;
    (void) value;
    assert(0 && "The cache miss will never happen. Guaranteed by the cache_init()");
}

void cache_cleanup(void)
{
    hmfree(table);
}
