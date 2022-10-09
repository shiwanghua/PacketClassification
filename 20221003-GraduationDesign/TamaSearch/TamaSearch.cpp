//
// Created by swh on 22-10-4.
//

#include "TamaSearch.h"

TamaSearch::TamaSearch() : ipLevelNum(TAMA_IP_LEVEL_NUM), portLevelNum(TAMA_PORT_LEVEL_NUM)
{
	ipNodeCounter = 0;
}

TamaSearch::~TamaSearch()
{
}

void TamaSearch::initiate_ip_layer(uint8_t lvlNo, uint8_t l, uint8_t r)
{
	if (lvlNo == ipLevelNum) return;
	uint16_t nodeNo = ipNodeCounter;
	ipMid[nodeNo] = l + ((r - l) >> 1);
	if (l != r)
	{ // in theory l can't equal to r because the last if
		ipLchild[nodeNo] = ++ipNodeCounter;
		initiate_ip_layer(lvlNo + 1, l, ipMid[nodeNo]);
		ipRchild[nodeNo] = ++ipNodeCounter;
		initiate_ip_layer(lvlNo + 1, ipMid[nodeNo] + 1, r);
	}
	else
	{
		printf("IP Initiate Error: lvl= %d, l= %d, r=%d\n", lvlNo, l, r); // never run to here
	}
}

void TamaSearch::initiate_port_layer(uint8_t lvlNo, uint16_t l, uint16_t r)
{
	if (lvlNo == portLevelNum) return;
	uint32_t nodeNo = portNodeCounter;
	portMid[nodeNo] = l + ((r - l) >> 1);
	if (l != r)
	{
		portLchild[nodeNo] = ++portNodeCounter;
		initiate_port_layer(lvlNo + 1, l, ipMid[nodeNo]);
		portRchild[nodeNo] = ++portNodeCounter;
		initiate_port_layer(lvlNo + 1, ipMid[nodeNo] + 1, r);
	}
}

void TamaSearch::initize(uint32_t numRule)
{
	ipNodeCounter = 0;
	uint16_t ipNodeNumber = 1 << (ipLevelNum + 1); // number of nodes in each IP attribute
	ipLchild.resize(ipNodeNumber, 0);
	ipRchild.resize(ipNodeNumber, 0);
	ipMid.resize(ipNodeNumber, 0);
	ip_port.resize(10, vector<vector<uint32_t>>(ipNodeNumber));
	uint32_t portNodeNumber = 1 << (portLevelNum);
	ip_port[8].resize(portNodeNumber);
	ip_port[9].resize(portNodeNumber);
	protocol.resize(16);
	ruleSize.resize(numRule, 0);
	counter.resize(numRule, 0);
	initiate_ip_layer(0, 0, 255);
	initiate_port_layer(0, 0, 65535);
}

// low/high value is the predicate interval value
// l/r value is the node interval value
void
TamaSearch::insert_ip_layer(uint8_t attrNo, uint8_t lvlNo, uint16_t nodeNo, uint32_t ruleNo, const uint8_t l, const uint8_t r, const uint8_t low, const uint8_t high)
{
	if ((low <= l && r <= high) || lvlNo == ipLevelNum)
	{
		if (l == r && lvlNo != ipLevelNum)
		{ // never run to here
			printf("attrNo= %d, lvlNo= %d, nodeNo= %d, ruleNo= %d, l= %d, r= %d, low= %d, high= %d\n", attrNo, lvlNo, nodeNo, ruleNo, l, r, low, high);
		}
		if (lvlNo == ipLevelNum && (low > l || high < r))
		{ // never run to here
			printf("attrNo= %d, lvlNo= %d, nodeNo= %d, ruleNo= %d, l= %d, r= %d, low= %d, high= %d\n", attrNo, lvlNo, nodeNo, ruleNo, l, r, low, high);
		}
		ip_port[attrNo][nodeNo].push_back(ruleNo);
		return;
	}
	if (high <= ipMid[nodeNo])
		insert_ip_layer(attrNo, lvlNo + 1, ipLchild[nodeNo], ruleNo, l, ipMid[nodeNo], low, high);
	else if (low > ipMid[nodeNo])
		insert_ip_layer(attrNo, lvlNo + 1, ipRchild[nodeNo], ruleNo, ipMid[nodeNo] + 1, r, low, high);
	else
	{
		insert_ip_layer(attrNo, lvlNo + 1, ipLchild[nodeNo], ruleNo, l, ipMid[nodeNo], low, high);
		insert_ip_layer(attrNo, lvlNo + 1, ipRchild[nodeNo], ruleNo, ipMid[nodeNo] + 1, r, low, high);
	}
}

void
TamaSearch::insert_port_layer(uint8_t attrNo, uint8_t lvlNo, uint32_t nodeNo, uint32_t ruleNo, const uint16_t l, const uint16_t r, const uint16_t low, const uint16_t high)
{
	if ((low <= l && r <= high) || lvlNo == portLevelNum)
	{
		ip_port[attrNo][nodeNo].push_back(ruleNo);
		return;
	}
	if (high <= portMid[nodeNo])
		insert_ip_layer(attrNo, lvlNo + 1, portLchild[nodeNo], ruleNo, l, ipMid[nodeNo], low, high);
	else if (low > ipMid[nodeNo])
		insert_ip_layer(attrNo, lvlNo + 1, portRchild[nodeNo], ruleNo, ipMid[nodeNo] + 1, r, low, high);
	else
	{
		insert_ip_layer(attrNo, lvlNo + 1, portLchild[nodeNo], ruleNo, l, ipMid[nodeNo], low, high);
		insert_ip_layer(attrNo, lvlNo + 1, portRchild[nodeNo], ruleNo, ipMid[nodeNo] + 1, r, low, high);
	}
}

void TamaSearch::insert(const rule* r)
{
//	int itv[8][2];
	uint8_t msk = r->source_mask;
	switch ((msk + 7) >> 3)
	{
	case 0: // mask=0
		break;
	case 1: // mask [1,8]
	{
		ruleSize[r->PRI] += 1;
		uint8_t free_bit = 8 - msk; // 0-7
		insert_ip_layer(0, 0, 0, r->PRI, 0, 255,
			r->source_ip[3] >> free_bit << free_bit, r->source_ip[3] | ((1 << free_bit) - 1));
		break;
	}
	case 2: // mask [9,16]
	{
		ruleSize[r->PRI] += 2;
		insert_ip_layer(0, 0, 0, r->PRI, 0, 255, r->source_ip[3], r->source_ip[3]);
		uint8_t free_bit = 16 - msk; // 0-7
		insert_ip_layer(1, 0, 0, r->PRI, 0, 255,
			r->source_ip[2] >> free_bit << free_bit, r->source_ip[2] | ((1 << free_bit) - 1));
		break;
	}
	case 3: // mask [17,24]
	{
		ruleSize[r->PRI] += 3;
		insert_ip_layer(0, 0, 0, r->PRI, 0, 255, r->source_ip[3], r->source_ip[3]);
		insert_ip_layer(1, 0, 0, r->PRI, 0, 255, r->source_ip[2], r->source_ip[2]);
		uint8_t free_bit = 24 - msk; // 0-7
		insert_ip_layer(2, 0, 0, r->PRI, 0, 255,
			r->source_ip[1] >> free_bit << free_bit, r->source_ip[1] | ((1 << free_bit) - 1));
		break;
	}
	case 4: // mask [25,32]
	{
		ruleSize[r->PRI] += 4;
		insert_ip_layer(0, 0, 0, r->PRI, 0, 255, r->source_ip[3], r->source_ip[3]);
		insert_ip_layer(1, 0, 0, r->PRI, 0, 255, r->source_ip[2], r->source_ip[2]);
		insert_ip_layer(2, 0, 0, r->PRI, 0, 255, r->source_ip[1], r->source_ip[1]);
		uint8_t free_bit = 32 - msk; // 0-7
		insert_ip_layer(3, 0, 0, r->PRI, 0, 255,
			r->source_ip[0] >> free_bit << free_bit, r->source_ip[0] | ((1 << free_bit) - 1));
		break;
	}
	}

	msk = r->destination_mask;
	switch ((msk + 7) >> 3)
	{
	case 0: // mask=0
		break;
	case 1: // mask [1,8]
	{
		ruleSize[r->PRI] += 1;
		uint8_t free_bit = 8 - msk; // 0-7
		insert_ip_layer(4, 0, 0, r->PRI, 0, 255,
			r->destination_ip[3] >> free_bit << free_bit, r->destination_ip[3] | ((1 << free_bit) - 1));
		break;
	}
	case 2: // mask [9,16]
	{
		ruleSize[r->PRI] += 2;
		insert_ip_layer(4, 0, 0, r->PRI, 0, 255, r->destination_ip[3], r->destination_ip[3]);
		uint8_t free_bit = 16 - msk; // 0-7
		insert_ip_layer(5, 0, 0, r->PRI, 0, 255,
			r->destination_ip[2] >> free_bit << free_bit, r->destination_ip[2] | ((1 << free_bit) - 1));
		break;
	}
	case 3: // mask [17,24]
	{
		ruleSize[r->PRI] += 3;
		insert_ip_layer(4, 0, 0, r->PRI, 0, 255, r->destination_ip[3], r->destination_ip[3]);
		insert_ip_layer(5, 0, 0, r->PRI, 0, 255, r->destination_ip[2], r->destination_ip[2]);
		uint8_t free_bit = 24 - msk; // 0-7
		insert_ip_layer(6, 0, 0, r->PRI, 0, 255,
			r->destination_ip[1] >> free_bit << free_bit, r->destination_ip[1] | ((1 << free_bit) - 1));
		break;
	}
	case 4: // mask [25,32]
	{
		ruleSize[r->PRI] += 4;
		insert_ip_layer(4, 0, 0, r->PRI, 0, 255, r->destination_ip[3], r->destination_ip[3]);
		insert_ip_layer(5, 0, 0, r->PRI, 0, 255, r->destination_ip[2], r->destination_ip[2]);
		insert_ip_layer(6, 0, 0, r->PRI, 0, 255, r->destination_ip[1], r->destination_ip[1]);
		uint8_t free_bit = 32 - msk; // 0-7
		insert_ip_layer(7, 0, 0, r->PRI, 0, 255,
			r->destination_ip[0] >> free_bit << free_bit, r->destination_ip[0] | ((1 << free_bit) - 1));
		break;
	}
	}

	if (r->source_port[0] != 0 || r->source_port[1] != 65535)
	{
		ruleSize[r->PRI]++;
		insert_port_layer(8, 0, 0, r->PRI, 0, 65535, r->source_port[0], r->source_port[1]);
	}
	if (r->destination_port[0] != 0 || r->destination_port)
	{
		ruleSize[r->PRI]++;
		insert_port_layer(9, 0, 0, r->PRI, 0, 65535, r->destination_port[0], r->destination_port[1]);
	}
	if (r->protocol[0])
	{
		ruleSize[r->PRI]++;
		protocol[r->protocol[1]].push_back(r->PRI);
	}
}

bool TamaSearch::deleteRule(int p, int att, int subID, int l, int r, int low, int high, int lvl)
{
	return false;
}

bool TamaSearch::deleteRule(const rule*)
{
	return false;
}

void
TamaSearch::ip_accurate_search(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint8_t value)
{
	if (lvlNo > ipLevelNum) return;
	for (auto& ri : ip_port[attrNo][nodeNo])
	{
//		if()
		counter[ri]--;
	}
	if (value <= ipMid[nodeNo])
		ip_accurate_search(attrNo, lvlNo + 1, ipLchild[nodeNo], value);
	else ip_accurate_search(attrNo, lvlNo + 1, ipRchild[nodeNo], value);
}

void
TamaSearch::port_accurate_search_nocheck(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value)
{
	if (lvlNo > portLevelNum) return;
	for (auto& ri : ip_port[attrNo][nodeNo])
	{
//		if()
		counter[ri]--;
	}
	if (value <= portMid[nodeNo])
		ip_accurate_search(attrNo, lvlNo + 1, portLchild[nodeNo], value);
	else ip_accurate_search(attrNo, lvlNo + 1, portRchild[nodeNo], value);
}

void
TamaSearch::port_accurate_search_check(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value, const ACL_rules* rules)
{

}

uint32_t TamaSearch::accurate_search(const message& pub, const ACL_rules* rules)
{
	counter = ruleSize;
	for (uint8_t ai = 0; ai < 4; ai++)
	{
		ip_accurate_search(ai, 0, 0, pub.source_ip[3 - ai]);
	}
	for (uint8_t ai = 4; ai < 8; ai++)
	{
		ip_accurate_search(ai, 0, 0, pub.destination_ip[7 - ai]);
	}

	if (portLevelNum < 17)
	{
		port_accurate_search_check(8, 0, 0, pub.source_port, rules);
		port_accurate_search_check(9, 0, 0, pub.destination_port, rules);
	}
	else
	{
		port_accurate_search_nocheck(8, 0, 0, pub.source_port);
		port_accurate_search_nocheck(9, 0, 0, pub.destination_port);
	}
	for (uint32_t ri = 0; ri < rules->size; ri++)
	{
		if (counter[ri] == 0)
			return ri;
	}

	return -1;
}

double TamaSearch::calMemory()
{
	return 0;
}















