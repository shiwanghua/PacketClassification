//
// Created by swh on 22-10-4.
//

#include "HEMBS.h"

HEMBS::HEMBS()
{
	begin_bits = nullptr;
	bitsets = nullptr;
	bitsets2 = nullptr;
	numUnit = 0;
	memorysize = 0;
}

HEMBS::~HEMBS()
{
	if (begin_bits != nullptr)
	{
		delete begin_bits;
	}
	else
	{
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 256; j++)
				if (bitsets[i][j])
					delete bitsets[i][j];
	}
	for (int i = 0; i < 8; i++)
		delete[] bitsets[i];
	if (bitsets)
		delete bitsets;
	if (bitsets2)
		delete bitsets2;
	bitsets = nullptr;
	bitsets2 = nullptr;
}

void HEMBS::forward_init_bitsets_IPv4(int numRule)
{
	if (begin_bits != nullptr) // successive
		delete begin_bits;
	else // begin_bits is null, so bitsets is not successive
	{
		if (bitsets)
			for (uint32_t i = 0; i < 8; i++)
				if (bitsets[i])
					for (uint32_t j = 0; j < 256; j++)
						if (bitsets[i][j])
							delete bitsets[i][j];
	}
	if (bitsets)
		for (uint32_t i = 0; i < HEM_BS_NUM_ATTR; i++)
			delete[] bitsets[i];
	delete bitsets;

	// not successive
	// for (int i = 0; i < 8; i++) {
	// 	bitset[i] = (unsigned long long**) malloc(256 * sizeof(unsigned long long*));
	// 	for (int j = 0; j < 256; j++) {
	// 		bitset[i][j] = (unsigned long long*) calloc(numUnit, sizeof(unsigned long long));
	// 	}
	// }

	// successive
#if HEM_BS_NUM_ATTR == 8
	uint32_t numBitsets = 8 * 256;
#elif HEM_BS_NUM_ATTR == 9
	uint32_t numBitsets = 256 * 8 + NUM_PROTOCOL + 1;
#else // HEM_BS_ATTR_NUM==11
	uint32_t numBitsets = 8 * 256 + NUM_PROTOCOL + 1 + 2 * HEM_BS_NUM_PORT_BITSET;
#endif

	numUnit = (numRule + sizeof(unsigned long long) * 8 - 1) / (sizeof(unsigned long long) * 8);
	memorysize = (sizeof(unsigned long long***) + sizeof(unsigned long long**) * HEM_BS_NUM_ATTR
				  + sizeof(unsigned long long*) * numBitsets
				  + sizeof(unsigned long long) * numUnit * numBitsets); // B

	unsigned long long* temp_bits = (unsigned long long*)calloc(numUnit * numBitsets, sizeof(unsigned long long));
	begin_bits = temp_bits;
	bitsets = (unsigned long long***)malloc(HEM_BS_NUM_ATTR * sizeof(unsigned long long**));
	for (uint32_t i = 0; i < 8; i++)
	{
		bitsets[i] = (unsigned long long**)malloc(256 * sizeof(unsigned long long*));
		for (uint32_t j = 0; j < 256; j++)
		{
			bitsets[i][j] = temp_bits;
			temp_bits += numUnit;
		}
	}

#if HEM_BS_NUM_ATTR > 8
	bitsets[8] = (unsigned long long**)malloc(sizeof(unsigned long long*) * (NUM_PROTOCOL + 1));
	for (uint32_t j = 0; j < (NUM_PROTOCOL + 1); j++)
	{
		bitsets[8][j] = temp_bits;
		temp_bits += numUnit;
	}
#endif

#if HEM_BS_NUM_ATTR == 11
	for (uint32_t ai = 9; ai < 11; ai++)
	{
		bitsets[ai] = (unsigned long long**)malloc(sizeof(unsigned long long*) * (HEM_BS_NUM_PORT_BITSET));
		for (uint32_t j = 0; j < HEM_BS_NUM_PORT_BITSET; j++)
		{
			bitsets[ai][j] = temp_bits;
			temp_bits += numUnit;
		}
	}
#endif
}

void HEMBS::forward_bitsets_insert_IPv4(const rule* r)
{
	uint32_t unitNo = r->PRI / 64;
	unsigned long long offsetMask = 1LL << r->PRI % 64; // ruleID from low bits (right) to high bits (left)

	uint32_t mask_by8 = (uint32_t)(r->source_mask >> 3); // 0,1,2,3,4
	for (uint32_t ai = 0; ai < mask_by8; ai++)
	{// 0,1,2,3 attribute  i means the (i+1)-th byte from high/right end
		bitsets[ai][(uint32_t)r->source_ip[3 - ai]][unitNo] |= offsetMask; // 多做了次减法，在读数据时用大端存储可避免；或者让低位字节为0号属性
	}

	if (mask_by8 < 4)
	{
		uint32_t rightShiftBits = 8 - ((uint32_t)r->source_mask & 7); // 临界字节（从高位起第mask_by8+1个字节）上的自由位数
		uint32_t lowValue =
			((uint32_t)r->source_ip[3 - mask_by8] >> rightShiftBits) << rightShiftBits; // Bug1: 少了 “3-”！
		for (uint32_t bi = lowValue; bi < lowValue + (1 << rightShiftBits); bi++)
			bitsets[mask_by8][bi][unitNo] |= offsetMask;
		for (uint32_t ai = mask_by8 + 1; ai < 4; ai++)
		{ // 1,2,3 attribute
			for (uint32_t bi = 0; bi < 256; bi++)
				bitsets[ai][bi][unitNo] |= offsetMask;
		}
	}

	mask_by8 = (uint32_t)(r->destination_mask >> 3); // 0,1,2,3,4
	for (uint32_t ai = 0; ai < mask_by8; ai++)
	{// 0,1,2,3 attribute i means the (i+1)-th byte from high/right end
		bitsets[4 + ai][(uint32_t)r->destination_ip[3 - ai]][unitNo] |= offsetMask;
	}
	if (mask_by8 < 4)
	{
		uint32_t freeBitsNum = 8 - ((uint32_t)r->destination_mask & 7);
		uint32_t lowValue = ((uint32_t)r->destination_ip[3 - mask_by8] >> freeBitsNum) << freeBitsNum;
		for (uint32_t bi = lowValue; bi < lowValue + (1 << freeBitsNum); bi++)
			bitsets[4 + mask_by8][bi][unitNo] |= offsetMask; // Bug2: 少加了4！
		for (uint32_t ai = mask_by8 + 5; ai < 8; ai++)
		{ // 5,6,7
			for (uint32_t bi = 0; bi < 256; bi++)
				bitsets[ai][bi][unitNo] |= offsetMask;
		}
	}

#if HEM_BS_NUM_ATTR > 8 // insert into protocol layer
	if ((uint32_t)r->protocol[1] == 0)
	{
		for (uint32_t bi = 0; bi < NUM_PROTOCOL + 1; bi++)
			bitsets[8][bi][unitNo] |= offsetMask;
	}
	else
	{
		switch ((uint32_t)r->protocol[1])
		{
		case ICMP:
			bitsets[8][0][unitNo] |= offsetMask;
			break;
		case IGMP:
			bitsets[8][1][unitNo] |= offsetMask;
			break;
		case GGP:
			bitsets[8][2][unitNo] |= offsetMask;
			break;
		case IP:
			bitsets[8][3][unitNo] |= offsetMask;
			break;
		case ST:
			bitsets[8][4][unitNo] |= offsetMask;
			break;
		case TCP:
			bitsets[8][5][unitNo] |= offsetMask;
			break;
		case CBT:
			bitsets[8][6][unitNo] |= offsetMask;
			break;
		case EGP:
			bitsets[8][7][unitNo] |= offsetMask;
			break;
		case UDP:
			bitsets[8][8][unitNo] |= offsetMask;
			break;
		case RSVP:
			bitsets[8][9][unitNo] |= offsetMask;
			break;
		case GRE:
			bitsets[8][10][unitNo] |= offsetMask;
			break;
		case ESP:
			bitsets[8][11][unitNo] |= offsetMask;
			break;
		case AH:
			bitsets[8][12][unitNo] |= offsetMask;
			break;
		case EIGRP:
			bitsets[8][13][unitNo] |= offsetMask;
			break;
		case OSPFIGP:
			bitsets[8][14][unitNo] |= offsetMask;
			break;
		case ISIS:
			bitsets[8][15][unitNo] |= offsetMask;
			break;
		default:
			fprintf(stderr, "Rule %d Error - unknown rule protocol %u !\n", r->PRI, r->protocol[1]);
			exit(0);
		}
	}
#endif

#if HEM_BS_NUM_ATTR == 11 // insert into port layer
	for (int bi = r->source_port[0] / HEM_BS_PORT_CELLWIDTH;
		 bi <= r->source_port[1] / HEM_BS_PORT_CELLWIDTH; bi++)
		bitsets[9][bi][unitNo] |= offsetMask;

	for (int bi = r->destination_port[0] / HEM_BS_PORT_CELLWIDTH;
		 bi <= r->destination_port[1] / HEM_BS_PORT_CELLWIDTH; bi++)
		bitsets[10][bi][unitNo] |= offsetMask;
#endif
}

std::array<uint64_t, 2>
HEMBS::forward_bitsets_search_IPv4(const message* msg, const rule* rules, uint32_t& matchRuleNo)
{
	unsigned long long int* b[HEM_BS_NUM_ATTR] =
		{
			bitsets[0][msg->source_ip[3]],
			bitsets[1][msg->source_ip[2]],
			bitsets[2][msg->source_ip[1]],
			bitsets[3][msg->source_ip[0]],
			bitsets[4][msg->destination_ip[3]],
			bitsets[5][msg->destination_ip[2]],
			bitsets[6][msg->destination_ip[1]],
			bitsets[7][msg->destination_ip[0]]
		};

#if HEM_BS_NUM_ATTR > 8
	switch ((unsigned int)msg->protocol)
	{
	case ICMP:
		b[8] = bitsets[8][0];
		break;
	case IGMP:
		b[8] = bitsets[8][1];
		break;
	case GGP:
		b[8] = bitsets[8][2];
		break;
	case IP:
		b[8] = bitsets[8][3];
		break;
	case ST:
		b[8] = bitsets[8][4];
		break;
	case TCP:
		b[8] = bitsets[8][5];
		break;
	case CBT:
		b[8] = bitsets[8][6];
		break;
	case EGP:
		b[8] = bitsets[8][7];
		break;
	case UDP:
		b[8] = bitsets[8][8];
		break;
	case RSVP:
		b[8] = bitsets[8][9];
		break;
	case GRE:
		b[8] = bitsets[8][10];
		break;
	case ESP:
		b[8] = bitsets[8][11];
		break;
	case AH:
		b[8] = bitsets[8][12];
		break;
	case EIGRP:
		b[8] = bitsets[8][13];
		break;
	case OSPFIGP:
		b[8] = bitsets[8][14];
		break;
	case ISIS:
		b[8] = bitsets[8][15];
		break;
	default:
		b[8] = bitsets[8][NUM_PROTOCOL];
//		 fprintf(stderr, "Matching rule %d - unknown message protocol %u !\n", msg->PRI, msg->protocol[1]);
	}
#endif

#if HEM_BS_NUM_ATTR == 11
	b[9] = bitsets[9][msg->source_port / HEM_BS_PORT_CELLWIDTH];
	b[10] = bitsets[10][msg->destination_port / HEM_BS_PORT_CELLWIDTH];
#endif

#if DEBUG
	uint64_t checkNum = 0, and64Num = 0;
#endif

	for (int bi = 0; bi < numUnit; bi++)
	{
//		unsigned long long int andResult = b[0][bi] & b[1][bi] & b[2][bi] & b[3][bi] & b[4][bi]\
// & b[5][bi] & b[6][bi] & b[7][bi] & b[8][bi] & b[9][bi] & b[10][bi];
		unsigned long long int andResult = b[0][bi];
		uint8_t ai = 1;
		// V1: 先检查反而慢一些...？
//		while (ai < HEM_BS_NUM_ATTR)
//		{
//			if (andResult == 0)
//				break;
//			andResult &= b[ai][bi];
//			ai++;
//		}

//	V2: 先运算后检查
		while (ai < HEM_BS_NUM_ATTR)
		{
			andResult &= b[ai][bi];
			if (andResult == 0)
				break;
			ai++;
		}

		// V3:
//		uint8_t a = 1;
//		unsigned long long int andResult0 = 0;
//		unsigned long long int andResult1 = 0;
//		unsigned long long int andResult2 = 0;
//		unsigned long long int andResult3 = 0;
//		while (andResult != ~0ULL && a + 3 < 11) {
//			andResult0 &= b[a][bi]; // 1,5
//			andResult1 &= b[a + 1][bi];
//			andResult2 &= b[a + 2][bi];
//			andResult3 &= b[a + 3][bi];
//			a += 4;
//			andResult = andResult0 & andResult1 & andResult2 & andResult3;
//		}
//		while(a < 11) {
//			andResult |= b[a][bi];
//			++a;
//		}

#if DEBUG
//		and64Num += ai - 1; // V1
				and64Num += std::min(10, (int)ai); // V2
#endif

//    __builtin_ctzl 找位判断版本
		//		while (andResult)
//		{
//#if DEBUG
//			checkNum++;
//#endif
//			matchRuleNo = (bi << 6) + __builtin_ctzl(andResult); // 求 andResult 中从右数第一个 1 右边０的个数
//			if (rules[matchRuleNo].source_port[0] <= msg->source_port &&
//				msg->source_port <= rules[matchRuleNo].source_port[1] &&
//				rules[matchRuleNo].destination_port[0] <= msg->destination_port &&
//				msg->destination_port <= rules[matchRuleNo].destination_port[1])
//			{
//#if HEM_BS_NUM_ATTR > 8
//				#if DEBUG
//						return std::array<uint64_t, 2>{ checkNum, and64Num };
//#else
//						return std::array<uint64_t, 2>{ -1ULL, -1ULL };
//#endif
//#else // HEM_BS_NUM_ATTR == 8
//				if ((rules[matchRuleNo].protocol[1] == msg->protocol) || (rules[matchRuleNo].protocol[0] == 0))
//				{
//#if DEBUG
//					return std::array<uint64_t, 2>{ checkNum, and64Num };
//#else
//					return std::array<uint64_t, 2>{ -1ULL, -1ULL };
//#endif
//				}
//#endif
//			}
//			andResult = andResult & (andResult - 1);
//		}

//    for循环迭代64次找位判断版本
		if (andResult)
		{
			for (matchRuleNo = bi << 6; matchRuleNo < (bi << 6) + 64; matchRuleNo++)
			{
				if (andResult & 1)
				{
#if  DEBUG
					checkNum++;
#endif
					if (rules[matchRuleNo].source_port[0] <= msg->source_port &&
						msg->source_port <= rules[matchRuleNo].source_port[1] &&
						rules[matchRuleNo].destination_port[0] <= msg->destination_port &&
						msg->destination_port <= rules[matchRuleNo].destination_port[1])
					{
#if HEM_BS_NUM_ATTR > 8
#if DEBUG
						return std::array<uint64_t, 2>{ checkNum, and64Num };
#else
						return std::array<uint64_t, 2>{ -1ULL, -1ULL };
#endif
#else // HEM_BS_NUM_ATTR == 8
						if ((rules[matchRuleNo].protocol[1] == msg->protocol) || (rules[matchRuleNo].protocol[0] == 0))
						{
#if DEBUG
							return std::array<uint64_t, 2>{ checkNum, and64Num };
#else
							return std::array<uint64_t, 2>{ -1ULL, -1ULL };
#endif
						}
#endif
					}
				}
				andResult = andResult >> 1;
			}
		}

	}
	return { -1ULL, -1ULL };
}



//---------------------------------------------------------backward-----------------------------------------------------------------------------------------

void HEMBS::backward_init_bitsets_IPv4(int numRule)
{
	if (begin_bits != nullptr) // successive
		delete begin_bits;
	else // begin_bits is null, so bitsets is not successive
	{
		if (bitsets)
			for (uint32_t i = 0; i < 8; i++)
				if (bitsets[i])
					for (uint32_t j = 0; j < 256; j++)
						if (bitsets[i][j])
							delete bitsets[i][j];
	}
	if (bitsets)
		for (uint32_t i = 0; i < HEM_BS_NUM_ATTR; i++)
			delete[] bitsets[i];
	delete bitsets;

	// not successive
	// for (int i = 0; i < 8; i++) {
	// 	bitset[i] = (unsigned long long**) malloc(256 * sizeof(unsigned long long*));
	// 	for (int j = 0; j < 256; j++) {
	// 		bitset[i][j] = (unsigned long long*) calloc(numUnit, sizeof(unsigned long long));
	// 	}
	// }

	// successive
#if HEM_BS_NUM_ATTR == 8
	uint32_t numBitsets = 8 * 256;
#elif HEM_BS_NUM_ATTR == 9
	uint32_t numBitsets = 256 * 8 + NUM_PROTOCOL + 1;
#else // HEM_BS_ATTR_NUM==11
	uint32_t numBitsets = 8 * 256 + NUM_PROTOCOL + 1 + 2 * HEM_BS_NUM_PORT_BITSET;
#endif

	numUnit = (numRule + sizeof(unsigned long long) * 8 - 1) / (sizeof(unsigned long long) * 8);
	memorysize = (sizeof(unsigned long long***) + sizeof(unsigned long long**) * HEM_BS_NUM_ATTR
				  + sizeof(unsigned long long*) * numBitsets
				  + sizeof(unsigned long long) * numUnit * numBitsets); // B

	unsigned long long* temp_bits = (unsigned long long*)calloc(numUnit * (numBitsets), sizeof(unsigned long long));
	begin_bits = temp_bits;
	bitsets = (unsigned long long***)malloc(HEM_BS_NUM_ATTR * sizeof(unsigned long long**));
	for (uint32_t i = 0; i < 8; i++)
	{
		bitsets[i] = (unsigned long long**)malloc(256 * sizeof(unsigned long long*));
		for (uint32_t j = 0; j < 256; j++)
		{
			bitsets[i][j] = temp_bits;
			temp_bits += numUnit;
		}
	}

#if HEM_BS_NUM_ATTR > 8
	bitsets[8] = (unsigned long long**)malloc(sizeof(unsigned long long*) * (NUM_PROTOCOL + 1));
	for (uint32_t j = 0; j < (NUM_PROTOCOL + 1); j++)
	{
		bitsets[8][j] = temp_bits;
		temp_bits += numUnit;
	}
	memset(bitsets[8][NUM_PROTOCOL], -1,
		sizeof(unsigned long long) * numUnit); // Init to all ones, means all mismatching.
#endif

#if HEM_BS_NUM_ATTR == 11
	for (uint32_t ai = 9; ai < 11; ai++)
	{
		bitsets[ai] = (unsigned long long**)malloc(sizeof(unsigned long long*) * (HEM_BS_NUM_PORT_BITSET));
		for (uint32_t j = 0; j < HEM_BS_NUM_PORT_BITSET; j++)
		{
			bitsets[ai][j] = temp_bits;
			temp_bits += numUnit;
		}
	}
#endif
}

void HEMBS::backward_bitsets_insert_IPv4(const rule* r)
{
	uint32_t unitNo = r->PRI / 64;
	unsigned long long offsetMask = 1ULL << (r->PRI % 64);

	for (uint32_t i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			if (r->source_mask / 8 > i)
			{
				for (uint32_t j = 0; j < r->source_ip[3 - i]; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				for (uint32_t j = r->source_ip[3 - i] + 1; j < 256; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
			}
			else if (r->source_mask / 8 == i)
			{
				// k=4: d=10101100 (10100000~10101111) match interval
				// unmatch interval: (160~175) (0~159,176~255) // 160+255-176+1=240
				// lowValue=(d>>k)<<k
				// highValue=lowValue+((1<<k)-1)
				uint8_t k = ((i + 1) << 3) - r->source_mask;
				uint32_t d = r->source_ip[3 - i];
				uint32_t lowValue = (d >> k) << k;
				for (uint32_t j = 0; j < lowValue; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				for (uint32_t j = lowValue + (1 << k); j < 256; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
			}
		}
		else
		{ // i= 4 5 6 7
			if (r->destination_mask / 8 > i - 4) // must be this fixed 8 bits
			{
				for (uint32_t j = 0; j < r->destination_ip[7 - i]; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;

				for (uint32_t j = r->destination_ip[7 - i] + 1; j < 256; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
			}
			else if (r->destination_mask / 8 == (i - 4)) // 0 1 2 3 4; 0 1 2 3
			{ // 0 1 2 3
				// printf("dMask=%d, i=%d\n", list->destination_mask, i);
				// k=4: d=10101100 (10100000~10101111)(160~175)(0~159,176~255) // 160+255-176+1=240
				// d k
				// lowValue=(d>>k)<<k
				// highValue=lowValue+((1<<k)-1)
				// k=19
				// 0~7
				// 8~15
				// 16~24
				uint32_t k = 8 * (i - 3) - r->destination_mask; // 8 16 24 32;
				uint32_t d = r->destination_ip[7 - i];
				uint32_t lowValue = (d >> k) << k;
				// int highValue = lowValue + (1 << k) - 1;
				for (uint32_t j = 0; j < lowValue; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				for (uint32_t j = lowValue + (1 << k); j < 256; j++)
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
			}
		}
	}

#if HEM_BS_NUM_ATTR > 8 // insert into protocol layer
	if ((unsigned int)r->protocol[1] == 0)
	{ // No requirement.
		bitsets[8][NUM_PROTOCOL][unitNo] &= ~offsetMask; // set this bit to zero, always matching.
	}
	else
	{
		uint8_t pno;
		switch ((unsigned int)r->protocol[1])
		{
		case ICMP:
			pno = 0;
			break;
		case IGMP:
			pno = 1;
			break;
		case GGP:
			pno = 2;
			break;
		case IP:
			pno = 3;
			break;
		case ST:
			pno = 4;
			break;
		case TCP:
			pno = 5;
			break;
		case CBT:
			pno = 6;
			break;
		case EGP:
			pno = 7;
			break;
		case UDP:
			pno = 8;
			break;
		case RSVP:
			pno = 9;
			break;
		case GRE:
			pno = 10;
			break;
		case ESP:
			pno = 11;
			break;
		case AH:
			pno = 12;
			break;
		case EIGRP:
			pno = 13;
			break;
		case OSPFIGP:
			pno = 14;
			break;
		case ISIS:
			pno = 15;
			break;
		default:
			fprintf(stderr, "Rule %d Error - unknown rule protocol %u !\n", r->PRI, r->protocol[1]);
			exit(0);
		}
		for (uint8_t i = 0; i < NUM_PROTOCOL; i++)
		{ // 15 is OK.
			if (i != pno)
				bitsets[8][i][unitNo] |= offsetMask;
		}
	}
#endif

#if HEM_BS_NUM_ATTR == 11 // insert into port layer
	for (uint32_t bi = 0; bi < r->source_port[0] / HEM_BS_PORT_CELLWIDTH; bi++)
		bitsets[9][bi][unitNo] |= offsetMask;

	for (uint32_t bi = r->source_port[1] / HEM_BS_PORT_CELLWIDTH + 1; bi < HEM_BS_NUM_PORT_BITSET; bi++)
		bitsets[9][bi][unitNo] |= offsetMask;

	for (uint32_t bi = 0; bi < r->destination_port[0] / HEM_BS_PORT_CELLWIDTH; bi++)
		bitsets[10][bi][unitNo] |= offsetMask;

	for (uint32_t bi = r->destination_port[1] / HEM_BS_PORT_CELLWIDTH + 1; bi < HEM_BS_NUM_PORT_BITSET; bi++)
		bitsets[10][bi][unitNo] |= offsetMask;
#endif
}

std::array<uint64_t, 2>
HEMBS::backward_bitsets_search_IPv4(const message* msg, const rule* rules, uint32_t& matchRuleNo)
{
	unsigned long long int* result[HEM_BS_NUM_ATTR] =
		{
			bitsets[0][msg->source_ip[3]],
			bitsets[1][msg->source_ip[2]],
			bitsets[2][msg->source_ip[1]],
			bitsets[3][msg->source_ip[0]],
			bitsets[4][msg->destination_ip[3]],
			bitsets[5][msg->destination_ip[2]],
			bitsets[6][msg->destination_ip[1]],
			bitsets[7][msg->destination_ip[0]]
		};

//	for (uint8_t i = 0; i < 8; i++)
//	{
//		if (i < 4)
//		{
//			uint8_t bitset_2d = msg->source_ip[3 - i];
//			result[i] = bitsets[i][bitset_2d];
//		}
//		else
//		{
//			uint8_t bitset_2d = msg->destination_ip[7 - i];
//			result[i] = bitsets[i][bitset_2d];
//		}
//	}

#if HEM_BS_NUM_ATTR > 8
	switch ((unsigned int)msg->protocol)
	{
	case ICMP:
		result[8] = bitsets[8][0];
		break;
	case IGMP:
		result[8] = bitsets[8][1];
		break;
	case GGP:
		result[8] = bitsets[8][2];
		break;
	case IP:
		result[8] = bitsets[8][3];
		break;
	case ST:
		result[8] = bitsets[8][4];
		break;
	case TCP:
		result[8] = bitsets[8][5];
		break;
	case CBT:
		result[8] = bitsets[8][6];
		break;
	case EGP:
		result[8] = bitsets[8][7];
		break;
	case UDP:
		result[8] = bitsets[8][8];
		break;
	case RSVP:
		result[8] = bitsets[8][9];
		break;
	case GRE:
		result[8] = bitsets[8][10];
		break;
	case ESP:
		result[8] = bitsets[8][11];
		break;
	case AH:
		result[8] = bitsets[8][12];
		break;
	case EIGRP:
		result[8] = bitsets[8][13];
		break;
	case OSPFIGP:
		result[8] = bitsets[8][14];
		break;
	case ISIS:
		result[8] = bitsets[8][15];
		break;
	default:
		result[8] = bitsets[8][NUM_PROTOCOL];
//		 fprintf(stderr, "Matching rule %d - unknown message protocol %u !\n", msg->PRI, msg->protocol[1]);
	}
#endif
#if HEM_BS_NUM_ATTR == 11
	result[9] = bitsets[9][msg->source_port / HEM_BS_PORT_CELLWIDTH];
	result[10] = bitsets[10][msg->destination_port / HEM_BS_PORT_CELLWIDTH];
#endif

#if DEBUG
	uint64_t checkNum = 0, or64Num = 0;
#endif
	for (uint32_t j = 0; j < numUnit; j++)
	{
		unsigned long long int orResult = result[0][j];
		uint8_t ai = 1;
		bool flag = true;
		// V1
//		while (ai < HEM_BS_NUM_ATTR)
//		{
//			if (orResult == ~0ULL) // All the 64 bits are mismatching
//			{
//				flag = false;
//				break;
//			}
//			orResult |= result[ai][j];
//			ai++;
//		}

//		V2
		while (ai < HEM_BS_NUM_ATTR)
		{
			orResult |= result[ai][j];
			if (orResult == ~0ULL) // All the 64 bits are mismatching
			{
				flag = false;
				break;
			}
			ai++;
		}

#if DEBUG
//		or64Num += ai - 1; // V1
				or64Num += std::min(10, (int)ai); // V2
#endif
		if (flag)
		{
			matchRuleNo = j << 6;
			for (uint32_t i = 0; i < 64; i++, matchRuleNo++)
			{
				if ((1LL << i & orResult) == 0) // may be a match
				{
#if DEBUG
					checkNum++;
#endif
					if ((rules[matchRuleNo].source_port[0] <= msg->source_port)
						&& (msg->source_port <= rules[matchRuleNo].source_port[1])
						&& (rules[matchRuleNo].destination_port[0] <= msg->destination_port)
						&& (msg->destination_port <= rules[matchRuleNo].destination_port[1]))
					{
#if HEM_BS_NUM_ATTR > 8
#if DEBUG
						return std::array<uint64_t, 2>{ checkNum, or64Num };
#else
						return std::array<uint64_t, 2>{ -1ULL, -1ULL };
#endif
#else // HEM_BS_NUM_ATTR == 8
						if ((rules[matchRuleNo].protocol[1] == msg->protocol) || (rules[matchRuleNo].protocol[0] == 0))
						{
							// add an "all zero" rule to the last row, so there is always a matched rule.
#if DEBUG
							return std::array<uint64_t, 2>{ checkNum, or64Num };
#else
							return 	std::array<uint64_t ,2> {-1ULL,-1ULL};
#endif
						}
#endif
					}
				}
			}

			// int ruleNo=j*64;
			// orResult=~orResult; // several 1s
			// rule _r;
			// while(orResult){
			// 	int ruleNoI = ruleNo + __builtin_ctz(orResult);
			// 	memcpy(&_r, rules->msg + ruleNoI, sizeof(rule));
			// 	if ((_r.source_port[0] <= msg->source_port)&&(msg->source_port <= _r.source_port[1])
			// 	&&(_r.destination_port[0] <= msg->destination_port)&&(msg->destination_port <= _r.destination_port[1])) {
			// 			if ((_r.protocol[1] == msg->protocol) || (_r.protocol[0] == 0)) {
			// 				return ruleNoI;
			// 			}
			// 	}
			// 	orResult=orResult&(orResult-1);
			// }
		}

		// unsigned long long int orResult=result[0][j]|result[1][j]|result[2][j]|result[3][j]|result[4][j]|result[5][j]|result[6][j]|result[7][j];
		// unsigned long long int k = 1;
		// for(int i=0;i<64;i++,ruleNo++){
		// 	if((k<<i & orResult)==0){
		// 		rule _r;
		// 		memcpy(&_r, rules->msg + ruleNo, sizeof(rule));
		// 		if ((_r.source_port[0] <= msg->source_port)&&(msg->source_port <= _r.source_port[1])
		// 		&&(_r.destination_port[0] <= msg->destination_port)&&(msg->destination_port <= _r.destination_port[1])) {
		// 				if ((_r.protocol[1] == msg->protocol) || (_r.protocol[0] == 0)) {
		// 					return ruleNo;
		// 				}
		// 		}
		// 	}
		// }
	}
	// printf("Error: unfound match.\n");
	return { -1ULL, -1ULL };
}

void HEMBS::visualize_bitsets(unsigned long long** bitsets)
{ // forward
	int zeroCount[11], baseIndex = 0;
	memset(zeroCount, 0, sizeof(zeroCount));
	for (int i = 0; i < 8; i++)
	{ // IP
		for (int j = baseIndex; j < baseIndex + 256; j++)
		{
			for (int k = 0; k < numUnit; k++)
				if (bitsets[j][k] == 0)
					zeroCount[i]++;
		}
		baseIndex += 256;
	}
	for (int i = 8; i < 10; i++)
	{ // Port
		for (int j = baseIndex; j < baseIndex + HEM_BS_NUM_PORT_BITSET; j++)
		{
			for (int k = 0; k < numUnit; k++)
				if (bitsets[j][k] == 0)
					zeroCount[i]++;
		}
		baseIndex += HEM_BS_NUM_PORT_BITSET;
	}

	for (int j = baseIndex; j < baseIndex + NUM_PROTOCOL + 1; j++)
		for (int k = 0; k < numUnit; k++)
			if (bitsets[j][k] == 0)
				zeroCount[10]++; // Protocol

	printf("\n");
	for (int i = 0; i < 4; i++)
		printf("srcIP=%d: 64-bits zeros = %d, other = %d, ratio = %.4f\n", i + 1, zeroCount[i],
			numUnit * 256 - zeroCount[i], (double)zeroCount[i] / (double)(numUnit * 256));
	for (int i = 4; i < 8; i++)
		printf("dstIP=%d: 64-bits zeros = %d, other = %d, ratio = %.4f\n", i - 3, zeroCount[i],
			numUnit * 256 - zeroCount[i], (double)zeroCount[i] / (double)(numUnit * 256));

	printf("srcPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[8],
		numUnit * HEM_BS_NUM_PORT_BITSET - zeroCount[8],
		(double)zeroCount[8] / (double)(numUnit * HEM_BS_NUM_PORT_BITSET));
	printf("dstPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[9],
		numUnit * HEM_BS_NUM_PORT_BITSET - zeroCount[9],
		(double)zeroCount[9] / (double)(numUnit * HEM_BS_NUM_PORT_BITSET));
	printf("Protocol: 64-bits zeros = %d, other = %d, ratio = %.4f\n\n", zeroCount[10],
		numUnit * (NUM_PROTOCOL + 1) - zeroCount[10],
		(double)zeroCount[10] / (double)(numUnit * (NUM_PROTOCOL + 1)));
}

void HEMBS::backward_bitsets_visualize_one(const char* ruleSetName)
{
	const unsigned long long int ones = ~0LL;
	int* oneCount[11] = { nullptr };
	int oneSum[8];
	memset(oneSum, 0, sizeof(int) * 8);
	for (int i = 0; i < 8; i++)
	{ // IP
		oneCount[i] = new int[256];
		memset(oneCount[i], 0, sizeof(int) * 256); // sizeof(zeroCount[i])=8Bytes
		for (int j = 0; j < 256; j++)
		{
			for (int k = 0; k < numUnit; k++)
				if (bitsets[i][j][k] == ones)
					oneCount[i][j]++;
			oneSum[i] += oneCount[i][j];
		}
	}

	// for (int i = 8; i < 10; i++) // Port
	// {
	// 	oneCount[i] = new int[HEM_BS_NUM_PORT_BITSET];
	// 	for (int j = 0; j < HEM_BS_NUM_PORT_BITSET; j++)
	// 	{
	// 		for (int k = 0; k < numUnit; k++)
	// 			if (bitsets[i][j][k] == ones)
	// 				oneCount[i][j]++;
	// 	}
	// }

	// oneCount[10] = new int[NUM_PROTOCOL + 1];
	// for (int j = 0; j < NUM_PROTOCOL + 1; j++)
	// 	for (int k = 0; k < numUnit; k++)
	// 		if (bitsets[10][j][k] == ones)
	// 			oneCount[10][j]++; // Protocol

	const char* filename = "backward_bitsets_visualization.txt";
	FILE* fto = fopen(filename, "a+");
	fprintf(fto, "%s:\n", ruleSetName);

	fprintf(fto, "attr1-srcIP3: ones-%d, notallones-%d, percent-%.2f\n", oneSum[0],
		256 * numUnit - oneSum[0], (double)oneSum[0] / (256 * numUnit));
	fprintf(fto, "attr2-srcIP2: ones-%d, notallones-%d, percent-%.2f\n", oneSum[1],
		256 * numUnit - oneSum[1], (double)oneSum[1] / (256 * numUnit));
	fprintf(fto, "attr3-srcIP1: ones-%d, notallones-%d, percent-%.2f\n", oneSum[2],
		256 * numUnit - oneSum[2], (double)oneSum[2] / (256 * numUnit));
	fprintf(fto, "attr4-srcIP0: ones-%d, notallones-%d, percent-%.2f\n", oneSum[3],
		256 * numUnit - oneSum[3], (double)oneSum[3] / (256 * numUnit));
	fprintf(fto, "attr5-dstIP3: ones-%d, notallones-%d, percent-%.2f\n", oneSum[4],
		256 * numUnit - oneSum[4], (double)oneSum[4] / (256 * numUnit));
	fprintf(fto, "attr6-dstIP2: ones-%d, notallones-%d, percent-%.2f\n", oneSum[5],
		256 * numUnit - oneSum[5], (double)oneSum[5] / (256 * numUnit));
	fprintf(fto, "attr7-dstIP1: ones-%d, notallones-%d, percent-%.2f\n", oneSum[6],
		256 * numUnit - oneSum[6], (double)oneSum[6] / (256 * numUnit));
	fprintf(fto, "attr8-dstIP0: ones-%d, notallones-%d, percent-%.2f\n\n", oneSum[7],
		256 * numUnit - oneSum[7], (double)oneSum[7] / (256 * numUnit));

	for (int i = 0; i < 8; i++)
	{
		fprintf(fto, "attr%d=[ ", i + 1);
		for (int j = 0; j < 255; j++)
		{
			fprintf(fto, "%d, ", oneCount[i][j]);
		}
		fprintf(fto, "%d ]\n", oneCount[i][255]);
	}
	fprintf(fto, "\nEvery bitsets: 0/1 %dunits\n", numUnit);
	for (int i = 0; i < 8; i++)
	{
		fprintf(fto, "attr%d_bitset=[\n", i + 1);
		for (int j = 0; j < 256; j++)
		{
			fprintf(fto, " [");
			for (int k = 0; k < numUnit - 1; k++)
			{
				fprintf(fto, "%d,", bitsets[i][j][k] == ones ? 1 : 0);
			}
			fprintf(fto, "%d]", bitsets[i][j][numUnit - 1] == ones ? 1 : 0);
			if (j < 255)
				fprintf(fto, ",\n");
			else fprintf(fto, "\n");
		}
		fprintf(fto, "]\n");
	}
	fprintf(fto, "\n\n\n");

	// 	for (int i = 0; i < 4; i++)
	// 		printf("srcIP=%d: 64-bits zeros = %d, other = %d, ratio = %.4f\n", i + 1, zeroCount[i],
	// 			   numUnit * 256 - zeroCount[i], (double) zeroCount[i] / (double) (numUnit * 256));
	// 	for (int i = 4; i < 8; i++)
	// 		printf("dstIP=%d: 64-bits zeros = %d, other = %d, ratio = %.4f\n", i - 3, zeroCount[i],
	// 			   numUnit * 256 - zeroCount[i], (double) zeroCount[i] / (double) (numUnit * 256));

	// 	printf("srcPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[8],
	// 		   numUnit * HEM_BS_NUM_PORT_BITSET - zeroCount[8], (double) zeroCount[8] / (double) (numUnit * HEM_BS_NUM_PORT_BITSET));
	// 	printf("dstPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[9],
	// 		   numUnit * HEM_BS_NUM_PORT_BITSET - zeroCount[9], (double) zeroCount[9] / (double) (numUnit * HEM_BS_NUM_PORT_BITSET));
	// 	printf("Protocol: 64-bits zeros = %d, other = %d, ratio = %.4f\n\n", zeroCount[10],
	// 		   numUnit * (NUM_PROTOCOL + 1) - zeroCount[10],
	// 		   (double) zeroCount[10] / (double) (numUnit * (NUM_PROTOCOL + 1)));

	fclose(fto);
	for (int i = 0; i < 11; i++)
	{
		delete[] oneCount[i];
	}
}

double HEMBS::calMemory()
{
	return memorysize;
}