// Copyright 2021 Alexey Kutepov <reximkut@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef LL_H_
#define LL_H_

// Tightly Packed Doubly Linked-List
//
//   Please don't use it for anything. This was implemented just for
//   demonstration purposes.
//
//   Implementation is inspired by https://github.com/nothings/stb/blob/master/stb_ds.h
//
//   All of the nodes are tightly packed in a continuous array and
//   referenced via indices. The index has type ptrdiff_t.
//   The index < 0 denotes no node and used as NULL throughout the
//   library. You can store whatever you want your nodes the library
//   only cares about next and prev fields of your Node definition.
//
// Non-function interface:
//
//   Node definition:
//   
//     typedef struct {
//         ptrdiff_t next;
//         ptrdiff_t prev;
//         // ...
//         // ... whatever custom fields you want to store your value(s) ...
//         // ...
//     } Node;
//
//   Declaring empty Linked-List:
//     Node *xs = NULL;
//
//   Accessing front and back nodes
//     xs[llfront(xs)].value = 69;
//     xs[llback(xs)].value = 69;
//
//   Iterating:
//     for (ptrdiff_t i = llfront(xs); i >= 0; i = xs[i].next) {
//         do_something_with_value(xs[i].value);
//     }
//
// Function interface (some of them are actually macros):
//
//   void llpushfront(Node *xs)
//     Push a new node at the front of the Linked-List.
//     You can access that node via xs[llfront(xs)].
//     Pusing a new value looks like this:
//       llpushfront(xs);
//       xs[llfront(xs)].value = 69;
//
//   void llmovefront(Node *xs, ptrdiff_t index)
//     Move the node refered by the index to the front of the Linked-List
//
//   size_t llcount(Node *xs)
//     Amount of the elements in the linked list
//
//   ptrdiff_t llfront(Node *xs)
//     Index of the front element
//
//   ptrdiff_t llback(Node *xs)
//     Index of the back element
//
//   void llfree(Node *xs)
//     Free all of the memory allocated by the list
//
//  Compile-Time options:
//    #define LL_INIT_CAP initial capacity of the list
//    #define LLASSERT(x) custom assert
//    #define LLREALLOC(p, s) custom realloc
//    #define LLFREE(p, s) custom free

#ifndef LLASSERT
#include <assert.h>
#define LLASSERT(x) assert(x)
#endif

#if defined(LLREALLOC) && !defined(LLFREE) || !defined(LLREALLOC) && defined(LLFREE)
#error "You must define both LLREALLOC and LLFREE, or neither."
#endif
#if !defined(LLREALLOC) && !defined(LLFREE)
#include <stdlib.h>
#define LLREALLOC(p,s) realloc(p,s)
#define LLFREE(p)      free(p)
#endif

#ifndef LL_INIT_CAP
#define LL_INIT_CAP 32
#endif

typedef struct {
    size_t count;
    size_t capacity;
    ptrdiff_t front;
    ptrdiff_t back;
} ll__header;

#define llpushfront(ll) \
    ((ll) = ll__pushfront((ll), sizeof(*(ll)), (char*) &(ll)->prev - (char*)(ll), (char*) &(ll)->next - (char*)(ll)))

ll__header *ll__grow(ll__header *header, size_t node_size)
{
    if (header == NULL) {
        header = LLREALLOC(NULL, sizeof(ll__header) + LL_INIT_CAP * node_size);
        LLASSERT(header);
        header->count = 0;
        header->capacity = LL_INIT_CAP;
        header->front = -1;
        header->back = -1;
    } else {
        header->capacity *= 2;
        header = LLREALLOC(header, sizeof(ll__header) + header->capacity * node_size);
    }

    return header;
}

void *ll__pushfront(void *ptr, size_t node_size, size_t prev_offset, size_t next_offset)
{
    ll__header *header = NULL;

    if (ptr == NULL) {
        header = ll__grow(NULL, node_size);
    } else {
        header = (ll__header*) ptr - 1;
        if (header->count >= header->capacity) {
            header = ll__grow(header, node_size);
        }
    }

    char *nodes = (char *)(header + 1);
#define DEREF(index, offset) *(ptrdiff_t*)(nodes + node_size * (index) + (offset))
    ptrdiff_t node_index = header->count++;
    if (node_index == 0) {
        DEREF(node_index, next_offset) = -1;
        DEREF(node_index, prev_offset) = -1;
        header->front = node_index;
        header->back  = node_index;
    } else {
        DEREF(node_index, prev_offset) = -1;
        DEREF(node_index, next_offset) = header->front;
        DEREF(header->front, prev_offset) = node_index;
        header->front = node_index;
    }
#undef DEREF

    return nodes;
}

size_t llcount(void *ll)
{
    return ll ? ((ll__header*) ll - 1)->count : 0;
}

ptrdiff_t llback(void *ll)
{
    return ll ? ((ll__header*) ll - 1)->back : -1;
}

ptrdiff_t llfront(void *ll)
{
    return ll ? ((ll__header*) ll - 1)->front : -1;
}

void llfree(void *ll)
{
    if (ll) LLFREE((ll__header*) ll - 1);
}

#define llmovefront(ll, index) ll__movefront((ll), sizeof(*(ll)), (char*)&(ll)->prev - (char*)(ll), (char*)&(ll)->next - (char*)(ll), (index))

void ll__movefront(void *ll, size_t node_size, size_t prev_offset, size_t next_offset, ptrdiff_t index)
{
    LLASSERT(ll != NULL);
    ll__header *header = (ll__header*)ll - 1;
    char *nodes = ll;

    LLASSERT(index >= 0);
    LLASSERT(index < (ptrdiff_t) header->count);

#define DEREF(index, offset) *(ptrdiff_t*)(nodes + node_size * (index) + (offset))
    if (DEREF(index, prev_offset) >= 0) DEREF(DEREF(index, prev_offset), next_offset) = DEREF(index, next_offset);
    if (DEREF(index, next_offset) >= 0) DEREF(DEREF(index, next_offset), prev_offset) = DEREF(index, prev_offset);
    if (index == header->back) header->back = DEREF(index, prev_offset);
    if (index == header->front) header->front = DEREF(index, next_offset);
    DEREF(index, prev_offset) = -1;
    DEREF(index, next_offset) = header->front;
    if (header->front >= 0) DEREF(header->front, prev_offset) = index;
    header->front = index;
#undef DEREF
}

#endif // LL_H_
