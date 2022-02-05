#ifndef __DATA_STRUCTURE_H_
#define __DATA_STRUCTURE_H_
#include<stdio.h>

typedef struct rule {
	int PRI;  //priority
	unsigned char protocol[2];
	unsigned char source_mask;
	unsigned char destination_mask;
	unsigned char source_ip[4];
	unsigned char destination_ip[4];
	unsigned short source_port[2];
	unsigned short destination_port[2];
}rule;

typedef struct ACL_rules
{
	int size;
	int capacity;
	rule* list;
}ACL_rules;

typedef struct data
{
	int PRI;  //priority
	unsigned short source_mask;
	unsigned short destination_mask;
	unsigned char source_ip[4];
	unsigned char destination_ip[4];
	unsigned short source_port[2];
	unsigned short destination_port[2];
}data;

typedef struct Cell
{
	int size;
	int capacity;
	data* list;
}Cell;

typedef struct message
{
	unsigned int protocol;
	unsigned char source_ip[4];
	unsigned char destination_ip[4];
	unsigned short source_port;
	unsigned short destination_port;
}message;

typedef struct ACL_messages
{
	int size;
	int capacity;
	message* list;
}ACL_messages;

#endif //__DATA_STRUCTURE_H_