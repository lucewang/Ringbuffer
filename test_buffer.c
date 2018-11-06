#include "ring_buffer.h"
#include "cpe_msg_type.h"
#include <unistd.h>

void printheadandtail(ringbuf* pbuf);
void pinrtmsg(cpe_swupdate_msg& p_msg);

void *thread_read(void* args);
void *thread_write(void* args);

int main()
{
	ringbuf* ringbuf_LED = NULL;

	ringbuf_LED = ringbuf_create(BUF_SIZE);	
	if(NULL==ringbuf_LED)
	{
		printf("init buffer error.\n");
		return 0; 
	}
	printheadandtail(ringbuf_LED);

    pthread_t pid[2];
    int ret;
    ret = pthread_create(&pid[0],NULL,thread_read,(void*)ringbuf_LED);
    if(ret!= 0)
    {
        printf("can not open thread_read\r\n");
    }
    ret = pthread_create(&pid[1],NULL,thread_write,(void*)ringbuf_LED);
    if(ret!= 0)
    {
        printf("can not open thread_write\r\n");
    }

    void* res;

    pthread_join(pid[0], &res);
    pthread_join(pid[1], &res);


    // debug msg type
    /*
	cpe_swupdate_msg l_msg;
    l_msg.msg_type = 1;
    l_msg.trans_id = 500;
    l_msg.content = 100000;
    
    printf("sizeof message is %d\n", (int)sizeof(l_msg));

    ringbuf_push(ringbuf_LED, (void*)&l_msg, sizeof(l_msg));
    printheadandtail(ringbuf_LED);
    
    cpe_swupdate_msg l_msg2;
    ringbuf_pop(ringbuf_LED, (void*)&l_msg2, sizeof(l_msg));
    pinrtmsg(l_msg2);
    printheadandtail(ringbuf_LED);

    ringbuf_push(ringbuf_LED, (void*)&l_msg, sizeof(l_msg));
    printheadandtail(ringbuf_LED);

    //cpe_swupdate_msg l_msg2;
    ringbuf_pop(ringbuf_LED, (void*)&l_msg2, sizeof(l_msg));
    pinrtmsg(l_msg2);
    printheadandtail(ringbuf_LED);
    */

    //  debug  string
    /*
	char tm[64];
	strcpy(tm,"helloworld");
	ringbuf_push(ringbuf_LED, tm, strlen(tm));
	printheadandtail(ringbuf_LED);

	memset(tm,0x00,sizeof(tm));
	ringbuf_pop(ringbuf_LED,tm,10);
	printf("from buf is %s\r\n",tm);
	printheadandtail(ringbuf_LED);


	ringbuf_push(ringbuf_LED, tm, strlen(tm));
	printheadandtail(ringbuf_LED);

	memset(tm,0x00,sizeof(tm));
	ringbuf_pop(ringbuf_LED,tm,10);
	printf("from buf is %s\r\n",tm);
	printheadandtail(ringbuf_LED);

	memset(tm,0x00,sizeof(tm));
	ringbuf_pop(ringbuf_LED,tm,10);
	printf("from buf is %s\r\n",tm);
	printheadandtail(ringbuf_LED);
    */

	ringbuf_destroy(ringbuf_LED);

	printf("init buffer ok\n"); 
	return 0;
}

void printheadandtail(ringbuf* pbuf)
{
	printf("buffer tail-header length = %d\n", (int)(RINGBUF_TAIL(pbuf)-RINGBUF_HEAD(pbuf))); 
}

void printmsg(cpe_swupdate_msg& p_msg)
{
    printf("msg_type = %d\n", p_msg.msg_type);
    printf("trans_id = %d\n", p_msg.trans_id);
    printf("content = %d\n", p_msg.content);
}


void* thread_read(void* args)
{
    cpe_swupdate_msg l_msg;
    ringbuf *buf=(ringbuf*)args;

    while(1)
    {
        ringbuf_pop(buf, (void*)&l_msg, sizeof(l_msg));
        printmsg(l_msg);
    }
    return 0;
}


void* thread_write(void* args)
{
    ringbuf *buf=(ringbuf*)args;
    cpe_swupdate_msg l_msg;
    l_msg.msg_type = 1;
    l_msg.trans_id = 10;
    l_msg.content = 100;

    while(1)
    {
        ringbuf_push(buf, (void*)&l_msg, sizeof(l_msg));        
        sleep(1);
        l_msg.trans_id++;
        l_msg.content++;
    }
    return 0;
}