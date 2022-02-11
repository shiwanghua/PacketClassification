#ifndef __CORE_H_
#define __CORE_H_

#include"tool.h"
#include "math.h"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define LEVEL_solution1 4
#define LEVEL_solution2 4
#define LEVEL_solution3 4

#define srcIP_LAYER_1 0
#define srcIP_SIZE_1 33
#define srcIP_END_CELL_1 32
#define srcIP_WIDTH_1 3  // using to comput index cell id ( x >> *_WIDYH = id )

#define srcIP_LAYER_2 1
#define srcIP_SIZE_2 257
#define srcIP_END_CELL_2 256
#define srcIP_WIDTH_2 0

#define srcIP_LAYER_3 2
#define srcIP_SIZE_1 33
#define srcIP_END_CELL_1 32
#define srcIP_WIDTH_1 3

#define dstIP_LAYER_1 0
#define dstIP_SIZE_1 257//33
#define dstIP_END_CELL_1 256//32
#define dstIP_WIDTH_1 0//3

#define dstIP_LAYER_2 1
#define dstIP_SIZE_2 257
#define dstIP_END_CELL_2 256
#define dstIP_WIDTH_2 0

#define PORT_LAYER 2
#define PORT_SIZE 65
#define PORT_END_CELL 64
#define PORT_WIDTH 10

#define PROTO_LAYER 3  // layer index
#define PROTO_SIZE 15  // protocol layer cell number
#define PROTO_END_CELL 14  // protocol layer cell end id

#define layer1 0
#define layer2 1
#define layer3 2
#define layer4 3

// ------solution3------
#define LAYER1_SIZE 15
#define LAYER1_END_CELL 14
#define LAYER1_WIDTH 0
#define LAYER2_SIZE 33
#define LAYER2_END_CELL 32
#define LAYER2_WIDTH 3
#define LAYER3_SIZE 65
#define LAYER3_END_CELL 64
#define LAYER3_WIDTH 2
#define LAYER4_SIZE 33
#define LAYER4_END_CELL 32
#define LAYER4_WIDTH 3

//#define LAYER_0 srcIP_SIZE_1
//#define LAYER_1 srcIP_SIZE_2
//#define LAYER_2 PORT_SIZE
//#define LAYER_3 PROTO_SIZE

#define CELL_SIZE_solution1 8268975 // srcIP1/srcIP2/dstPort/protocol
#define CELL_SIZE_solution2 64397775//8268975 // dtsIP1/dstIP2/dstPort/protocol
#define CELL_SIZE_solution3 1061775 // srcIP1/srcIP2/srcIP3/protocol

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


void insert_solution1(Cell *c_list, rule *p);

void insert_solution2(Cell *c_list, rule *p);

void insert_solution3(Cell *c_list, rule *p);

int simple_match(ACL_rules* a, message* p,int*);

int match_solution1(Cell *_c, message *p);

int match_with_log_solution1(Cell *_c, message *p, int *_cycle);

int match_with_log2_solution1(Cell *_c, message *p, int *_cycle, int *checkNum);

int match_with_log_solution2(Cell *_c, message *p, int *_cycle);

int match_with_log_solution3(Cell *_c, message *p, int *_cycle);

//void check(ACL_rules* r, message* m);
void get_cell_size(Cell *c, char[],int);

void analyse_log(ACL_rules *data);

void analyse_log2(ACL_rules *data, char[]);

double get_memory(Cell *c_list);

void visualize(int c[], int checkNum[], int size);

#endif // !__CORE_H_
