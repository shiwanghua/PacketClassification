//
// Created by swh on 22-10-4.
//

#ifndef _HEMBS_H_
#define _HEMBS_H_

#include<malloc.h>
#include <cstring>
#include <array>
#include <cmath>
#include <vector>
#include "constant.h"
#include "Utils.h"
#include"data_structure.h"

class HEMBS // bitsets search
{
	uint32_t numUnit; // The times of OR operation between two bitsets
	double memorysize;
	unsigned long long* beginBits; // for successive storage
	unsigned long long*** bitsets; // [attrID][bitsetID][offset]
	unsigned long long** bitsets2; // [bitsetID][offset], Deprecated

	uint32_t 	aggRatio; // aggregating ratio, 1 bit represents $aggRatio bits
	uint32_t log2AggRatio; // =log_2 (aggRatio), use ">>log2AggRatio" instead of "/aggRatio"
	uint32_t numAggUnit; // The number of 64-bits aggregated units in an aggregated bitset
	uint32_t numBitsTo64; // $numBitsTo64 bits in aggBitsets can represent 64 bits in bitsets (require aggRatio<=64)
	uint32_t num64Bits; // 1 aggregating bit represents $num64Bits 64-bits unit (require aggRatio>=64)
	uint32_t log2Num64Bits; // =log_2(num64Bits)
	unsigned long long* aggBeginBits; // for successive aggregating storage
	unsigned long long*** aggBitsets; // [attrID][bitsetID][offset]

	uint32_t log2AggRatio2; //  = log2AggRatio+6

	uint32_t increment; // RLE, the add increment when searching
	std::vector<std::vector<std::pair<uint32_t,uint32_t>>> rleCode[HEM_BS_NUM_ATTR]; // Run Length Encoding

	void aggregate_forward_aggbitsets_delete_IPv4(uint64_t unit, unsigned long long& aggUnit, const uint32_t ofi, uint64_t aggOffsetMask);

 public:
	HEMBS();
	~HEMBS();

	void forward_init_bitsets_IPv4(uint32_t numRule);
	void forward_bitsets_insert_IPv4(const rule* r);
	std::array<uint64_t, 3> forward_bitsets_search_IPv4(const message* m, const rule* rules, uint32_t& matchRuleNo);

	void backward_init_bitsets_IPv4(uint32_t numRule);
	void backward_bitsets_insert_IPv4(const rule*);
	std::array<uint64_t, 3>
	backward_bitsets_search_IPv4(const message* list, const rule* rules, uint32_t& matchRuleNo);

	void aggregate_forward_init_bitsets_IPv4(uint32_t numRule);
	void aggregate_forward_bitsets_insert_IPv4(const rule* r);
	void aggregate_forward_bitsets_delete_IPv4(const rule* r);
	std::array<uint64_t, 5> aggregate_forward_bitsets_search_IPv4(const message* m, const rule* rules, uint32_t& matchRuleNo);

	void RLE_forward_init_bitsets_IPv4(uint32_t numRule);
	void RLE_forward_bitsets_insert_IPv4(const rule* r);
	void RLE_forward_construction_IPv4();
	std::array<uint64_t, 3> RLE_forward_bitsets_search_IPv4(const message* m, const rule* rules, uint32_t& matchRuleNo);


	void forward_bitsets_visualization(std::string& outStr);
	void backward_bitsets_visualize_one(const char* ruleSetName);

	double calMemory();
};

#endif //_HEMBS_H_
