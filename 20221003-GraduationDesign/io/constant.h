#ifndef __CONSTANT_H_
#define __CONSTANT_H_

#include <string>

const std::string expID="1";

#define VERIFICATION true
#define DEBUG true

#define NUM_PROTOCOL 16
#define PORT_NUMBER 65536

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