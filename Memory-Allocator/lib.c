#include "lib.h"
#include <sys/mman.h>

header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock = PTHREAD_MUTEX_INITIALIZER;

header_t *get_free_block(size_t size){
    header_t *curr = head;
    while(curr){
        if(curr->s.is_free && curr->s.size >= size){
            return curr;
        }
        curr = curr->s.next;
    }
    return NULL;
}

void *malloc(size_t size){
    size_t total_size;
    void *block;
    header_t *header;
    if (!size) {
        return NULL;
    }
    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);
    if(header){
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*) (header + 1);
    }else{
        total_size = sizeof(header_t) + size;
        // MAP_ANON is used for memory not backed by a file
        block = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);

        if(block == MAP_FAILED){
            pthread_mutex_unlock(&global_malloc_lock);
            return NULL;
        }

        header = block;
        header->s.size = size;
        header->s.is_free = 0;
        header->s.next = NULL;
        if(!head)
            head = header;
        if(tail)
            tail->s.next = header;
        tail = header;
        pthread_mutex_unlock(&global_malloc_lock);

        return (void*)(header + 1);
    }
}

void free(void *block){
    header_t *header, *tmp;

    if(!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;

    // Check if this is the last block in the list
    if(header == tail){
        if(head == tail){
            head = tail = NULL;
        }else{
            tmp = head;
            while (tmp) {
                if(tmp->s.next == tail) {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }

        // Unmap the memory including the header
        munmap(header, sizeof(header_t) + header->s.size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

void *calloc(size_t num, size_t nsize){
    size_t size;
    void *block;
    if(!num || !nsize){
        return NULL;
    }

    size = num * nsize;
    if(nsize != size / num){
        return NULL;
    }
    block = malloc(size);
    if(!block){
        return NULL;
    }
    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size){
    header_t * header;
    void *ret;
    if(!block ||!size){
        return malloc(size);
    }
    header = (header_t*)block - 1;
    if(header->s.size >= size){
        return block;
    }
    ret = malloc(size);
    if(ret){
        memcpy(ret, block, header->s.size);
        free(block);
    }
    return ret;

}

void print_heap_status() {
    header_t *curr = head;
    printf("HEAP STATUS: ");
    if (!head) { printf("[EMPTY]\n"); return; }

    while(curr) {
        printf("[%zu bytes | %s] -> ", curr->s.size, curr->s.is_free ? "FREE" : "USED");
        curr = curr->s.next;
    }
    printf("NULL\n");
}
