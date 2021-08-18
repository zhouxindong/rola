#ifndef ROLA_JSNADER_HEARTBEAT_H
#define ROLA_JSNADER_HEARTBEAT_H

#define MSG_TYPE1 1
#define MSG_TYPE2 2
#define MSG_HEARTBEAT 3

typedef struct
{
	unsigned int type;
	char data[2000];
}msg_t;

#define T1 60 /*idle time before heartbeat*/
#define T2 10 /*time to wait for response*/

#endif // !ROLA_JSNADER_HEARTBEAT_H
