/**
*
* @brief     rampopula's thread safe queue interface implementation
* @file      ts_queue.h
* @author    Eduard Chaika
* @date      June 25, 2019
*
*/

#ifndef __RAMP_TS_QUEUE___
#define __RAMP_TS_QUEUE___

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define ALIGNMENT_SIZE		(64)

typedef struct TSNode {
	int32_t size;
	void *data;
	struct TSNode *next;
} TSNode_t;

typedef struct TSQueue {
	int32_t	lenght;
	int32_t inited;
	TSNode_t *head;
	TSNode_t *tail;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
} TSQueue_t;


int32_t queue_init(TSQueue_t *queue);

int32_t queue_deinit(TSQueue_t *queue);

int32_t queue_node_free(TSNode_t *node);

int32_t queue_push_item(TSQueue_t *queue, void *data, int32_t size);

TSNode_t *queue_pop_item(TSQueue_t *queue);

#endif