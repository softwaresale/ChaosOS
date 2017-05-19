#include <queue.h>
#include <stdint.h>
#include <malloc.h>
#include <stddef.h>
#include <string.h>

queue_t
queue(size_t size)
{
	// quick sanity check
	if (size < 0)
		size = DEFAULT_SIZE;

	queue_t tmp;
	tmp.data     = (void*) malloc((uint32_t) size); // allocate pointer
	tmp.max_size = size;
	tmp.index    = 0;

	return tmp;
}

void
qpush(queue_t que, void* data)
{
	// cast ptr to int
	char* tmp = (char*) que.data;
	int i;
	for (i = 0; i < sizeof(data); i++)
		append(tmp, (char)&data[i]); // append each value to the queue
	
	que.index += i;

	// recast back.
	// TODO: this will likely cause an error
	tmp.data = (void*) tmp;
	
	// update size
}
