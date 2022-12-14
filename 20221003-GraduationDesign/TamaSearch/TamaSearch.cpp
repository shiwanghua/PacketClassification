//
// Created by swh on 22-10-4.
//

#include <iostream>
#include <cstring>
#include "TamaSearch.h"

TamaSearch::TamaSearch()
	: ipLevelNum(TAMA_IP_LEVEL_NUM), portLevelNum(TAMA_PORT_LEVEL_NUM), ipNodeCounter(0), portNodeCounter(0)
{
}

TamaSearch::~TamaSearch()
{
}

void TamaSearch::initiate_ip_layer(uint8_t lvlNo, uint8_t l, uint8_t r)
{
	if (lvlNo == ipLevelNum) return;
	uint16_t nodeNo = ipNodeCounter;
	ipMid[nodeNo] = l + ((r - l) >> 1);
//	if (l != r)
//	{ // in theory l can't equal to r because the last if
	ipLchild[nodeNo] = ++ipNodeCounter;
	initiate_ip_layer(lvlNo + 1, l, ipMid[nodeNo]);
	ipRchild[nodeNo] = ++ipNodeCounter;
	initiate_ip_layer(lvlNo + 1, ipMid[nodeNo] + 1, r);
//	}
//	else
//	{
//		printf("IP Initiate Error: lvl= %d, l= %d, r=%d\n", lvlNo, l, r); // never run to here
//	}
}

void TamaSearch::initiate_port_layer(uint8_t lvlNo, uint16_t l, uint16_t r)
{
	if (lvlNo == portLevelNum) return;
	uint32_t nodeNo = portNodeCounter;
	portMid[nodeNo] = l + ((r - l) >> 1);
//	if (nodeNo == 11277)
//	{
//		cout << "l= " << l << ", r= " << r << ", mid= " << portMid[nodeNo] << ", lvlNo= " << (int)lvlNo << ", nodeNo= "
//			 << nodeNo << "\n";
//	}
//	if (l != r)
//	{
	portLchild[nodeNo] = ++portNodeCounter;
	initiate_port_layer(lvlNo + 1, l, portMid[nodeNo]); // bug: portMid not ipMid
	portRchild[nodeNo] = ++portNodeCounter;
	initiate_port_layer(lvlNo + 1, portMid[nodeNo] + 1, r);
//	}
//	else cout<<"Port Initiate Error: l= "<<l<<", r= "<<r<<", lvlNo= "<<lvlNo<<"\n";
}

void TamaSearch::initize(uint32_t numRule)
{
	ipNodeCounter = portNodeCounter = 0;
	uint16_t ipNodeNumber = (1 << (ipLevelNum + 1)) - 1; // number of nodes in each IP attribute
	ipLchild.resize(ipNodeNumber, 0);
	ipRchild.resize(ipNodeNumber, 0);
	ipMid.resize(ipNodeNumber, 0);
	ip_port.resize(10, vector<vector<uint32_t>>(ipNodeNumber));
	uint32_t portNodeNumber = (1 << (portLevelNum + 1)) - 1;
	portLchild.resize(portNodeNumber, 0);
	portRchild.resize(portNodeNumber, 0);
	portMid.resize(portNodeNumber, 0);
	ip_port[8].resize(portNodeNumber);
	ip_port[9].resize(portNodeNumber);
	protocol.resize(16);
	ruleSize.resize(numRule, 0);
	counter.resize(numRule, 0);
	initiate_ip_layer(0, 0, 255);
	initiate_port_layer(0, 0, 65535);
#if DEBUG
	//	cout << "ipNodeNumber= " << ipNodeNumber << ", portNodeNumber= " << portNodeNumber << "\n";
	//	cout << "ipNodeCounter= " << ipNodeCounter << ", portNodeCounter= " << portNodeCounter << "\n";
	totalMinusNum = 0;
	totalCmpNum = 0;
#endif
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
TamaSearch::insert_port_layer(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint32_t ruleNo, const uint16_t l, const uint16_t r, const uint16_t low, const uint16_t high)
{
	if ((low <= l && r <= high) || lvlNo == portLevelNum)
	{
		ip_port[attrNo][nodeNo].push_back(ruleNo);
		return;
	}
	if (high <= portMid[nodeNo])
		insert_port_layer(attrNo,
			lvlNo + 1, portLchild[nodeNo], ruleNo, l, portMid[nodeNo], low, high); // Bug: portMid not ipMid
	else if (low > portMid[nodeNo]) // Bug: portMid not ipMid
		insert_port_layer(attrNo, lvlNo + 1, portRchild[nodeNo], ruleNo, portMid[nodeNo] + 1, r, low, high);
	else
	{
		insert_port_layer(attrNo, lvlNo + 1, portLchild[nodeNo], ruleNo, l, portMid[nodeNo], low, high);
		insert_port_layer(attrNo, lvlNo + 1, portRchild[nodeNo], ruleNo, portMid[nodeNo] + 1, r, low, high);
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
	if (r->destination_port[0] != 0 || r->destination_port[1] != 65535)
	{
		ruleSize[r->PRI]++;
		insert_port_layer(9, 0, 0, r->PRI, 0, 65535, r->destination_port[0], r->destination_port[1]);
	}
	if (r->protocol[0])
	{
		ruleSize[r->PRI]++;
		uint8_t pi;
		switch (r->protocol[1])
		{
		case ICMP:
			pi = 0;
			break;
		case IGMP:
			pi = 1;
			break;
		case GGP:
			pi = 2;
			break;
		case IP:
			pi = 3;
			break;
		case ST:
			pi = 4;
			break;
		case TCP:
			pi = 5;
			break;
		case CBT:
			pi = 6;
			break;
		case EGP:
			pi = 7;
			break;
		case UDP:
			pi = 8;
			break;
		case RSVP:
			pi = 9;
			break;
		case GRE:
			pi = 10;
			break;
		case ESP:
			pi = 11;
			break;
		case AH:
			pi = 12;
			break;
		case EIGRP:
			pi = 13;
			break;
		case OSPFIGP:
			pi = 14;
			break;
		case ISIS:
			pi = 15;
			break;
		default:
			fprintf(stderr, "Insert Rule Error - unknown protocol %u !\n", r->protocol[1]);
			exit(0);
		}
		protocol[pi].push_back(r->PRI);
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
	for (const auto& ri : ip_port[attrNo][nodeNo])
	{
#if TAMA_PRIORITY_CHECK
		if (ri < matchRuleNo)
		{
			counter[ri]--;
			if (counter[ri] == 0)
				matchRuleNo = ri;
#if DEBUG
			totalMinusNum++;
			totalCmpNum++;
#endif
		}
#if DEBUG
		totalCmpNum++;
#endif
#else // not check priority
		counter[ri]--;
#if DEBUG
		totalMinusNum++;
#endif
#endif
	}
	if (value <= ipMid[nodeNo])
		ip_accurate_search(attrNo, lvlNo + 1, ipLchild[nodeNo], value);
	else ip_accurate_search(attrNo, lvlNo + 1, ipRchild[nodeNo], value);
}

void
TamaSearch::port_accurate_search_nocheck(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value)
{
	if (lvlNo > portLevelNum) return;
	for (const auto& ri : ip_port[attrNo][nodeNo])
	{
#if TAMA_PRIORITY_CHECK
		if (ri < matchRuleNo)
		{
			counter[ri]--;
			if (counter[ri] == 0)
				matchRuleNo = ri;
#if DEBUG
			totalMinusNum++;
			totalCmpNum++;
#endif
		}
#if DEBUG
		totalCmpNum++;
#endif
#else // not check priority
		counter[ri]--;
#if DEBUG
		totalMinusNum++;
#endif
#endif
	}
	if (value <= portMid[nodeNo])
		port_accurate_search_nocheck(attrNo, lvlNo + 1, portLchild[nodeNo], value);
	else port_accurate_search_nocheck(attrNo, lvlNo + 1, portRchild[nodeNo], value);
}

void
TamaSearch::port_accurate_search_check(const uint8_t attrNo, const uint8_t lvlNo, const uint32_t nodeNo, const uint16_t value, const ACL_rules* rules)
{
	if (lvlNo == portLevelNum)
	{
		if (attrNo == 8)
		{ // source port
			for (const auto& ri : ip_port[attrNo][nodeNo])
			{
				if (rules->list[ri].source_port[0] <= value && value <= rules->list[ri].source_port[1])
				{
#if TAMA_PRIORITY_CHECK
					if (ri < matchRuleNo)
					{
						counter[ri]--;
						if (counter[ri] == 0)
							matchRuleNo = ri;
#if DEBUG
						totalMinusNum++;
						totalCmpNum++;
#endif
					}
#if DEBUG
					totalCmpNum++;
#endif
#else // not check priority
					counter[ri]--;
#if DEBUG
					totalMinusNum++;
#endif
#endif
				}
			}
		}
		else
		{ // destination port
			for (const auto& ri : ip_port[attrNo][nodeNo])
			{
				if (rules->list[ri].destination_port[0] <= value && value <= rules->list[ri].destination_port[1])
				{
#if TAMA_PRIORITY_CHECK
					if (ri < matchRuleNo)
					{
						counter[ri]--;
						if (counter[ri] == 0)
							matchRuleNo = ri;
#if DEBUG
						totalMinusNum++;
						totalCmpNum++;
#endif
					}
#if DEBUG
					totalCmpNum++;
#endif
#else // not check priority
					counter[ri]--;
#if DEBUG
					totalMinusNum++;
#endif
#endif
				}
			}
		}
		return;
	}
	for (const auto& ri : ip_port[attrNo][nodeNo])
	{
#if TAMA_PRIORITY_CHECK
		if (ri < matchRuleNo)
		{
			counter[ri]--;
			if (counter[ri] == 0)
				matchRuleNo = ri;
#if DEBUG
			totalMinusNum++;
			totalCmpNum++;
#endif
		}
#if DEBUG
		totalCmpNum++;
#endif
#else // not check priority
		counter[ri]--;
#if DEBUG
		totalMinusNum++;
#endif
#endif
	}
	if (value <= portMid[nodeNo])
		port_accurate_search_nocheck(attrNo, lvlNo + 1, portLchild[nodeNo], value);
	else port_accurate_search_nocheck(attrNo, lvlNo + 1, portRchild[nodeNo], value);
}

uint32_t TamaSearch::search(const message& msg, const ACL_rules* rules)
{
#if TAMA_PRIORITY_CHECK
	matchRuleNo = rules->size - 1; // for priority check
#endif
	counter = ruleSize;
	for (uint8_t ai = 0; ai < 4; ai++)
	{
//		std::cout << ", ruleSize[56427]= " << (int)ruleSize[56427] << "，cnt= " << (int)counter[56427] << "\n";
		ip_accurate_search(ai, 0, 0, msg.source_ip[3 - ai]);
	}
	for (uint8_t ai = 4; ai < 8; ai++)
	{
//		std::cout << ", ruleSize[56427]= " << (int)ruleSize[56427] << "，cnt= " << (int)counter[56427] << "\n";
		ip_accurate_search(ai, 0, 0, msg.destination_ip[7 - ai]);
	}
//	std::cout << "dstIP: ruleSize[56427]= " << (int)ruleSize[56427] << "，cnt= " << (int)counter[56427] << "\n";
#if TAMA_PORT_LEVEL_NUM < 16
	port_accurate_search_check(8, 0, 0, msg.source_port, rules);
	port_accurate_search_check(9, 0, 0, msg.destination_port, rules);
#else
	port_accurate_search_nocheck(8, 0, 0, msg.source_port);
//	std::cout << "srcPort: ruleSize[56427]= " << (int)ruleSize[56427] << "，cnt= " << (int)counter[56427] << "\n";
	port_accurate_search_nocheck(9, 0, 0, msg.destination_port);
#endif
//	std::cout << "dstPort: ruleSize[56427]= " << (int)ruleSize[56427] << "，cnt= " << (int)counter[56427] << "\n";
	int8_t pi;
	switch (msg.protocol)
	{
	case ICMP:
		pi = 0;
		break;
	case IGMP:
		pi = 1;
		break;
	case GGP:
		pi = 2;
		break;
	case IP:
		pi = 3;
		break;
	case ST:
		pi = 4;
		break;
	case TCP:
		pi = 5;
		break;
	case CBT:
		pi = 6;
		break;
	case EGP:
		pi = 7;
		break;
	case UDP:
		pi = 8;
		break;
	case RSVP:
		pi = 9;
		break;
	case GRE:
		pi = 10;
		break;
	case ESP:
		pi = 11;
		break;
	case AH:
		pi = 12;
		break;
	case EIGRP:
		pi = 13;
		break;
	case OSPFIGP:
		pi = 14;
		break;
	case ISIS:
		pi = 15;
		break;
	default:
		pi = -1;
//		fprintf(stderr, "Message Error - unknown protocol %u !\n", msg.protocol);
	}
	if (pi != -1)
		for (const auto& ri : protocol[pi])
		{
#if TAMA_PRIORITY_CHECK
			if (ri < matchRuleNo)
			{
				counter[ri]--;
				if (counter[ri] == 0)
					matchRuleNo = ri;
#if DEBUG
				totalMinusNum++;
				totalCmpNum++;
#endif
			}
#if DEBUG
			totalCmpNum++;
#endif
#else // not check priority
			counter[ri]--;
#if DEBUG
			totalMinusNum++;
#endif
#endif
		}

#if TAMA_PRIORITY_CHECK
	return matchRuleNo;
#else
	for (uint32_t ri = 0; ri < rules->size; ri++)
	{
#if DEBUG
		totalCmpNum++;
#endif
		if (counter[ri] == 0)
			return ri;
	}
	return -1;
#endif
}

double TamaSearch::calMemory() const
{
	using namespace std;
//	cout << sizeof(ip_port) << "，" << sizeof(ip_port[0]) << ", " << sizeof(ip_port[7][0]) << ", "
//		 << sizeof(ip_port[8][100])\
// << ", " << sizeof(protocol) << ", " << sizeof(protocol[10]) << ", " << sizeof(ipLchild) << ", " << sizeof(portLchild)
//		 << ", "\
// << sizeof(ipMid) << ", " << sizeof(ipMid[0]) << "， " << sizeof(uint16_t) << "\n";
	double ms = 2 * sizeof(ipLchild) + 2 * sizeof(portLchild) + sizeof(ipMid) + sizeof(portMid)\
 + sizeof(ip_port) + sizeof(ip_port[0]) * ip_port.size() + sizeof(ip_port[0][0]) * ip_port[0].size() * ip_port.size()\
 + sizeof(protocol) + sizeof(protocol[0]) * protocol.size() + 2 * sizeof(ruleSize);

	ms += 2LL * sizeof(ipLchild[0]) * ipLchild.size() + 2LL * sizeof(portLchild[0]) * portLchild.size()
		  + sizeof(ipMid[0]) * ipMid.size() + sizeof(portMid[0]) * portMid.size();

	for (const auto& attrNodes : ip_port)
	{
		for (const auto& node : attrNodes)
		{
			ms += sizeof(uint32_t) * node.size();
		}
	}
	for (auto& pi : protocol)
	{
		ms += sizeof(uint16_t) * pi.size();
	}
	ms += 2LL * sizeof(uint8_t) * ruleSize.size();
	return ms;
}

double TamaSearch::statistics() const
{
	uint32_t count[12], sizeSum = 0;
	memset(count, 0, 12 * sizeof(int32_t));
	for (const auto& i : ruleSize)
	{
		count[i]++;
	}
	printf("\n---Rule Size Distribution---\n");
	for (uint32_t i = 0; i < 12; i++)
	{
		sizeSum += i * count[i];
		printf("ruleSize= %d, ruleNumber= %d\n", i, count[i]);
	}
	printf("avgRuleSize= %.2f\n\n", (double)sizeSum / ruleSize.size());
	return sizeSum / ruleSize.size();
}
