#ifndef __CORE_H_
#define __CORE_H_

#include"tool.h"

#define LEVEL 4

#define IP_LAYER_1 0
#define IP_SIZE_1 33
#define IP_EDN_CELL_1 32
#define IP_WIDTH_1 3  // using to comput index cell id ( x >> *_WIDYH = id )

#define IP_LAYER_2 1
#define IP_SIZE_2 257
#define IP_EDN_CELL_2 256
#define IP_WIDTH_2 0

#define PORT_LAYER 2
#define PORT_SIZE 65
#define PORT_END_CELL 64
#define PORT_WIDTH 10

#define PROTO_LAYER 3  // layer index
#define PROTO_SIZE 15  // protocol layer cell number
#define PROTO_END_CELL 14  // protocol layer cell end id

#define LAYER_0 IP_SIZE_1
#define LAYER_1 IP_SIZE_2
#define LAYER_2 PORT_SIZE
#define LAYER_3 PROTO_SIZE

#define CELL_SIZE 8268975

#define ICMP 0x01    // 1
#define IGMP 0x02    // 2
#define GGP 0x03     // 3
#define IP 0x04      // 4
#define ST 0x05      // 5
#define TCP 0x06     // 6
#define CBT 0x07     // 7
#define EGP 0x08     // 8
#define UDP 0x11     // 17
#define GRE 0x2F     // 47
#define ESP 0x32     // 50
#define AH 0x33      // 51
#define EIGRP 0x58   // 88
#define OSPFIGP 0x59 // 89


void insert(Cell *c_list, rule *r);

int simple_match(ACL_rules* a, message* p,int*);

int match(Cell *c_list, message *m);

int match_with_log(Cell *c_list, message *m, int *_cycle);

int match_with_log2(Cell *_c, message *p, int *_cycle, int *checkNum);

//void check(ACL_rules* r, message* m);
void get_cell_size(Cell *c, char[]);

void analyse_log(ACL_rules *data);

void analyse_log2(ACL_rules *data, char[]);

double get_memory(Cell *c_list);

void visualize(int c[], int checkNum[], int size);

#endif // !__CORE_H_
