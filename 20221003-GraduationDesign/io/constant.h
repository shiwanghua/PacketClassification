#ifndef __CONSTANT_H_
#define __CONSTANT_H_

#include <string>

const std::string expID="230";
#define DATASET_NO 2

#define VERIFICATION false
#define DEBUG false
#define SHUFFLEMESSAGES true
#define SHUFFLERULES false // Do not change.

#define HEM_BS_NUM_ATTR 11 // 8 (IP), 9 (IP+Protocol), 11 (IP+Protocol+Port)
#define HEM_BS_PORT_CELLWIDTH 256
#define HEM_BS_NUM_PORT_BITSET 256 // HEM_BS_PORT_CELLWIDTH * HEM_BS_NUM_PORT_BITSET == PORT_NUMBER, both are power of 2
#define AGGREGATE_RATIO 32 // used in AFBS, aggregating ratio, 1 bit represents $aggRatio bits, Compression ratio
#define INCREMENT 10 // used in RFBS, the add increment when searching

#define TAMA_PRIORITY_CHECK false
#define TAMA_IP_LEVEL_NUM 8 // must be 8; the first level (level 0) is always empty, the last level is 8 (0-8)
#define TAMA_PORT_LEVEL_NUM 16

#define NUM_PROTOCOL 16

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

#endif
