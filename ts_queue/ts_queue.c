/**
*
* @brief     rampopula's thread safe queue implementation
* @file      ts_queue.c
* @author    Eduard Chaika
* @date      June 25, 2019
*
*/

#include "ts_queue.h"

static inline int32_t queue_align_size(int32_t size) {

	return (size % CACHE_LINE_SIZE) ? size + (CACHE_LINE_SIZE - (size % CACHE_LINE_SIZE)) : size;
}

static void cleanup_mutex(void *arg) {
	pthread_mutex_unlock((pthread_mutex_t*)arg);
}

int32_t queue_init(TSQueue_t *queue) {

	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);

	pthread_mutex_lock(&queue->mutex);

	queue->head = NULL;
	queue->tail = NULL;

	queue->lenght = 0;
	queue->inited = 1;

	pthread_mutex_unlock(&queue->mutex);

	return 0;
}

int32_t queue_deinit(TSQueue_t *queue) {

	TSNode_t *node, *node_next;

	queue->inited = 0;
	pthread_cond_signal(&queue->cond);

	node = queue->head;
	while (node) {
		
		node_next = node->next;
		
		if (node->data != NULL) {
			free(node->data);
			node->data = NULL;
		}

		free(node);
		node = node_next;
	}

	queue->head = NULL;
	queue->tail = NULL;
	queue->lenght = 0;

	pthread_mutex_unlock(&queue->mutex);

	pthread_cond_destroy(&queue->cond);
	pthread_mutex_destroy(&queue->mutex);

	return 0;
}

int32_t queue_node_free(TSNode_t *node) {

	if (node->data != NULL) {
		free(node->data);
		node->data = NULL;
	}

	free(node);

	return 0;
}

int32_t queue_push_item(TSQueue_t *queue, void *data, int32_t size) {

	TSNode_t *node;

	node = (TSNode_t*)malloc(sizeof(TSNode_t));
	if (node == NULL) {
		fprintf(stdout, "%s() node malloc failed!", __func__);
		return -1;
	}

	node->next = NULL;

	if (size > 0) {

		node->data = malloc(queue_align_size(size));
		if (node->data == NULL) {
			free(node);
			fprintf(stdout, "%s() node data malloc failed!", __func__);		
			return -1;
		}

		memcpy(node->data, data, size);
		node->size = size;
	}
	else {

		node->data = NULL;
		node->size = 0;
	}

	pthread_mutex_lock(&queue->mutex);
	pthread_cleanup_push(cleanup_mutex, (void*)&queue->mutex);

	if (queue->tail == NULL) {

		queue->head = node;
	} 
	else {

		queue->tail->next = node;
	}

	queue->tail = node;
	queue->lenght++;

	pthread_cond_signal(&queue->cond);
	pthread_cleanup_pop(1);

	return 0;
}

TSNode_t *queue_pop_item(TSQueue_t *queue) {

	TSNode_t *node;

	pthread_mutex_lock(&queue->mutex);
	pthread_cleanup_push(cleanup_mutex, (void*)&queue->mutex);

	for (;;) {

		if (!queue->inited) {
			node = NULL;
			break;
		}

		node = queue->head;
		if (node != NULL) {

			queue->head = node->next;

			if (queue->head == NULL) {
				queue->tail = NULL;
			}

			queue->lenght--;
			break;
		}
		else {
			pthread_cond_wait(&queue->cond, &queue->mutex);
		}
	}

	pthread_cleanup_pop(1);

	return node;
}