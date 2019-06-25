#include <stdio.h>
#include "ts_queue.h"




int main(int argc, char **argv) {

	int32_t ret = 0;
	TSQueue_t data_q;
	
	ret = queue_init(&data_q);
	if (ret == -1) {
		fprintf(stdout, "%s() failed!\n", __func__);
		return -1;
	}


	getchar();
	return 0;
}

