#ifndef _CPEMSGTYPE_HEADER_
#define _CPEMSGTYPE_HEADER_


typedef struct cpe_swupdate_msg
{
    unsigned int msg_type;
    unsigned int trans_id;
    unsigned int content;
}cpe_swupdate_msg;

#endif