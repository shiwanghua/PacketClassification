//
// Created by swh on 22-10-4.
//

#ifndef _TAMASEARCH_H_
#define _TAMASEARCH_H_

#include "data_structure.h"
#include "constant.h"
#include <cstdint>
#include <vector>

using namespace std;

class TamaSearch // 13 attributes
{
	const uint8_t ipLevelNum; // the first level (level 0) is always empty, the last level is 8 (0-8)
	const uint8_t portLevelNum;

	uint16_t ipNodeCounter; //used as the No. of a new node, start by 0 (the root node)
	uint32_t portNodeCounter;

	vector<uint16_t> ipLchild, ipRchild;
	vector<uint32_t> portLchild, portRchild;

	vector<uint8_t> ipMid; // ipMid records the middle value of the interval covered by the node
	vector<uint16_t> portMid;

	vector<vector<vector<uint32_t>>> ip_port; // 0-7 srcIP3-0 dstIP3-0; 8: srcPort_high8bits; 9: srcPort_low8bits; 10: dstPort_high8bits; 11: dstPort_low8bits
	vector<vector<uint32_t>> protocol;

	vector<uint8_t> ruleSize, counter;

	void initiate_ip_layer(uint8_t lvlNo, uint8_t l, uint8_t r);
	void initiate_port_layer(uint8_t lvlNo, uint16_t l, uint16_t r);

	void
	insert_ip_layer(uint8_t attrNo, uint8_t lvlNo, uint16_t nodeNo, uint32_t ruleNo, const uint8_t l, const uint8_t r, const uint8_t low, const uint8_t high);
	void
	insert_port_layer(uint8_t attrNo, uint8_t lvlNo, uint32_t nodeNo, uint32_t ruleNo, const uint16_t l, const uint16_t r, const uint16_t low, const uint16_t high);

	bool deleteRule(int p, int att, int subID, int l, int r, int low, int high, int lvl);

	void
	ip_accurate_search(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint8_t value);
	void
	port_accurate_search_nocheck(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value);
	void
	port_accurate_search_check(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value, const ACL_rules*);

 public:

	TamaSearch();
	~TamaSearch();

	void initize(uint32_t numRule);

	void insert(const rule*);

	bool deleteRule(const rule*);

	uint32_t accurate_search(const message& pub, const ACL_rules* rules);

	double calMemory();

};

#endif //_TAMASEARCH_H_
