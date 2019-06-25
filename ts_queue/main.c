#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "ts_queue.h"

static TSQueue_t data_q;

void *dest_routine(void *arg) {

	pthread_detach(pthread_self());

	printf("receiver started...\n");

	while (1) {		

		TSNode_t *node = NULL;
		node = queue_pop_item(&data_q);
		if (node == NULL) {
			printf("destination: there is no data!\n");
		}
		else {
			printf("destination: received %d bytes of data\n", node->size);	
			queue_node_free(node);
		}	

		usleep(1000 * 1000);

	}
}


void *src_routine(void *arg) {

	pthread_detach(pthread_self());

	printf("sender started...\n");

	while (1) {

		usleep(1000 * 1000);

		int32_t rand_val = 1 + rand() % 200000;
		uint8_t *tmp = malloc(rand_val);
		memset(tmp, 0x00, rand_val);
		queue_push_item(&data_q, (void*)tmp, rand_val);
		free(tmp);

		printf("source: send %d bytes of data\n", rand_val);	
	}
}

int main(int argc, char **argv) {

	srand(time(NULL));

	int32_t ret = 0;
	
	pthread_t dest_thr, src_thr;

	ret = queue_init(&data_q);
	if (ret == -1) {
		fprintf(stdout, "queue_init() failed!\n");
		goto exit;
	}

	ret = pthread_create(&dest_thr, NULL, dest_routine, NULL);
	if (ret == -1) {
		fprintf(stdout, "pthread_create() dest_routine failed!\n");
		goto exit;
	}

	ret = pthread_create(&src_thr, NULL, src_routine, NULL);
	if (ret == -1) {
		fprintf(stdout, "pthread_create() src_routine failed!\n");
		goto exit;
	}

	usleep(3000 * 1000);

	ret = pthread_cancel(dest_thr);
	if (ret == -1) {
		fprintf(stdout, "pthread_cancel() failed!\n");
	}
	else {
		fprintf(stdout, "pthread_cancel() ok!\n");
	}




exit:
	getchar();
	queue_deinit(&data_q);
	return 0;
}

