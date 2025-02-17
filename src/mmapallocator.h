#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#define BLOCK_SIZE sizeof(header_t)

typedef struct header {
	size_t size;
	unsigned short int is_free;
	struct header* next;
} header_t;

header_t *head = NULL, *tail = NULL;
pthread_mutex_t global_malloc_lock = PTHREAD_MUTEX_INITIALIZER;

header_t* get_free_mblock(size_t size) {
	header_t* curr = head;
	while (curr) {
		if (curr->is_free && curr->size >= size) {
			return curr;
		}

		curr = curr->next;
	}

	return NULL;
}

// Merging of adjacent free blocks
void coalesce() {
	header_t* curr = head;
	while (curr && curr->next) {
		if (curr->is_free && curr->next->is_free) {
			curr->size += BLOCK_SIZE + curr->next->size;
			curr->next = curr->next->next;

			if (curr->next == NULL) {
				tail = curr;
			}
		}
		else {
			curr = curr->next;
		}
	}
}

void* mmalloc(size_t size) {
	if (!size) {
		return NULL;
	}

	size = ALIGN(size);
	pthread_mutex_lock(&global_malloc_lock);

	header_t* header = get_free_mblock(size);
	if (header) {
		header->is_free = 0;
		pthread_mutex_unlock(&global_malloc_lock);

		return (void*)(header + 1);
	}

	size_t total_size = BLOCK_SIZE + size;
	void* block = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block == MAP_FAILED) {
		pthread_mutex_unlock(&global_malloc_lock);
		return NULL;
	}

	header = (header_t*)block;
	header->size = size;
	header->is_free = 0;
	header->next = NULL;

	if (!head) {
		head = header;
	}
	if (tail) {
		tail->next = header;
	}
	tail = header;

	pthread_mutex_unlock(&global_malloc_lock);
	return (void*)(header + 1);
}

void mfree(void* block) {
	if (!block) {
		return;
	}

	pthread_mutex_lock(&global_malloc_lock);
	header_t* header = (header_t*)block - 1;
	header->is_free = 1;

	coalesce();


    /*if (head == NULL || header->next == NULL) {
        tail = head;
    }*/

	pthread_mutex_unlock(&global_malloc_lock);
}

void* mrealloc(void* block, size_t size) {
	if (!block) {
		return mmalloc(size);
	}
	if (!size) {
		mfree(block);
		return NULL;
	}

	header_t* header = (header_t*)block - 1;
	if (header->size >= size) {
		return block;
	}

	void* new_block = mmalloc(size);
	if (new_block) {
		memcpy(new_block, block, header->size);
		mfree(block);
	}

	return new_block;
}

void* mcalloc(size_t num, size_t nsize) {
	if (!num || !nsize) {
		return NULL;
	}

	size_t size = num * nsize;
	if (nsize != size / num) {
		return NULL;
	}

	void* block = mmalloc(size);
	if (block) {
		memset(block, 0, size);
	}

	return block;
}

void print_memory_list() {
	header_t* curr = head;
	printf("Memory list:\n");
	printf("head = %p, tail = %p\n", (void*)head, (void*)tail);

	while (curr) {
		printf("Addr = %p, Size = %zu, Decimal block size = %zu, Hexadecimal block size = %x, Free = %u, next = %p\n",
		(void*)curr, curr->size, BLOCK_SIZE + curr->size,
		BLOCK_SIZE + curr->size, curr->is_free, (void*)curr->next);

		curr = curr->next;
	}
}
