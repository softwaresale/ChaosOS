
/* This file handles a simple array-based queue */

#ifndef QUEUE_H
#define QUEUE_H 1

#include <stdint.h>
#include <malloc.h>

#define DEFAULT_SIZE 32

// first, queue data type
struct _queue
{
	void*  data;     // empty array
	size_t max_size; // size of array
	int    index;    // current index of array
};

typedef struct _queue queue_t;

/**\breif Create a new queue object 
 * \param size -- max size of queue
 * \return An instantiated queue */
queue_t
queue(size_t size);

/**\breif Pushes a value to the queue 
 * \param queue_t -- queue to push to
 * \param void*   -- value to push */
void
qpush(queue_t, void*);

/**\breif Removes the front value
 * \param queue_t queue to pop from*/
void
qpop(queue_t);

/**\breif Get's front value from queue
 * \param queue_t -- queue to get front val from
 * \return front value */
void*
qfront(queue_t);

/**\breif Deallocates queue
 * \param queue_t queue to deallocate
 * */
void 
qdestroy(queue_t);

#endif // QUEUE_H
