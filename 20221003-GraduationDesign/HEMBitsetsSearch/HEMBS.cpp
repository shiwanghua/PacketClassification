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
	delete bitsets;
	delete bitsets2;
	bitsets = nullptr;
	bitsets2 = nullptr;
}

void HEMBS::forward_init_bitsets_IPv4(int numRule)
{
	//printf("sizeof long= %d\n", sizeof(unsigned long));

	numUnit = (numRule + 1 + 63) / 64; // PRI of rule begins from 1 but stored from index 0
//	int numPortBitset = PORT_NUMBER / PORT_WIDTH;
//	int numBitset = 256 * 8 + NUM_PROTOCOL + 1 + numPortBitset*2; // 1: protocol=0
//	printf("\n\n\nNUM_PORT_BITSET= %d, PORT_NUMBER=%d, NUM_BITSET=%d\n\n\n", NUM_PORT_BITSET, PORT_NUMBER, NUM_BITSET);
	bitsets2 = (unsigned long long**)malloc(NUM_BITSET * sizeof(unsigned long long*));
	unsigned long long* bitsSpace = (unsigned long long*)calloc(numUnit * NUM_BITSET, sizeof(unsigned long long));
//	for (int i = 0; i < numUnit * NUM_BITSET; i++)
//		if(bitsSpace[i]!=0){
//			bitsSpace[i] = 0LL;
//			printf("bitsSpace error.\n");
//			return;
//		}

	for (int i = 0; i < NUM_BITSET; i++)
	{
		bitsets2[i] = bitsSpace;
		bitsSpace += numUnit;
	}

	double mf = NUM_BITSET * sizeof(unsigned long long*) + NUM_BITSET * numUnit * sizeof(unsigned long long);
	printf("Memory footprint: %u MB\n", (unsigned int)(mf / 1024.0 / 1024.0));
	fflush(stdout);
}

void HEMBS::forward_bitsets_insert_IPv4(rule* r, unsigned long** bitsets)
{
	int unitNo = r->PRI / 64;
	unsigned long offsetNo = 1LL << r->PRI % 64; // from low bits (right) // Bug: not int type!!!
	unsigned int baseIndex = 0;

	unsigned int mask_by8 = (unsigned int)(r->source_mask >> 3); // 0,1,2,3,4
	for (unsigned int i = 0; i < mask_by8; i++)
	{// 0,1,2,3  i means (i+1)-th byte from left end
		bitsets[baseIndex + (unsigned int)r->source_ip[i]][unitNo] |= offsetNo;
		baseIndex += 256;
	}

//	if(bitsets[135][835]&(1<<29)){
//		printf("uno= %d, offset= %d\n", unitNo, offsetNo);
//	}

	if (mask_by8 < 4)
	{
		int rightShiftBits = 8 - ((unsigned int)r->source_mask & 7);
		unsigned int extraBitNum = (unsigned int)r->source_ip[mask_by8] >> rightShiftBits;
		for (unsigned int i = 0; i < 256; i++)
		{
			if (i >> rightShiftBits == extraBitNum)
			{
				bitsets[baseIndex + i][unitNo] |= offsetNo;
			}
		}
		baseIndex += 256;
		for (unsigned int i = mask_by8 + 1; i < 4; i++)
		{ // 1,2,3
			for (unsigned int i = 0; i < 256; i++)
			{
				bitsets[baseIndex + i][unitNo] |= offsetNo;
			}
			baseIndex += 256;
		}
	}

//	if(bitsets[135][835]&(1<<29)){
//		printf("uno= %d, offset= %d\n", unitNo, offsetNo);
//	}

	mask_by8 = (unsigned int)(r->destination_mask >> 3); // 0,1,2,3,4
	for (unsigned int i = 0; i < mask_by8; i++)
	{// 0,1,2,3
		bitsets[baseIndex +
				(unsigned int)r->destination_ip[i]][unitNo] |= offsetNo; // i means (i+1)-th byte from left end
		baseIndex += 256;
	}
	if (mask_by8 < 4)
	{
		int rightShiftBits = 8 - ((unsigned int)r->destination_mask & 7);
		//printf("mask= %u\nright= %d\n\n", (unsigned int) r->destination_mask, rightShiftBits);

		unsigned int extraBitNum = (unsigned int)r->destination_ip[mask_by8] >> rightShiftBits;
		for (unsigned int i = 0; i < 256; i++)
		{
			if (i >> rightShiftBits == extraBitNum)
			{
				bitsets[baseIndex + i][unitNo] |= offsetNo;
			}
		}
		baseIndex += 256;
		for (unsigned int i = mask_by8 + 1; i < 4; i++)
		{ // 1,2,3
			for (unsigned int id = baseIndex; id < baseIndex + 256; id++)
			{
				bitsets[id][unitNo] |= offsetNo;
			}
			baseIndex += 256;
		}
	}

	for (int i = baseIndex + r->source_port[0] / PORT_WIDTH; i <= baseIndex + r->source_port[1] / PORT_WIDTH; i++)
	{
		bitsets[i][unitNo] |= offsetNo;
	}
	baseIndex += NUM_PORT_BITSET;
	for (int i = baseIndex + r->destination_port[0] / PORT_WIDTH;
		 i <= baseIndex + r->destination_port[1] / PORT_WIDTH; i++)
	{
		bitsets[i][unitNo] |= offsetNo;
	}
	baseIndex += NUM_PORT_BITSET;

	if ((unsigned int)r->protocol[1] == 0)
	{
		for (unsigned int id = baseIndex; id < baseIndex + NUM_PROTOCOL + 1; id++)
		{
			bitsets[id][unitNo] |= offsetNo;
		}
	}
	else
	{
		switch ((unsigned int)r->protocol[1])
		{
		case ICMP:
			bitsets[baseIndex][unitNo] |= offsetNo;
			break;
		case IGMP:
			bitsets[baseIndex + 1][unitNo] |= offsetNo;
			break;
		case GGP:
			bitsets[baseIndex + 2][unitNo] |= offsetNo;
			break;
		case IP:
			bitsets[baseIndex + 3][unitNo] |= offsetNo;
			break;
		case ST:
			bitsets[baseIndex + 4][unitNo] |= offsetNo;
			break;
		case TCP:
			bitsets[baseIndex + 5][unitNo] |= offsetNo;
			break;
		case CBT:
			bitsets[baseIndex + 6][unitNo] |= offsetNo;
			break;
		case EGP:
			bitsets[baseIndex + 7][unitNo] |= offsetNo;
			break;
		case UDP:
			bitsets[baseIndex + 8][unitNo] |= offsetNo;
			break;
		case RSVP:
			bitsets[baseIndex + 9][unitNo] |= offsetNo;
			break;
		case GRE:
			bitsets[baseIndex + 10][unitNo] |= offsetNo;
			break;
		case ESP:
			bitsets[baseIndex + 11][unitNo] |= offsetNo;
			break;
		case AH:
			bitsets[baseIndex + 12][unitNo] |= offsetNo;
			break;
		case EIGRP:
			bitsets[baseIndex + 13][unitNo] |= offsetNo;
			break;
		case OSPFIGP:
			bitsets[baseIndex + 14][unitNo] |= offsetNo;
			break;
		case ISIS:
			bitsets[baseIndex + 15][unitNo] |= offsetNo;
			break;
		default:
			fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", r->PRI, r->protocol[1]);
			return;
		}
	}
}

unsigned int HEMBS::forward_bitsets_search_IPv4(const rule* ruleList, message* m, int* _cycle,
	unsigned long* checkNum)
{
	unsigned int baseIndex = 0;
	unsigned long long* b[11];
	b[0] = bitsets2[baseIndex + (unsigned int)m->source_ip[3]];
	baseIndex += 256;
	b[1] = bitsets2[baseIndex + (unsigned int)m->source_ip[2]];
	baseIndex += 256;
	b[2] = bitsets2[baseIndex + (unsigned int)m->source_ip[1]];
	baseIndex += 256;
	b[3] = bitsets2[baseIndex + (unsigned int)m->source_ip[0]];
	baseIndex += 256;
	b[4] = bitsets2[baseIndex + (unsigned int)m->destination_ip[3]];
	baseIndex += 256;
	b[5] = bitsets2[baseIndex + (unsigned int)m->destination_ip[2]];
	baseIndex += 256;
	b[6] = bitsets2[baseIndex + (unsigned int)m->destination_ip[1]];
	baseIndex += 256;
	b[7] = bitsets2[baseIndex + (unsigned int)m->destination_ip[0]];
	baseIndex += 256;
	b[8] = bitsets2[baseIndex + m->source_port / PORT_WIDTH];
	baseIndex += NUM_PORT_BITSET;
	b[9] = bitsets2[baseIndex + m->destination_port / PORT_WIDTH];
	baseIndex += NUM_PORT_BITSET;

//	b[0] = bitsets2[baseIndex + (unsigned int) m->source_ip[3]];
//	b[1] = bitsets2[baseIndex + 256 + (unsigned int) m->source_ip[2]];
//	b[2] = bitsets2[baseIndex + 512 + (unsigned int) m->source_ip[1]];
//	b[3] = bitsets2[baseIndex + 768 + (unsigned int) m->source_ip[0]];
//	b[4] = bitsets2[baseIndex + 1024 + (unsigned int) m->destination_ip[3]];
//	b[5] = bitsets2[baseIndex + 1280 + (unsigned int) m->destination_ip[2]];
//	b[6] = bitsets2[baseIndex + 1536 + (unsigned int) m->destination_ip[1]];
//	b[7] = bitsets2[baseIndex + 1792 + (unsigned int) m->destination_ip[0]];
//	b[8] = bitsets2[baseIndex + 2048 + m->source_port / PORT_WIDTH];
//	b[9] = bitsets2[baseIndex + 2304 + m->destination_port / PORT_WIDTH];
//	baseIndex = baseIndex + 2560;


	switch ((unsigned int)m->protocol)
	{
	case ICMP:
		b[10] = bitsets2[baseIndex];
		break;
	case IGMP:
		b[10] = bitsets2[baseIndex + 1];
		break;
	case GGP:
		b[10] = bitsets2[baseIndex + 2];
		break;
	case IP:
		b[10] = bitsets2[baseIndex + 3];
		break;
	case ST:
		b[10] = bitsets2[baseIndex + 4];
		break;
	case TCP:
		b[10] = bitsets2[baseIndex + 5];
		break;
	case CBT:
		b[10] = bitsets2[baseIndex + 6];
		break;
	case EGP:
		b[10] = bitsets2[baseIndex + 7];
		break;
	case UDP:
		b[10] = bitsets2[baseIndex + 8];
		break;
	case RSVP:
		b[10] = bitsets2[baseIndex + 9];
		break;
	case GRE:
		b[10] = bitsets2[baseIndex + 10];
		break;
	case ESP:
		b[10] = bitsets2[baseIndex + 11];
		break;
	case AH:
		b[10] = bitsets2[baseIndex + 12];
		break;
	case EIGRP:
		b[10] = bitsets2[baseIndex + 13];
		break;
	case OSPFIGP:
		b[10] = bitsets2[baseIndex + 14];
		break;
	case ISIS:
		b[10] = bitsets2[baseIndex + 15];
		break;
	default:
		b[10] = bitsets2[baseIndex + 16];
//			fprintf(stderr, "Message - Rule %d Error - unknown message protocol %u !\n", m->rule_id, m->protocol);
	}

	unsigned long globalB;//calloc(numUnit,sizeof(unsigned long));
//		unsigned long mask64 = 0xffffffffffffffff;
	unsigned short srcPort = m->source_port, dstPort = m->destination_port;
	baseIndex = 0; // id base
	unsigned int id;
	for (int i = 0; i < numUnit; i++)
	{
		globalB = b[0][i] & b[1][i] & b[2][i] & b[3][i] & b[4][i] & b[5][i] & b[6][i] & b[7][i] & b[8][i] & b[9][i] &
				  b[10][i];
		while (globalB)
		{
#ifdef DEBUG
			(*checkNum)++;
#endif
			id = baseIndex + __builtin_ctzl(globalB); // 求 globalB 中从右数第一个 1 右边０的个数
			if (ruleList[id].source_port[0] <= srcPort &&
				srcPort <= ruleList[id].source_port[1] &&
				ruleList[id].destination_port[0] <= dstPort &&
				dstPort <= ruleList[id].destination_port[1])
			{
				return id;
			}
			globalB = globalB & (globalB - 1);
		}
//		if (globalB) {
//			for (int id = baseIndex; id < baseIndex + 64; id++) {
//				if (globalB & 1) {
//#ifdef DEBUG
//					(*checkNum)++;
//#endif
//					if (ruleList[id].source_port[0] <= srcPort &&
//						srcPort <= ruleList[id].source_port[1] &&
//						ruleList[id].destination_port[0] <= dstPort &&
//						dstPort <= ruleList[id].destination_port[1]) {
////						if (id == 53469) {
////							printf("\n%d\n", id);
////						}
//						return id;
//					}
//				}
//				globalB = globalB >> 1;
//			}
//		}
		baseIndex += 64;
	}
	return -1;
}



//-----------------------------------------------------------------------------------------------------------------------------------------

void HEMBS::backward_init_bitsets_IPv4(int numRule)
{
	if (begin_bits != nullptr) // successive
	{
		delete begin_bits;
	}
	else // not successive
	{
		if (bitsets)
			for (int i = 0; i < 8; i++)
				if (bitsets[i])
					for (int j = 0; j < 256; j++)
						if (bitsets[i][j])
							delete bitsets[i][j];
	}
	if (bitsets)
		for (int i = 0; i < 8; i++)
			delete[] bitsets[i];
	delete bitsets;
	delete bitsets2;
	bitsets = nullptr;
	bitsets2 = nullptr;

	numUnit = (numRule + sizeof(unsigned long long) * 8 - 1) / (sizeof(unsigned long long) * 8);
	bitsets = (unsigned long long***)malloc(8 * sizeof(unsigned long long**));

	// not successive
	// for (int i = 0; i < 8; i++) {
	// 	bitset[i] = (unsigned long long**) malloc(256 * sizeof(unsigned long long*));
	// 	for (int j = 0; j < 256; j++) {
	// 		bitset[i][j] = (unsigned long long*) calloc(numUnit, sizeof(unsigned long long));
	// 	}
	// }

	// successive
	unsigned long long* temp_bits = (unsigned long long*)calloc(numUnit * 256 * 8, sizeof(unsigned long long));
	begin_bits = temp_bits;
	for (int i = 0; i < 8; i++)
	{
		bitsets[i] = (unsigned long long**)malloc(256 * sizeof(unsigned long long*));
		for (int j = 0; j < 256; j++)
		{
			bitsets[i][j] = temp_bits;
			temp_bits += numUnit;
		}
	}

	// for protocol attribute
	// bitsets[8] = (unsigned long long**) malloc(15 * sizeof(unsigned long long*));
	// for(int j=0;j<15;j++){
	// 	bitsets[8][j]=temp_bits;
	// 	temp_bits+=numUnit;
	// }
	// memset(bitsets[8][14],1, numUnit*8);

	memorysize = (sizeof(unsigned long long***) + 8 * sizeof(unsigned long long**)
				  + numUnit * sizeof(unsigned long long) * (256 * 8)) / 1024.0 / 1024.0; // MB
}


// void HEMBS::backward_bitsets_insert_IPv4( rule* r) {

// 	int unitNo=r->PRI / 64;
// 	unsigned long long offsetMask=1LL<<(r->PRI % 64);

// 	for (int i = 0; i < 8; i++) {
// 		if (i < 4) {
// 			if (r->source_mask/8 > i) {
// 				for (int j = 0; j < r->source_ip[3-i]; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 				for (int j = r->source_ip[3 - i] + 1; j < 256; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 			}
// 			else if (r->source_mask / 8 == i) {
// 				// k=4: d=10101100 (10100000~10101111)(160~175)(0~159,176~255) // 160+255-176+1=240
// 				// d k
// 				// lowValue=(d>>k)<<k
// 				// highValue=lowValue+((1<<k)-1)
// 				// k=19
// 				// 0~7 鏍囷拷??255锟??锟??
// 				// 8~15 鏍囷拷??255锟??锟??
// 				// 16~24 锟??4锟??鍥哄畾浣嶏紝4锟??浠绘剰浣嶏紝浠绘剰浣嶅彲鏋勬垚16绉嶅€硷紝浠ｈ〃杩欎釜瀛楄妭涓婄殑鍖洪棿瀹藉害锟??16锛屽尮閰嶏拷?锟界巼锟??16/256锛岄渶瑕佹爣锟??240锟??
// 				int k = 8*(i+1) - r->source_mask;//鐏垫椿浣嶆暟
// 				int d = r->source_ip[3 - i];
// 				int lowValue = (d >> k) << k;
// 				int highValue = lowValue + (1 << k) - 1;
// 				for (int j = 0; j < lowValue; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 				for (int j = highValue + 1; j < 256; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 			}
// 		}
// 		else { // i= 4 5 6 7
// 			if (r->destination_mask / 8 > i - 4) {
// 				for (int j = 0; j < r->destination_ip[7 - i]; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 				for (int j = r->destination_ip[7 - i] + 1; j < 256; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 			}
// 			else if (r->destination_mask / 8 == (i - 4)) { // 0 1 2 3
// 				// printf("dMask=%d, i=%d\n", list->destination_mask, i);
// 				// k=4: d=10101100 (10100000~10101111)(160~175)(0~159,176~255) // 160+255-176+1=240
// 				// d k
// 				// lowValue=(d>>k)<<k
// 				// highValue=lowValue+((1<<k)-1)
// 				// k=19
// 				// 0~7 鏍囷拷??255锟??锟??
// 				// 8~15 鏍囷拷??255锟??锟??
// 				// 16~24 锟??4锟??鍥哄畾浣嶏紝4锟??浠绘剰浣嶏紝浠绘剰浣嶅彲鏋勬垚16绉嶅€硷紝浠ｈ〃杩欎釜瀛楄妭涓婄殑鍖洪棿瀹藉害锟??16锛屽尮閰嶏拷?锟界巼锟??16/256锛岄渶瑕佹爣锟??240锟??
// 				int k = 8*(i-3) - r->destination_mask;//鐏垫椿浣嶆暟
// 				int d = r->destination_ip[7 - i];
// 				int lowValue = (d >> k) << k;
// 				// int highValue = lowValue + (1 << k) - 1;
// 				for (int j = 0; j < lowValue; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 				for (int j = lowValue + (1 << k); j < 256; j++) {
// 					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
// 				}
// 			}
// 		}
// 	}

// 	if ((unsigned int) r->protocol[1] == 0) {
// 		bitsets[8][14][unitNo]&=~offsetMask;
// 	} else {
// 		int pno=-1;
// 		switch ((unsigned int) r->protocol[1]) {
// 			case ICMP:
// 				pno=0;
// 				break;
// 			case IGMP:
// 				pno=1;
// 				break;
// 			case GGP:
// 				pno=2;
// 				break;
// 			case IP:
// 				pno=3;
// 				break;
// 			case ST:
// 				pno=4;
// 				break;
// 			case TCP:
// 				pno=5;
// 				break;
// 			case CBT:
// 				pno=6;
// 				break;
// 			case EGP:
// 				pno=7;
// 				break;
// 			case UDP:
// 				pno=8;
// 				break;
// 			case GRE:
// 				pno=9;
// 				break;
// 			case ESP:
// 				pno=10;
// 				break;
// 			case AH:
// 				pno=11;
// 				break;
// 			case EIGRP:
// 				pno=12;
// 				break;
// 			case OSPFIGP:
// 				pno=13;
// 				break;
// 			default:
// 				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", r->PRI, r->protocol[1]);
// 				return;
// 		}
// 		for(int i=0;i<14;i++){ // 15 is OK.
// 			if(i!=pno)
// 				bitsets[8][i][unitNo]|=offsetMask;
// 		}
// 	}
// }

// int HEMBS::backward_bitsets_search_IPv4( message* msg, ACL_rules* rules) {

// 	unsigned long long int* result[9]=
// 	{
// 		bitsets[0][msg->source_ip[3]],
// 		bitsets[1][msg->source_ip[2]],
// 		bitsets[2][msg->source_ip[1]],
// 		bitsets[3][msg->source_ip[0]],
// 		bitsets[4][msg->destination_ip[3]],
// 		bitsets[5][msg->destination_ip[2]],
// 		bitsets[6][msg->destination_ip[1]],
// 		bitsets[7][msg->destination_ip[0]]
// 	};

// 	// unsigned long long int* result[9]=
// 	// {
// 	// 	NULL,
// 	// 	bitsets[1][msg->source_ip[2]],
// 	// 	bitsets[5][msg->destination_ip[2]],
// 	// 	bitsets[6][msg->destination_ip[1]],
// 	// 	bitsets[0][msg->source_ip[3]],
// 	// 	bitsets[2][msg->source_ip[1]],
// 	// 	bitsets[3][msg->source_ip[0]],
// 	// 	bitsets[4][msg->destination_ip[3]],
// 	// 	bitsets[7][msg->destination_ip[0]]
// 	// };

// 	switch ((unsigned int) msg->protocol) {
// 			case ICMP:
// 				result[8]=bitsets[8][0];
// 				break;
// 			case IGMP:
// 				result[8]=bitsets[8][1];
// 				break;
// 			case GGP:
// 				result[8]=bitsets[8][2];
// 				break;
// 			case IP:
// 				result[8]=bitsets[8][3];
// 				break;
// 			case ST:
// 				result[8]=bitsets[8][4];
// 				break;
// 			case TCP:
// 				result[8]=bitsets[8][5];
// 				break;
// 			case CBT:
// 				result[8]=bitsets[8][6];
// 				break;
// 			case EGP:
// 				result[8]=bitsets[8][7];
// 				break;
// 			case UDP:
// 				result[8]=bitsets[8][8];
// 				break;
// 			case GRE:
// 				result[8]=bitsets[8][9];
// 				break;
// 			case ESP:
// 				result[8]=bitsets[8][10];
// 				break;
// 			case AH:
// 				result[8]=bitsets[8][11];
// 				break;
// 			case EIGRP:
// 				result[8]=bitsets[8][12];
// 				break;
// 			case OSPFIGP:
// 				result[8]=bitsets[8][13];
// 				break;
// 			default:
// 				result[8]=bitsets[8][14];
// 				// fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", r->PRI, r->protocol[1]);
// 		}

// 	// for (int i = 0; i < 8; i++) {
// 	// 	if (i < 4) {
// 	// 		int bitset_2d = (int)msg->source_ip[3-i];
// 	// 		result[i] = bitsets[i][bitset_2d];
// 	// 	}
// 	// 	else{
// 	// 		int bitset_2d = (int)msg->destination_ip[7-i];
// 	// 		result[i] = bitsets[i][bitset_2d];
// 	// 	}
// 	// }

// 	// int checkNum=0;
// 	for (int j = 0; j < numUnit; j++) {

// 		unsigned long long int orResult=result[0][j];
// 		int tmp = 1;
// 		int flag = 1;
// 		while (tmp < 9)
// 		{
// 			orResult = orResult | result[tmp][j];
// 			if (orResult == ~0) {
// 				flag = 0;
// 				break;
// 			}
// 			tmp++;
// 		}
// 		if (flag) {

// 			for(int i=0, ruleNo=j*64;i<64;i++,ruleNo++){
// 				// if (ruleNo > rules->size)
// 				// 	return -1;
// 				// else
// 				if((1LL<<i & orResult)==0){
// 					// checkNum++;
// 					rule _r;
// 					memcpy(&_r, rules->list + ruleNo, sizeof(rule));
// 					if ((_r.source_port[0] <= msg->source_port)&&(msg->source_port <= _r.source_port[1])
// 					&&(_r.destination_port[0] <= msg->destination_port)&&(msg->destination_port <= _r.destination_port[1])) {
// 							return ruleNo;
// 					}
// 				}
// 			}

// 			// int ruleNo=j*64;
// 			// orResult=~orResult; // several 1s
// 			// rule _r;
// 			// while(orResult){
// 			// 	int ruleNoI = ruleNo + __builtin_ctz(orResult);
// 			// 	memcpy(&_r, rules->list + ruleNoI, sizeof(rule));
// 			// 	if ((_r.source_port[0] <= list->source_port)&&(list->source_port <= _r.source_port[1])
// 			// 	&&(_r.destination_port[0] <= list->destination_port)&&(list->destination_port <= _r.destination_port[1])) {
// 			// 			if ((_r.protocol[1] == list->protocol) || (_r.protocol[0] == 0)) {
// 			// 				return ruleNoI;
// 			// 			}
// 			// 	}
// 			// 	orResult=orResult&(orResult-1);
// 			// }
// 		}

// 		// unsigned long long int orResult=result[0][j]|result[1][j]|result[2][j]|result[3][j]|result[4][j]|result[5][j]|result[6][j]|result[7][j];
// 		// unsigned long long int k = 1;
// 		// for(int i=0;i<64;i++,ruleNo++){
// 		// 	if((k<<i & orResult)==0){
// 		// 		rule _r;
// 		// 		memcpy(&_r, rules->list + ruleNo, sizeof(rule));
// 		// 		if ((_r.source_port[0] <= list->source_port)&&(list->source_port <= _r.source_port[1])
// 		// 		&&(_r.destination_port[0] <= list->destination_port)&&(list->destination_port <= _r.destination_port[1])) {
// 		// 				if ((_r.protocol[1] == list->protocol) || (_r.protocol[0] == 0)) {
// 		// 					return ruleNo;
// 		// 				}
// 		// 		}
// 		// 	}
// 		// }
// 	}
// 	// printf("Error: unfound match.\n");
// 	return -1;
// }

void HEMBS::backward_bitsets_insert_IPv4(const rule* r)
{

	int unitNo = r->PRI / 64;
	unsigned long long offsetMask = 1LL << (r->PRI % 64);

	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			if (r->source_mask / 8 > i)
			{
				for (int j = 0; j < r->source_ip[3 - i]; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
				for (int j = r->source_ip[3 - i] + 1; j < 256; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
			}
			else if (r->source_mask / 8 == i)
			{
				// k=4: d=10101100 (10100000~10101111)(160~175)(0~159,176~255) // 160+255-176+1=240
				// d k
				// lowValue=(d>>k)<<k
				// highValue=lowValue+((1<<k)-1)
				// k=19
				int k = 8 * (i + 1) - r->source_mask;
				int d = r->source_ip[3 - i];
				int lowValue = (d >> k) << k;
				int highValue = lowValue + (1 << k) - 1;
				for (int j = 0; j < lowValue; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
				for (int j = highValue + 1; j < 256; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
			}
		}
		else
		{ // i= 4 5 6 7
			if (r->destination_mask / 8 > i - 4)
			{
				for (int j = 0; j < r->destination_ip[7 - i]; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
				for (int j = r->destination_ip[7 - i] + 1; j < 256; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
			}
			else if (r->destination_mask / 8 == (i - 4))
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
				int k = 8 * (i - 3) - r->destination_mask;
				int d = r->destination_ip[7 - i];
				int lowValue = (d >> k) << k;
				// int highValue = lowValue + (1 << k) - 1;
				for (int j = 0; j < lowValue; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
				for (int j = lowValue + (1 << k); j < 256; j++)
				{
					bitsets[i][j][unitNo] = bitsets[i][j][unitNo] | offsetMask;
				}
			}
		}
	}
}

unsigned int HEMBS::backward_bitsets_search_IPv4(const message* msg, const ACL_rules* rules)
{

	unsigned long long int* result[8]; //=
	// {
	// 	bitsets[0][msg->source_ip[3]],
	// 	bitsets[1][msg->source_ip[2]],
	// 	bitsets[2][msg->source_ip[1]],
	// 	bitsets[3][msg->source_ip[0]],
	// 	bitsets[4][msg->destination_ip[3]],
	// 	bitsets[5][msg->destination_ip[2]],
	// 	bitsets[6][msg->destination_ip[1]],
	// 	bitsets[7][msg->destination_ip[0]]
	// };

	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			int bitset_2d = (int)msg->source_ip[3 - i];
			result[i] = bitsets[i][bitset_2d];
		}
		else
		{
			int bitset_2d = (int)msg->destination_ip[7 - i];
			result[i] = bitsets[i][bitset_2d];
		}
	}

	// int checkNum=0;
	for (int j = 0; j < numUnit; j++)
	{
		unsigned long long int orResult = result[0][j];
		int tmp = 1;
		int flag = 1;
		while (tmp < 8)
		{
			orResult = orResult | result[tmp][j];
			if (orResult == ~0ULL)
			{
				flag = 0;
				break;
			}
			tmp++;
		}
		if (flag)
		{
			for (int i = 0, ruleNo = j * 64; i < 64; i++, ruleNo++)
			{
				// if (ruleNo > rules->size)
				// 	return -1;
				// else
				if ((1LL << i & orResult) == 0)
				{
					// checkNum++;
					rule _r;
					memcpy(&_r, rules->list + ruleNo, sizeof(rule));
					if ((_r.source_port[0] <= msg->source_port) && (msg->source_port <= _r.source_port[1])
						&& (_r.destination_port[0] <= msg->destination_port)
						&& (msg->destination_port <= _r.destination_port[1]))
					{
						if ((_r.protocol[1] == msg->protocol) || (_r.protocol[0] == 0))
						{
							if (ruleNo < rules->size)
								return ruleNo;
							else return -1;
							// return checkNum;
						}
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
	return -1;
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
		for (int j = baseIndex; j < baseIndex + NUM_PORT_BITSET; j++)
		{
			for (int k = 0; k < numUnit; k++)
				if (bitsets[j][k] == 0)
					zeroCount[i]++;
		}
		baseIndex += NUM_PORT_BITSET;
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
		numUnit * NUM_PORT_BITSET - zeroCount[8], (double)zeroCount[8] / (double)(numUnit * NUM_PORT_BITSET));
	printf("dstPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[9],
		numUnit * NUM_PORT_BITSET - zeroCount[9], (double)zeroCount[9] / (double)(numUnit * NUM_PORT_BITSET));
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
	// 	oneCount[i] = new int[NUM_PORT_BITSET];
	// 	for (int j = 0; j < NUM_PORT_BITSET; j++)
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
	// 		   numUnit * NUM_PORT_BITSET - zeroCount[8], (double) zeroCount[8] / (double) (numUnit * NUM_PORT_BITSET));
	// 	printf("dstPort: 64-bits zeros = %d, other = %d, ratio = %.4f\n", zeroCount[9],
	// 		   numUnit * NUM_PORT_BITSET - zeroCount[9], (double) zeroCount[9] / (double) (numUnit * NUM_PORT_BITSET));
	// 	printf("Protocol: 64-bits zeros = %d, other = %d, ratio = %.4f\n\n", zeroCount[10],
	// 		   numUnit * (NUM_PROTOCOL + 1) - zeroCount[10],
	// 		   (double) zeroCount[10] / (double) (numUnit * (NUM_PROTOCOL + 1)));

	fclose(fto);
	for (int i = 0; i < 11; i++)
	{
		delete[] oneCount[i];
	}
}

double HEMBS::memory_size()
{
	return memorysize;
}