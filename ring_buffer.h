#ifndef _RINGBUF_HEADER_
#define _RINGBUF_HEADER_

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 20

#define RINGBUF_START(pbuf) ((unsigned char*)(pbuf->buf_start))
#define RINGBUF_END(pbuf) (pbuf->buf_start+pbuf->buf_size-1)
#define RINGBUF_HEAD(pbuf) (pbuf->buf_head)
#define RINGBUF_TAIL(pbuf) (pbuf->buf_tail)
#define RINGBUF_SIZE(pbuf) (pbuf->buf_size)

struct ring_buffer
{
    pthread_mutex_t mutex;
    pthread_cond_t notempty;
	unsigned int buf_size;
	unsigned char* buf_head;
	unsigned char* buf_tail;
	unsigned char buf_start[1];
};

typedef struct ring_buffer ringbuf;

ringbuf* ringbuf_create(unsigned int size);

int ringbuf_push(ringbuf* pbuf, const void* data, unsigned int size);

int ringbuf_pop(ringbuf* pbuf, void* data, unsigned int size);

void ringbuf_destroy(ringbuf* pbuf);

unsigned int ringbuf_free(ringbuf* pbuf);

unsigned int ringbuf_used(ringbuf* pbuf);

#endif
