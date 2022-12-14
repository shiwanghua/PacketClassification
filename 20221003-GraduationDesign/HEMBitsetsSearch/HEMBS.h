//
// Created by swh on 22-10-4.
//

#ifndef _HEMBS_H_
#define _HEMBS_H_

#include<malloc.h>
#include <cstring>
#include <array>
#include "constant.h"
#include"data_structure.h"


#define NUM_BITSET 2577 // 256*8+HEM_BS_NUM_PORT_BITSET*2+NUM_PROTOCOL+1 // 1: protocol=0

class HEMBS // bitsets search
{
	unsigned long long ***bitsets;
	unsigned long long **bitsets2;
	unsigned long long *begin_bits;
	int numUnit; // The times of OR operation between two bitsets
	double memorysize;
 public:

	HEMBS();
	~HEMBS();

	void forward_init_bitsets_IPv4(int numRule);
	void forward_bitsets_insert_IPv4(rule *r, unsigned long **);
	unsigned int forward_bitsets_search_IPv4(const rule *, message *m, int *_cycle, unsigned long *);

	void backward_init_bitsets_IPv4(int numRule);
	void backward_bitsets_insert_IPv4(const rule*);
	std::array<uint64_t ,2> backward_bitsets_search_IPv4(const message* list, const ACL_rules* rules, uint32_t& matchRuleNo);

	void visualize_bitsets(unsigned long long ** bitsets);
	void backward_bitsets_visualize_one(const char *ruleSetName);

	double calMemory();
};

#endif //_HEMBS_H_
