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

class HEMBS // bitsets search
{
	unsigned long long* begin_bits; // for successive storage
	unsigned long long*** bitsets; // [attrID][bitsetID][offset]
	unsigned long long** bitsets2; // [bitsetID][offset], Deprecated
	int numUnit; // The times of OR operation between two bitsets
	double memorysize;
 public:

	HEMBS();
	~HEMBS();

	void forward_init_bitsets_IPv4(int numRule);
	void forward_bitsets_insert_IPv4(const rule* r);
	std::array<uint64_t, 2> forward_bitsets_search_IPv4(const message* m, const rule* rules, uint32_t& matchRuleNo);

	void backward_init_bitsets_IPv4(int numRule);
	void backward_bitsets_insert_IPv4(const rule*);
	std::array<uint64_t, 2>
	backward_bitsets_search_IPv4(const message* list, const rule* rules, uint32_t& matchRuleNo);

	void visualize_bitsets(unsigned long long** bitsets);
	void backward_bitsets_visualize_one(const char* ruleSetName);

	double calMemory();
};

#endif //_HEMBS_H_
