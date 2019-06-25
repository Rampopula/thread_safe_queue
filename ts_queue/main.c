#include <stdio.h>
#include <stdint.h>

#define CACHE_LINE_SIZE		(64)

inline uint32_t queue_align_size(uint32_t size);


typedef struct TSNode {
	uint32_t size;
	void *data;
	struct TSNode *next;
} TSNode_t;

typedef struct TSQueue {



} TSQueue_t;


int main(int argc, char **argv) {



	getchar();
	return 0;
}

inline uint32_t queue_align_size(uint32_t size) {

	return (size % CACHE_LINE_SIZE) ? size + (CACHE_LINE_SIZE - (size % CACHE_LINE_SIZE)) : size;
}