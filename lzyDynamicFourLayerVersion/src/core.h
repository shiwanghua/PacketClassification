#ifndef __CORE_H_
#define __CORE_H_

#include"tool.h"
#include "math.h"

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

#define ENABLE_LOG 0
#define ENABLE_ANALYSE 0
//#define DEBUG

#define LEVEL 4

#define PROTO 1  // if equal to 1 using this att to index
#define PROTO_LAYER 1  //  if this att is enabled, this refers to the layer
#define PROTO_SIZE 4  // protocol layer cell number
#define PROTO_END_CELL 3  // protocol layer cell end id

#define SIP_1 0
#define SIP_1_LAYER 0
#define SIP_SIZE_1 129
#define SIP_EDN_CELL_1 128
#define SIP_WIDTH_1 1  // using to comput index cell id ( x >> *_WIDYH = id )

#define SIP_2 0
#define SIP_2_LAYER 2
#define SIP_SIZE_2 33
#define SIP_EDN_CELL_2 32
#define SIP_WIDTH_2 3

#define SIP_3 1
#define SIP_3_LAYER 0
#define SIP_SIZE_3 129
#define SIP_EDN_CELL_3 128
#define SIP_WIDTH_3 1

#define SIP_4 0
#define SIP_4_LAYER 0
#define SIP_SIZE_4 129
#define SIP_EDN_CELL_4 128
#define SIP_WIDTH_4 1

#define DIP_1 0
#define DIP_1_LAYER 2
#define DIP_SIZE_1 33
#define DIP_EDN_CELL_1 32
#define DIP_WIDTH_1 3

#define DIP_2 0
#define DIP_2_LAYER 0
#define DIP_SIZE_2 129
#define DIP_EDN_CELL_2 128
#define DIP_WIDTH_2 1

#define DIP_3 0
#define DIP_3_LAYER 0
#define DIP_SIZE_3 129
#define DIP_EDN_CELL_3 128
#define DIP_WIDTH_3 1

#define DIP_4 0
#define DIP_4_LAYER 0
#define DIP_SIZE_4 65
#define DIP_EDN_CELL_4 64
#define DIP_WIDTH_4 2

#define SPORT 1
#define SPORT_LAYER 3
#define SPORT_SIZE 257
#define SPORT_END_CELL 256
#define SPORT_WIDTH 8

#define DPORT 1
#define DPORT_LAYER 2
#define DPORT_SIZE 257
#define DPORT_END_CELL 256
#define DPORT_WIDTH 8

#define LAYER_0 129
#define LAYER_1 4
#define LAYER_2 257
#define LAYER_3 257

#define CELL_SIZE 34081284 // 4601025

#define NUM_PROTOCOL 16
#define PORT_NUMBER 65536
#define PORT_WIDTH 256
#define NUM_PORT_BITSET 256 // PORT_WIDTH * NUM_PORT_BITSET == PORT_NUMBER
#define NUM_BITSET 2577 // 256*8+NUM_PORT_BITSET*2+NUM_PROTOCOL+1 // 1: protocol=0

#define ICMP 0x01    // 1 d2
#define IGMP 0x02    // 2
#define GGP 0x03     // 3
#define IP 0x04      // 4
#define ST 0x05      // 5
#define TCP 0x06     // 6 d2
#define CBT 0x07     // 7
#define EGP 0x08     // 8
#define UDP 0x11     // 17 d2
#define RSVP 0x2E    // 46 d2new
#define GRE 0x2F     // 47
#define ESP 0x32     // 50
#define AH 0x33      // 51
#define EIGRP 0x58   // 88
#define OSPFIGP 0x59 // 89
#define ISIS 0x7C    // 124 d2new

#define useShuffleHeader 2 // 0:不随机 1:随机 2:华为英才数据
#define useShuffleRule 2   // 0:不随机 1:随机 2:华为英才数据

void insert(Cell *c_list, rule *r);

void insert_cache(Cell *c_list, rule *r, unsigned char **);

int match(Cell *c_list, message *m);

int match_with_log(Cell *c_list, message *m, int *_cycle, MatchLog *log);

int match_with_log_cache(Cell *c_list, message *m, unsigned char **, int *_cycle, MatchLog *log);

//void check(ACL_rules* r, message* m);
void get_cell_size(Cell *c);

void analyse_log(ACL_rules *data);

void analyse_log2(ACL_rules *data, char outputFileName[]);

void printProtocolType(ACL_rules *data, char outputFileName[]);

double get_memory(Cell *c_list);

void visualize(int c[], int size);

int numUnit; // The times of OR operation between two bitsets
void init_bitset_IPv4(int numRule, unsigned long ***bitsets);

void insert_bitset_forward_IPv4(rule *r, unsigned long **);

int match_bitset_forward_IPv4(const rule *datasets, message *m, unsigned long long **bitsets, int *_cycle, unsigned long *);

void visualize_bitsets(unsigned long ** bitsets);
void backward_bitsets_visualize_one(const char *ruleSetName, int numUnit, unsigned long long ***bitsets);

#endif // !__CORE_H_
