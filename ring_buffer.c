#include "ring_buffer.h"
#include <stdlib.h>

ringbuf* ringbuf_create(unsigned int size)
{
	if(size > 0)
	{
		ringbuf* buf = (ringbuf*)malloc(sizeof(ringbuf) + size);
		if(buf == NULL)
		{
			return NULL;
		}

		buf->buf_size = size;
		buf->buf_head = buf->buf_tail = buf->buf_start;

		pthread_mutex_init(&buf->mutex,NULL);
		pthread_cond_init(&buf->notempty, NULL);
		return buf;
	}
	return NULL;
}

int ringbuf_push(ringbuf* pbuf, const void* data, unsigned int size)
{
	pthread_mutex_lock(&pbuf->mutex);

	//if buffer free space larger than message size then handle it, otherwise discard this message
	printf("buf_free is %d\n", ringbuf_free(pbuf));
	if(pbuf && data && size > 0 && size < ringbuf_free(pbuf))
	{
		unsigned int tail_free = RINGBUF_END(pbuf) - RINGBUF_TAIL(pbuf) + 1;
		printf("tail free is %d\n", tail_free);
		//copy 1 time if remaining free space from buffer_tail to buffer_end is larger than message size
		if(tail_free >= size)
		{
			memcpy(RINGBUF_TAIL(pbuf),(char*)data,size);
			RINGBUF_TAIL(pbuf) += size;
			if(RINGBUF_TAIL(pbuf) > RINGBUF_END(pbuf) )
				RINGBUF_TAIL(pbuf) = RINGBUF_START(pbuf);
		}
		//copy 2 time if remaining free space from buffer_tail to buffer_end is less than message size
		else
		{
			memcpy(RINGBUF_TAIL(pbuf),(char*)data,tail_free);
			unsigned int lev = size - tail_free;
			memcpy(RINGBUF_START(pbuf),(char*)data+tail_free,lev);
			RINGBUF_TAIL(pbuf) = RINGBUF_START(pbuf) + lev;
		}

		pthread_cond_signal(&pbuf->notempty);
		pthread_mutex_unlock(&pbuf->mutex);
		return 1;
	}	
	pthread_mutex_unlock(&pbuf->mutex);
	return 0;
 
}


int ringbuf_pop(ringbuf* pbuf, void* data, unsigned int size)
{
	printf("pthread_cond_wait...\n");

	pthread_mutex_lock(&pbuf->mutex);
	if(RINGBUF_HEAD(pbuf) == RINGBUF_TAIL(pbuf))
		pthread_cond_wait(&pbuf->notempty, &pbuf->mutex);

	printf("receive pthread_cond_wait singal\n");

	if(pbuf && data && size > 0 && size <= ringbuf_used(pbuf) )
	{
		unsigned int head_used = RINGBUF_END(pbuf) - RINGBUF_HEAD(pbuf) + 1;
		if(head_used >= size)
		{
			memcpy((char*)data,RINGBUF_HEAD(pbuf),size);
			RINGBUF_HEAD(pbuf) += size;
			if(RINGBUF_HEAD(pbuf) > RINGBUF_END(pbuf))
				RINGBUF_HEAD(pbuf) = RINGBUF_START(pbuf);
		}
		else
		{
			memcpy((char*)data,RINGBUF_HEAD(pbuf),head_used);
			unsigned int lev = size - head_used;
			memcpy((char*)data+head_used,RINGBUF_START(pbuf),lev);
			RINGBUF_HEAD(pbuf) = RINGBUF_START(pbuf) + lev;
		}
		pthread_mutex_unlock(&pbuf->mutex);
		return 1;
	}
	pthread_mutex_unlock(&pbuf->mutex);
	return 0;
 
}

void ringbuf_destroy(ringbuf* pbuf)
{
	free(pbuf);
}

unsigned int ringbuf_free(ringbuf* pbuf)
{
	return (RINGBUF_HEAD(pbuf) <= RINGBUF_TAIL(pbuf) ? RINGBUF_SIZE(pbuf)+RINGBUF_HEAD(pbuf) - RINGBUF_TAIL(pbuf) 
		: RINGBUF_HEAD(pbuf) - RINGBUF_TAIL(pbuf));
}

unsigned int ringbuf_used(ringbuf* pbuf)
{
	return (RINGBUF_HEAD(pbuf) <= RINGBUF_TAIL(pbuf) ? RINGBUF_TAIL(pbuf) - RINGBUF_HEAD(pbuf) 
		: RINGBUF_SIZE(pbuf)+RINGBUF_TAIL(pbuf)-RINGBUF_HEAD(pbuf));
}