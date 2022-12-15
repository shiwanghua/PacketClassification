#include"MultiLevelHash.h"

void MultiLevelHash::insert(Cell* c_list, rule* p)
{
//	rule *p = r;
	unsigned int c_id[LEVEL]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));

#if SIP_1 || SIP_2 || SIP_3 || SIP_4
	unsigned int s_mask = (unsigned int)(p->source_mask >> 3);
#endif
#if DIP_1 || DIP_2 || DIP_3 || DIP_4
	unsigned int d_mask = (unsigned int) (p->destination_mask >> 3);
#endif // DIP_1 || DIP_2 || DIP_3 || DIP_4

#if PROTO
	if ((unsigned int)p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else
	{
		switch ((unsigned int)p->protocol[1])
		{
		case TCP:
			c_id[PROTO_LAYER] = 0;
			break;
		case ICMP:
			c_id[PROTO_LAYER] = 1;
			break;
		case UDP:
			c_id[PROTO_LAYER] = 2;
			break;
		default:
			c_id[PROTO_LAYER] = PROTO_END_CELL;
			break;
		}
	}
#endif
#if SIP_1
	if (s_mask > 0)c_id[SIP_1_LAYER] = p->source_ip[3] >> SIP_WIDTH_1;
	else c_id[SIP_1_LAYER] = SIP_EDN_CELL_1;
#endif
#if SIP_2
	if (s_mask > 1)c_id[SIP_2_LAYER] = p->source_ip[2] >> SIP_WIDTH_2;
	else c_id[SIP_2_LAYER] = SIP_EDN_CELL_2;
#endif
#if SIP_3
	if (s_mask > 2)c_id[SIP_3_LAYER] = p->source_ip[1] >> SIP_WIDTH_3;
	else c_id[SIP_3_LAYER] = SIP_EDN_CELL_3;
#endif
#if SIP_4
	if (s_mask > 3)c_id[SIP_4_LAYER] = p->source_ip[0] >> SIP_WIDTH_4;
	else c_id[SIP_4_LAYER] = SIP_EDN_CELL_4;
#endif
#if DIP_1
	if (d_mask > 0)c_id[DIP_1_LAYER] = p->destination_ip[3] >> DIP_WIDTH_1;
	else c_id[DIP_1_LAYER] = DIP_EDN_CELL_1;
#endif
#if DIP_2
	if (d_mask > 1)c_id[DIP_2_LAYER] = p->destination_ip[2] >> DIP_WIDTH_2;
	else c_id[DIP_2_LAYER] = DIP_EDN_CELL_2;
#endif
#if DIP_3
	if (d_mask > 2)c_id[DIP_3_LAYER] = p->destination_ip[1] >> DIP_WIDTH_3;
	else c_id[DIP_3_LAYER] = DIP_EDN_CELL_3;
#endif
#if DIP_4
	if (d_mask > 3)c_id[DIP_4_LAYER] = p->destination_ip[0] >> DIP_WIDTH_4;
	else c_id[DIP_4_LAYER] = DIP_EDN_CELL_4;
#endif
#if SPORT
	if (p->source_port[0] >> SPORT_WIDTH == p->source_port[1] >> SPORT_WIDTH)
		c_id[SPORT_LAYER] = p->source_port[0] >> SPORT_WIDTH;
	else c_id[SPORT_LAYER] = SPORT_END_CELL;
#endif
#if DPORT
	if (p->destination_port[0] >> DPORT_WIDTH == p->destination_port[1] >> DPORT_WIDTH)
		c_id[DPORT_LAYER] = p->destination_port[0] >> DPORT_WIDTH;
	else c_id[DPORT_LAYER] = DPORT_END_CELL;
#endif

	int id = ((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3];
	//printf("%d %d\n", p->PRI, id);
	//for(int i=0;i<LEVEL;i++)printf("%d ", c_id[i]);
	//printf("\n");

	add_data_ordered(c_list + (((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3]), &_d);
}

void MultiLevelHash::insert_cache(Cell* c_list, rule* p, unsigned char** bitset)
{
	unsigned int c_id[LEVEL]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));

#if SIP_1 || SIP_2 || SIP_3 || SIP_4
	unsigned int s_mask = (unsigned int)(p->source_mask >> 3);
#endif
#if DIP_1 || DIP_2 || DIP_3 || DIP_4
	unsigned int d_mask = (unsigned int) (p->destination_mask >> 3);
#endif // DIP_1 || DIP_2 || DIP_3 || DIP_4

#if PROTO
	if ((unsigned int)p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else
	{
		switch ((unsigned int)p->protocol[1])
		{
		case TCP:
			c_id[PROTO_LAYER] = 0;
			break;
		case ICMP:
			c_id[PROTO_LAYER] = 1;
			break;
		case UDP:
			c_id[PROTO_LAYER] = 2;
			break;
		default:
			c_id[PROTO_LAYER] = PROTO_END_CELL;
			break;
		}
	}
#endif
#if SIP_1
	if (s_mask > 0)c_id[SIP_1_LAYER] = p->source_ip[3] >> SIP_WIDTH_1;
	else c_id[SIP_1_LAYER] = SIP_EDN_CELL_1;
#endif
#if SIP_2
	if (s_mask > 1)c_id[SIP_2_LAYER] = p->source_ip[2] >> SIP_WIDTH_2;
	else c_id[SIP_2_LAYER] = SIP_EDN_CELL_2;
#endif
#if SIP_3
	if (s_mask > 2)c_id[SIP_3_LAYER] = p->source_ip[1] >> SIP_WIDTH_3;
	else c_id[SIP_3_LAYER] = SIP_EDN_CELL_3;
#endif
#if SIP_4
	if (s_mask > 3)c_id[SIP_4_LAYER] = p->source_ip[0] >> SIP_WIDTH_4;
	else c_id[SIP_4_LAYER] = SIP_EDN_CELL_4;
#endif
#if DIP_1
	if (d_mask > 0)c_id[DIP_1_LAYER] = p->destination_ip[3] >> DIP_WIDTH_1;
	else c_id[DIP_1_LAYER] = DIP_EDN_CELL_1;
#endif
#if DIP_2
	if (d_mask > 1)c_id[DIP_2_LAYER] = p->destination_ip[2] >> DIP_WIDTH_2;
	else c_id[DIP_2_LAYER] = DIP_EDN_CELL_2;
#endif
#if DIP_3
	if (d_mask > 2)c_id[DIP_3_LAYER] = p->destination_ip[1] >> DIP_WIDTH_3;
	else c_id[DIP_3_LAYER] = DIP_EDN_CELL_3;
#endif
#if DIP_4
	if (d_mask > 3)c_id[DIP_4_LAYER] = p->destination_ip[0] >> DIP_WIDTH_4;
	else c_id[DIP_4_LAYER] = DIP_EDN_CELL_4;
#endif
#if SPORT
	if (p->source_port[0] >> SPORT_WIDTH == p->source_port[1] >> SPORT_WIDTH)
		c_id[SPORT_LAYER] = p->source_port[0] >> SPORT_WIDTH;
	else c_id[SPORT_LAYER] = SPORT_END_CELL;
#endif
#if DPORT
	if (p->destination_port[0] >> DPORT_WIDTH == p->destination_port[1] >> DPORT_WIDTH)
		c_id[DPORT_LAYER] = p->destination_port[0] >> DPORT_WIDTH;
	else c_id[DPORT_LAYER] = DPORT_END_CELL;
#endif

	int id = ((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3];
	//printf("%d %d\n", p->PRI, id);
	//for(int i=0;i<LEVEL;i++)printf("%d ", c_id[i]);
	//printf("\n");

//	if (p->destination_mask == 32) {
//		int bitsetNo = (int) p->destination_ip[0];
//		int unitNo = p->PRI / 8;
//		int offset = p->PRI % 8;
//		for (int i = 0; i < 256; i++) {
//			if (i != bitsetNo)
//				bitset[i][unitNo] = bitset[i][unitNo] | 1 << offset;
//		}
//	}

	if (p->destination_mask > 7)
	{
		int bitsetNo = (int)p->destination_ip[3];
		int unitNo = p->PRI / 8;
		int offset = p->PRI & 7;
		for (int i = 0; i < 256; i++)
		{
			if (i != bitsetNo)
				bitset[i][unitNo] = bitset[i][unitNo] | 1 << offset;
		}
//		printf("%d %d %d\n",bitsetNo,unitNo,offset);
	}

//	if ((unsigned int) p->protocol[0] != 0){
//		int bitsetNo=-1;
//		int unitNo=p->PRI/8;
//		int offset=p->PRI&7;
//		switch ((unsigned int) p->protocol[1]) {
//			case ICMP:
//				bitsetNo=0;
//				break;
//			case IGMP:
//				bitsetNo = 1;
//				break;
//			case GGP:
//				bitsetNo = 2;
//				break;
//			case IP:
//				bitsetNo = 3;
//				break;
//			case ST:
//				bitsetNo = 4;
//				break;
//			case TCP:
//				bitsetNo = 5;
//				break;
//			case CBT:
//				bitsetNo = 6;
//				break;
//			case EGP:
//				bitsetNo = 7;
//				break;
//			case UDP:
//				bitsetNo = 8;
//				break;
//			case GRE:
//				bitsetNo = 9;
//				break;
//			case ESP:
//				bitsetNo = 10;
//				break;
//			case AH:
//				bitsetNo = 11;
//				break;
//			case EIGRP:
//				bitsetNo = 12;
//				break;
//			case OSPFIGP:
//				bitsetNo = 13;
//				break;
//			default:
//				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
//				return;
//		}
//		for (int i = 0; i < 14; i++) {
//			if(i!=bitsetNo)
//				bitset[i][unitNo] = bitset[i][unitNo] | 1 << offset;
//		}
//	}
	add_data_ordered(c_list + (((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3]), &_d);
}

int MultiLevelHash::match_with_log(Cell* c_list, message* m, int* _cycle, MatchLog* log)
{
	/*
	printf("try to match %u.%u.%u.%u %u.%u.%u.%u %u %u %u %u\n",
		m->source_ip[3], m->source_ip[2], m->source_ip[1], m->source_ip[0],
		m->destination_ip[3], m->destination_ip[2], m->destination_ip[1], m->destination_ip[0],
		m->source_port, m->destination_port, m->protocol, m->rule_id);
	*/

//	uint64_t time_1, time_2;
//
//	time_1 = GetCPUCycle();
	Cell* _c = c_list;
	message* p = m;
	unsigned int es_ip, ed_ip;
	unsigned char e_protocol;
	unsigned short es_port, ed_port;
	int res;

//	for (int num = 0; num < 100; num++) {
#if ENABLE_LOG
	log->rules = log->ele = 0;
	memset(log->list, 0, 16 * sizeof(LogInCell));
#endif
	e_protocol = p->protocol;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	es_port = p->source_port;
	ed_port = p->destination_port;

	unsigned int c_id[LEVEL][2];

#if PROTO
	switch ((unsigned int)p->protocol)
	{
	case TCP:
		c_id[PROTO_LAYER][0] = 0;
		break;
	case ICMP:
		c_id[PROTO_LAYER][0] = 1;
		break;
	case UDP:
		c_id[PROTO_LAYER][0] = 2;
		break;
	default:
		c_id[PROTO_LAYER][0] = PROTO_END_CELL;
		break;
	}
	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
#endif
#if SIP_1
	c_id[SIP_1_LAYER][0] = p->source_ip[3] >> SIP_WIDTH_1;
	c_id[SIP_1_LAYER][1] = SIP_EDN_CELL_1;
#endif
#if SIP_2
	c_id[SIP_2_LAYER][0] = p->source_ip[2] >> SIP_WIDTH_2;
	c_id[SIP_2_LAYER][1] = SIP_EDN_CELL_2;
#endif
#if SIP_3
	c_id[SIP_3_LAYER][0] = p->source_ip[1] >> SIP_WIDTH_3;
	c_id[SIP_3_LAYER][1] = SIP_EDN_CELL_3;
#endif
#if SIP_4
	c_id[SIP_4_LAYER][0] = p->source_ip[0] >> SIP_WIDTH_4;
	c_id[SIP_4_LAYER][1] = SIP_EDN_CELL_4;
#endif
#if DIP_1
	c_id[DIP_1_LAYER][0] = p->destination_ip[3] >> DIP_WIDTH_1;
	c_id[DIP_1_LAYER][1] = DIP_EDN_CELL_1;
#endif
#if DIP_2
	c_id[DIP_2_LAYER][0] = p->destination_ip[2] >> DIP_WIDTH_2;
	c_id[DIP_2_LAYER][1] = DIP_EDN_CELL_2;
#endif
#if DIP_3
	c_id[DIP_3_LAYER][0] = p->destination_ip[1] >> DIP_WIDTH_3;
	c_id[DIP_3_LAYER][1] = DIP_EDN_CELL_3;
#endif
#if DIP_4
	c_id[DIP_4_LAYER][0] = p->destination_ip[0] >> DIP_WIDTH_4;
	c_id[DIP_4_LAYER][1] = DIP_EDN_CELL_4;
#endif
#if SPORT
	c_id[SPORT_LAYER][0] = p->source_port >> SPORT_WIDTH;
	c_id[SPORT_LAYER][1] = SPORT_END_CELL;
#endif
#if DPORT
	c_id[DPORT_LAYER][0] = p->destination_port >> DPORT_WIDTH;
	c_id[DPORT_LAYER][1] = DPORT_END_CELL;
#endif

	res = 0x7FFFFFFF;

#if ENABLE_LOG
	int cell_num = -1;
#endif

	for (int i = 0; i < 2; i++)
	{
		int id_1 = c_id[0][i] * LAYER_1;
		for (int j = 0; j < 2; j++)
		{
			int id_2 = (id_1 + c_id[1][j]) * LAYER_2;
			for (int v = 0; v < 2; v++)
			{
				int id_3 = (id_2 + c_id[2][v]) * LAYER_3;
				for (int w = 0; w < 2; w++)
				{
					Cell* q = _c + id_3 + c_id[3][w];
//						int _size = q->size;

#if ENABLE_LOG
					cell_num++;
					log->list[cell_num].id = id_3 + c_id[3][w];
					log->list[cell_num].size = _size;
					log->list[cell_num].layer[0] = c_id[0][i];
					log->list[cell_num].layer[1] = c_id[1][j];
					log->list[cell_num].layer[2] = c_id[2][v];
					log->list[cell_num].layer[3] = c_id[3][w];
#endif

//						if (q->size == 0)continue;
					data* _d = q->list;

//					for (int u = 0; u < q->size; u++, ++_d){
//						*_cycle++;
//					}
					for (int u = 0; u < q->size; u++, ++_d)
					{ //check in cell


#if ENABLE_LOG
						log->rules++;
					log->list[cell_num].rules++;
					//__builtin_prefetch(_d + 4, 0);
					log->ele++;
					log->list[cell_num].ele++;
#endif

						if (res < _d->PRI)break; // check priority

#if ENABLE_LOG
						//						log->ele++;
						//					log->list[cell_num].ele++;
#endif

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele++;
#endif
						unsigned int _ip;
						unsigned int m_bit =
							32 - (unsigned int)_d->destination_mask;  // comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
							continue;  // if destination ip not match, check next

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele++;
#endif

						m_bit = 32 - (unsigned int)_d->source_mask;  // comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (m_bit != 32 && es_ip >> m_bit != _ip >> m_bit)
							continue;  // if source ip not match, check next

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele++;
#endif

						if (e_protocol != _d->protocol[1] && _d->protocol[0] != 0 ||
							ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port ||
							es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  // if destination port not match, check next

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele+=3;
#endif

						res = _d->PRI;
#if ENABLE_LOG
						log->list[cell_num].match = res;
#endif
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;
//		++p;
//	}

//	time_2 = GetCPUCycle();
//
//	*_cycle = time_2 - time_1;
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

int MultiLevelHash::match_with_log_cache(Cell* c_list, message* m, unsigned char** bitset, int* _cycle, MatchLog* log)
{
	//	uint64_t time_1, time_2;
//
//	time_1 = GetCPUCycle();
	Cell* _c = c_list;
	message* p = m;
	unsigned int es_ip, ed_ip;
	unsigned char e_protocol;
	unsigned short es_port, ed_port;
	int res;

//	for (int num = 0; num < 100; num++) {
#if ENABLE_LOG
	log->rules = log->ele = 0;
	memset(log->list, 0, 16 * sizeof(LogInCell));
#endif
	e_protocol = p->protocol;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	es_port = p->source_port;
	ed_port = p->destination_port;
	int bitsetNo = (int)p->destination_ip[3];
//	int bitsetNo;//=(int) p->destination_ip[3];
//	switch (e_protocol) {
//		case ICMP:
//			bitsetNo = 0;
//			break;
//		case IGMP:
//			bitsetNo = 1;
//			break;
//		case GGP:
//			bitsetNo = 2;
//			break;
//		case IP:
//			bitsetNo = 3;
//			break;
//		case ST:
//			bitsetNo = 4;
//			break;
//		case TCP:
//			bitsetNo = 5;
//			break;
//		case CBT:
//			bitsetNo = 6;
//			break;
//		case EGP:
//			bitsetNo = 7;
//			break;
//		case UDP:
//			bitsetNo = 8;
//			break;
//		case GRE:
//			bitsetNo = 9;
//			break;
//		case ESP:
//			bitsetNo = 10;
//			break;
//		case AH:
//			bitsetNo = 11;
//			break;
//		case EIGRP:
//			bitsetNo = 12;
//			break;
//		case OSPFIGP:
//			bitsetNo = 13;
//			break;
//	}
	unsigned char* bitset_ = bitset[bitsetNo];

	unsigned int c_id[LEVEL][2];

#if PROTO
	switch ((unsigned int)p->protocol)
	{
	case TCP:
		c_id[PROTO_LAYER][0] = 0;
		break;
	case ICMP:
		c_id[PROTO_LAYER][0] = 1;
		break;
	case UDP:
		c_id[PROTO_LAYER][0] = 2;
		break;
	default:
		c_id[PROTO_LAYER][0] = PROTO_END_CELL;
		break;
	}
	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
#endif
#if SIP_1
	c_id[SIP_1_LAYER][0] = p->source_ip[3] >> SIP_WIDTH_1;
	c_id[SIP_1_LAYER][1] = SIP_EDN_CELL_1;
#endif
#if SIP_2
	c_id[SIP_2_LAYER][0] = p->source_ip[2] >> SIP_WIDTH_2;
	c_id[SIP_2_LAYER][1] = SIP_EDN_CELL_2;
#endif
#if SIP_3
	c_id[SIP_3_LAYER][0] = p->source_ip[1] >> SIP_WIDTH_3;
	c_id[SIP_3_LAYER][1] = SIP_EDN_CELL_3;
#endif
#if SIP_4
	c_id[SIP_4_LAYER][0] = p->source_ip[0] >> SIP_WIDTH_4;
	c_id[SIP_4_LAYER][1] = SIP_EDN_CELL_4;
#endif
#if DIP_1
	c_id[DIP_1_LAYER][0] = p->destination_ip[3] >> DIP_WIDTH_1;
	c_id[DIP_1_LAYER][1] = DIP_EDN_CELL_1;
#endif
#if DIP_2
	c_id[DIP_2_LAYER][0] = p->destination_ip[2] >> DIP_WIDTH_2;
	c_id[DIP_2_LAYER][1] = DIP_EDN_CELL_2;
#endif
#if DIP_3
	c_id[DIP_3_LAYER][0] = p->destination_ip[1] >> DIP_WIDTH_3;
	c_id[DIP_3_LAYER][1] = DIP_EDN_CELL_3;
#endif
#if DIP_4
	c_id[DIP_4_LAYER][0] = p->destination_ip[0] >> DIP_WIDTH_4;
	c_id[DIP_4_LAYER][1] = DIP_EDN_CELL_4;
#endif
#if SPORT
	c_id[SPORT_LAYER][0] = p->source_port >> SPORT_WIDTH;
	c_id[SPORT_LAYER][1] = SPORT_END_CELL;
#endif
#if DPORT
	c_id[DPORT_LAYER][0] = p->destination_port >> DPORT_WIDTH;
	c_id[DPORT_LAYER][1] = DPORT_END_CELL;
#endif

	res = 0x7FFFFFFF;

#if ENABLE_LOG
	int cell_num = -1;
#endif

	for (int i = 0; i < 2; i++)
	{
		int id_1 = c_id[0][i] * LAYER_1;
		for (int j = 0; j < 2; j++)
		{
			int id_2 = (id_1 + c_id[1][j]) * LAYER_2;
			for (int v = 0; v < 2; v++)
			{
				int id_3 = (id_2 + c_id[2][v]) * LAYER_3;
				for (int w = 0; w < 2; w++)
				{
					Cell* q = _c + id_3 + c_id[3][w];
//						int _size = q->size;

#if ENABLE_LOG
					cell_num++;
					log->list[cell_num].id = id_3 + c_id[3][w];
					log->list[cell_num].size = _size;
					log->list[cell_num].layer[0] = c_id[0][i];
					log->list[cell_num].layer[1] = c_id[1][j];
					log->list[cell_num].layer[2] = c_id[2][v];
					log->list[cell_num].layer[3] = c_id[3][w];
#endif

//						if (q->size == 0)continue;
					data* _d = q->list;
					unsigned int _ip;
					if (q->size > 5000)
					{
						for (int u = 0; u < q->size; u++, ++_d)
						{ //check in cell


#if ENABLE_LOG
							log->rules++;
					log->list[cell_num].rules++;
					//__builtin_prefetch(_d + 4, 0);
					log->ele++;
					log->list[cell_num].ele++;
#endif

							if (res < _d->PRI)break; // check priority
							if (bitset_[_d->PRI >> 3] & 1 << (_d->PRI & 7))
							{
//								printf("if Cell%d %d\n",id_3 + c_id[3][w],_d->PRI);
								continue;
							}
//							else{
//								printf("else Cell%d %d\n",id_3 + c_id[3][w],_d->PRI);
//							}
#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (e_protocol != _d->protocol[1] && _d->protocol[0] != 0)continue; // check protocol

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							unsigned int m_bit =
								32 - (unsigned int)_d->destination_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->destination_ip, 4);
							if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
								continue;  // if destination ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							m_bit = 32 - (unsigned int)_d->source_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->source_ip, 4);
							if (m_bit != 32 && es_ip >> m_bit != _ip >> m_bit)
								continue;  // if source ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
								continue;  // if destination port not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
								continue;  // if source port not match, check next

							res = _d->PRI;
#if ENABLE_LOG
							log->list[cell_num].match = res;
#endif
							break;
						}
					}
					else
					{
						for (int u = 0; u < q->size; u++, ++_d)
						{ //check in cell


#if ENABLE_LOG
							log->rules++;
					log->list[cell_num].rules++;
					//__builtin_prefetch(_d + 4, 0);
					log->ele++;
					log->list[cell_num].ele++;
#endif

							if (res < _d->PRI)break; // check priority
//						if (bitset[_d->PRI >> 3] & 1 << (_d->PRI & 7)) continue;
#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (e_protocol != _d->protocol[1] && _d->protocol[0] != 0)continue; // check protocol

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							unsigned int m_bit =
								32 - (unsigned int)_d->destination_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->destination_ip, 4);
							if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
								continue;  // if destination ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							m_bit = 32 - (unsigned int)_d->source_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->source_ip, 4);
							if (m_bit != 32 && es_ip >> m_bit != _ip >> m_bit)
								continue;  // if source ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
								continue;  // if destination port not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
								continue;  // if source port not match, check next

							res = _d->PRI;
#if ENABLE_LOG
							log->list[cell_num].match = res;
#endif
							break;
						} // End for
					} // End else
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;
//		++p;
//	}

//	time_2 = GetCPUCycle();
//
//	*_cycle = time_2 - time_1;
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

void MultiLevelHash::get_cell_size(Cell* c)
{
	int nullN = 0, maxV = 0;
	FILE* fp = NULL;
//	fp = fopen("cell_size.txt", "w");
//	fp = fopen(outputFileName, "w");
	for (int i = 0; i < CELL_SIZE; i++)
	{
//		fprintf(fp, "ID: %d size: %d\n", i, (c + i)->size);
		if ((c[i].size) == 0) nullN++;
		else if (c[i].size > maxV)maxV = c[i].size;
	}
//	fclose(fp);
	printf("maxCell: %d; nullCell: %d; cellNum: %d; nullPercent: %f\n", maxV, nullN, CELL_SIZE,
		(double)nullN / CELL_SIZE);
}

void MultiLevelHash::analyse_log(ACL_rules* data)
{
	int _log[11][258] = { 0 };

	for (int i = 0; i < data->size; i++)
	{
		rule* p = data->list + i;
		unsigned int c_id[11]; //index cell id
		unsigned int s_mask = (unsigned int)(p->source_mask >> 3);
		unsigned int d_mask = (unsigned int)(p->destination_mask >> 3);

		if ((unsigned int)p->protocol[0] == 0)c_id[0] = 4;
		else
		{
			switch ((unsigned int)p->protocol[1])
			{
			case TCP:
				c_id[0] = 0;
				break;
			case ICMP:
				c_id[0] = 1;
				break;
			case UDP:
				c_id[0] = 2;
				break;
			default:
				c_id[0] = 3;
				break;
			}
		}
		if (p->source_mask == 0)
		{
			c_id[1] = 257;
			c_id[2] = 257;
			c_id[3] = 257;
			c_id[4] = 257;
		}
		else
		{
			switch (s_mask)
			{
			case 0:
				c_id[1] = 256;
				c_id[2] = 256;
				c_id[3] = 256;
				c_id[4] = 256;
				break;
			case 1:
				c_id[1] = p->source_ip[3];
				c_id[2] = 256;
				c_id[3] = 256;
				c_id[4] = 256;
				break;
			case 2:
				c_id[1] = p->source_ip[3];
				c_id[2] = p->source_ip[2];
				c_id[3] = 256;
				c_id[4] = 256;
				break;
			case 3:
				c_id[1] = p->source_ip[3];
				c_id[2] = p->source_ip[2];
				c_id[3] = p->source_ip[1];
				c_id[4] = 256;
				break;
			default:
				c_id[1] = p->source_ip[3];
				c_id[2] = p->source_ip[2];
				c_id[3] = p->source_ip[1];
				c_id[4] = p->source_ip[0];
				break;
			}
		}
		if (p->destination_mask == 0)
		{
			c_id[5] = 257;
			c_id[6] = 257;
			c_id[7] = 257;
			c_id[8] = 257;
		}
		else
		{
			switch (d_mask)
			{
			case 0:
				c_id[5] = 256;
				c_id[6] = 256;
				c_id[7] = 256;
				c_id[8] = 256;
				break;
			case 1:
				c_id[5] = p->destination_ip[3];
				c_id[6] = 256;
				c_id[7] = 256;
				c_id[8] = 256;
				break;
			case 2:
				c_id[5] = p->destination_ip[3];
				c_id[6] = p->destination_ip[2];
				c_id[7] = 256;
				c_id[8] = 256;
				break;
			case 3:
				c_id[5] = p->destination_ip[3];
				c_id[6] = p->destination_ip[2];
				c_id[7] = p->destination_ip[1];
				c_id[8] = 256;
				break;
			default:
				c_id[5] = p->destination_ip[3];
				c_id[6] = p->destination_ip[2];
				c_id[7] = p->destination_ip[1];
				c_id[8] = p->destination_ip[0];
				break;
			}
		}
		if (p->source_port[0] == 0 && p->source_port[1] == (unsigned int)65535) c_id[9] = 257;
		else if (p->source_port[0] == p->source_port[1] || p->source_port[0] >> 8 == p->source_port[1] >> 8)
			c_id[9] = p->source_port[0] >> 8;
		else c_id[9] = 256;
		if (p->destination_port[0] == 0 && p->destination_port[1] == (unsigned int)65535) c_id[10] = 257;
		else if (p->destination_port[0] == p->destination_port[1] ||
				 p->destination_port[0] >> 8 == p->destination_port[1] >> 8)
			c_id[10] = p->source_port[0] >> 8;
		else c_id[10] = 256;

		for (int j = 0; j < 11; j++)
		{
			_log[j][c_id[j]]++;
		}
	}

	FILE* fp = NULL;
	fp = fopen("analyse_data.txt", "w");
	fprintf(fp, "0 ");
	for (int j = 0; j < 5; j++)
		fprintf(fp, "%d ", _log[0][j]);
	fprintf(fp, "\n");
	for (int i = 1; i < 11; i++)
	{
		fprintf(fp, "%d ", i);
		for (int j = 0; j < 258; j++)
			fprintf(fp, "%d ", _log[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

// Rule Distribution
void MultiLevelHash::analyse_log2(ACL_rules* data, char outputFileName[])
{
	int* protocal = (int*)calloc(17, sizeof(int));

	int* storage = (int*)calloc(4 * 257, sizeof(int));
	int** srcIP = (int**)malloc(4 * sizeof(void*));
	for (int i = 0; i < 4; i++)
	{
		srcIP[i] = storage + i * 257 * sizeof(int); // 服务器环境
	}
	storage = (int*)calloc(4 * 257, sizeof(int));
	int** dstIP = (int**)malloc(4 * sizeof(void*));
	for (int i = 0; i < 4; i++)
	{
		dstIP[i] = storage + i * 257 * sizeof(int); // 服务器环境
	}
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 257; j++)
			if (srcIP[i][j] != 0 || dstIP[i][j] != 0)
				printf("%d %d\n", srcIP[i][j], dstIP[i][j]);

	int srcMaskType[33];
	memset(srcMaskType, 0, sizeof(srcMaskType));
	int dstMaskType[33];
	memset(dstMaskType, 0, sizeof(dstMaskType));

	const int portCellNum = 257;
	int portShift = 16 - __builtin_ctz(portCellNum - 1); // 区间长度是2的多少次方
	//int portShift = 16 - _tzcnt_u64(portCellNum - 1); // 区间长度是2的多少次方  // Windows
	int srcPort[portCellNum];
	int dstPort[portCellNum];
	memset(srcPort, 0, sizeof(srcPort));
	memset(dstPort, 0, sizeof(dstPort));

	int sPWidth[portCellNum];
	int dPWidth[portCellNum];
	memset(sPWidth, 0, sizeof(sPWidth));
	memset(dPWidth, 0, sizeof(dPWidth));

	for (int i = 0; i < data->size; i++)
	{
		rule* p = data->list + i;

		if ((unsigned int)p->protocol[0] == 0) protocal[0]++;
		else
		{
			int ptl;
			switch ((unsigned int)p->protocol[1])
			{
			case ICMP:
				ptl = 1;
				break;
			case TCP:
				ptl = 2;
				break;
			case UDP:
				ptl = 3;
				break;
			case RSVP:
				ptl = 4;
				break;
			case ISIS:
				ptl = 5;
				break;
			default:
				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
				return;
			}
//			switch ((unsigned int) p->protocol[1]) {
//				case ICMP:
//					ptl = 1;
//					break;
//				case IGMP:
//					ptl = 2;
//					break;
//				case GGP:
//					ptl = 3;
//					break;
//				case IP:
//					ptl = 4;
//					break;
//				case ST:
//					ptl = 5;
//					break;
//				case TCP:
//					ptl = 6;
//					break;
//				case CBT:
//					ptl = 7;
//					break;
//				case EGP:
//					ptl = 8;
//					break;
//				case UDP:
//					ptl = 9;
//					break;
//				case RSVP:
//					ptl = 10;
//					break;
//				case GRE:
//					ptl = 11;
//					break;
//				case ESP:
//					ptl = 12;
//					break;
//				case AH:
//					ptl = 13;
//					break;
//				case EIGRP:
//					ptl = 14;
//					break;
//				case OSPFIGP:
//					ptl = 15;
//					break;
//				case ISIS:
//					ptl = 1;
//					break;
//				default:
//					fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
//					return;
//			}
			protocal[ptl]++;
		}

		srcMaskType[(unsigned int)(p->source_mask)]++;
		switch ((unsigned int)(p->source_mask >> 3))
		{
		case 0:
			srcIP[0][256]++;
			break;
		case 1:
			srcIP[0][(unsigned int)p->source_ip[3]]++;
			srcIP[1][256]++;
			break;
		case 2:
			srcIP[0][(unsigned int)p->source_ip[3]]++;
			srcIP[1][(unsigned int)p->source_ip[2]]++;
			srcIP[2][256]++;
			break;
		case 3:
			srcIP[0][(unsigned int)p->source_ip[3]]++;
			srcIP[1][(unsigned int)p->source_ip[2]]++;
			srcIP[2][(unsigned int)p->source_ip[1]]++;
			srcIP[3][256]++;
			break;
		case 4:
			srcIP[0][(unsigned int)p->source_ip[3]]++;
			srcIP[1][(unsigned int)p->source_ip[2]]++;
			srcIP[2][(unsigned int)p->source_ip[1]]++;
			srcIP[3][(unsigned int)p->source_ip[0]]++;
			break;
		default:
			printf("Error src_mask: %d\n", p->source_mask);
		}

		dstMaskType[(unsigned int)p->destination_mask]++;
		switch ((unsigned int)(p->destination_mask >> 3))
		{
		case 0:
			dstIP[0][256]++;
			break;
		case 1:
			dstIP[0][(unsigned int)p->destination_ip[3]]++;
			dstIP[1][256]++;
			break;
		case 2:
			dstIP[0][(unsigned int)p->destination_ip[3]]++;
			dstIP[1][(unsigned int)p->destination_ip[2]]++;
			dstIP[2][256]++;
			break;
		case 3:
			dstIP[0][(unsigned int)p->destination_ip[3]]++;
			dstIP[1][(unsigned int)p->destination_ip[2]]++;
			dstIP[2][(unsigned int)p->destination_ip[1]]++;
			dstIP[3][256]++;
			break;
		case 4:
			dstIP[0][(unsigned int)p->destination_ip[3]]++;
			dstIP[1][(unsigned int)p->destination_ip[2]]++;
			dstIP[2][(unsigned int)p->destination_ip[1]]++;
			dstIP[3][(unsigned int)p->destination_ip[0]]++;
			break;
		default:
			printf("Error dst_mask: %d\n", p->destination_mask >> 3);
		}

		int portCell = p->source_port[0] >> portShift;
		if (portCell == p->source_port[1] >> portShift)
			srcPort[portCell]++;
		else srcPort[portCellNum - 1]++;

		portCell = p->destination_port[0] >> portShift;
		if (portCell == p->destination_port[1] >> portShift)
			dstPort[portCell]++;
		else dstPort[portCellNum - 1]++;

		sPWidth[((int)p->source_port[1] - (int)p->source_port[0]) >> portShift]++;
		dPWidth[((int)p->destination_port[1] - (int)p->destination_port[0]) >> portShift]++;
	}

	long long int protocolAvg = 0, protocalVar = 0, protocolNum = 0;
	for (int i = 0; i < 17; i++)
		if (protocal[i] > 0)
		{
			protocolAvg += protocal[i];
			protocolNum++;
		}
	protocolNum = 6; // 按15个桶算方差，方差小一些
	protocolAvg /= protocolNum;
	for (int i = 0; i < 15; i++)
		if (protocal[i] > 0)
			protocalVar += pow(protocal[i] - protocolAvg, 2);
	protocalVar = sqrt(protocalVar / protocolNum);

	long long int IPvar[8], IPavg[8];
	memset(IPvar, 0, sizeof(IPvar));
	memset(IPavg, 0, sizeof(IPavg));
	for (int i = 0; i < 257; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			IPavg[j] += srcIP[j][i];
			IPavg[j + 4] += dstIP[j][i];
		}
	}
	for (int i = 0; i < 8; i++)
		IPavg[i] /= 257;
	for (int i = 0; i < 257; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			IPvar[j] += pow(srcIP[j][i] - IPavg[j], 2);
			IPvar[j + 4] += pow(dstIP[j][i] - IPavg[j + 4], 2);
		}
	}
	for (int i = 0; i < 8; i++)
		IPvar[i] = (int)sqrt((double)IPvar[i] / 257);

	long long int portVar[2], portAvg[2];
	memset(portVar, 0, sizeof(portVar));
	memset(portAvg, 0, sizeof(portAvg));
	for (int i = 0; i < portCellNum; i++)
	{
		portAvg[0] += srcPort[i];
		portAvg[1] += dstPort[i];
	}
	portAvg[0] /= portCellNum;
	portAvg[1] /= portCellNum;
	for (int i = 0; i < portCellNum; i++)
	{
		portVar[0] += pow(srcPort[i] - portAvg[0], 2);
		portVar[1] += pow(dstPort[i] - portAvg[1], 2);
	}
	for (int i = 0; i < 2; i++)
		portVar[i] = (int)sqrt((double)portVar[i] / portCellNum);

	long long int portWidthVar[2], portWidthAvg[2];
	memset(portWidthVar, 0, sizeof(portWidthVar));
	memset(portWidthAvg, 0, sizeof(portWidthAvg));
	for (int i = 0; i < portCellNum; i++)
	{
		portWidthAvg[0] += sPWidth[i];
		portWidthAvg[1] += dPWidth[i];
	}
	portWidthAvg[0] /= portCellNum;
	portWidthAvg[1] /= portCellNum;
	for (int i = 0; i < portCellNum; i++)
	{
		portWidthVar[0] += pow(sPWidth[i] - portWidthAvg[0], 2);
		portWidthVar[1] += pow(dPWidth[i] - portWidthAvg[1], 2);
	}
	for (int i = 0; i < 2; i++)
		portWidthVar[i] = (int)sqrt((double)portWidthVar[i] / portCellNum);

	FILE* fp = NULL;
	fp = fopen(outputFileName, "w");
	fprintf(fp, "Total Rules: %d\n\n", data->size);
	for (int i = 0; i < 15; i++)
	{
		if (i > 0 && i % 5 == 0)fprintf(fp, "\n");
		fprintf(fp, "P%d=%-6d\t", i, protocal[i]);
	}
	fprintf(fp, "\nprotocalVar= %d\n\n", protocalVar);

	fprintf(fp, "srcMask=0: %d\n", srcMaskType[0]);
	for (int i = 1; i < 33; i++)
	{
		fprintf(fp, "%2d: %4d\t", i, srcMaskType[i]);
		if (i % 8 == 0) fprintf(fp, "\n");
	}
	fprintf(fp, "\ndstMask=0: %d\n", dstMaskType[0]);
	for (int i = 1; i < 33; i++)
	{
		fprintf(fp, "%2d: %4d\t", i, dstMaskType[i]);
		if (i % 8 == 0) fprintf(fp, "\n");
	}
//	for(int i=0;i<33;i++){
//		totalSrcMaskType[i]+=srcMaskType[i];
//		totalDstMaskType[i]+=dstMaskType[i];
//	}

	fprintf(fp, "\nNo.\tsrc1\tsrc2\tsrc3\tsrc4\tdst1\tdst2\tdst3\tdst4\tsrcPort\tdstPort\tsPWidth\tdPWidth\n");
	for (int i = 0; i < MAX(257, portCellNum); i++)
	{
		if (i < MIN(257, portCellNum))
			fprintf(fp, "%d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\n", i, srcIP[0][i], srcIP[1][i],
				srcIP[2][i], srcIP[3][i], dstIP[0][i], dstIP[1][i], dstIP[2][i], dstIP[3][i], srcPort[i],
				dstPort[i], sPWidth[i], dPWidth[i]);
		else if (i < 257)
			fprintf(fp, "%d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\n", i, srcIP[0][i], srcIP[1][i], srcIP[2][i],
				srcIP[3][i], dstIP[0][i], dstIP[1][i], dstIP[2][i], dstIP[3][i]);
		else
			fprintf(fp, "%d\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t%4d\t%4d\t%4d\t%4d\n", i, srcPort[i], dstPort[i],
				sPWidth[i], dPWidth[i]);
	}
	fprintf(fp, "var\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\n", IPvar[0], IPvar[1],
		IPvar[2],
		IPvar[3], IPvar[4],
		IPvar[5], IPvar[6], IPvar[7], portVar[0], portVar[1], portWidthVar[0], portWidthVar[1]);

	// 不算通配求方差
	memset(IPvar, 0, sizeof(IPvar));
	memset(IPavg, 0, sizeof(IPavg));
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			IPavg[j] += srcIP[j][i];
			IPavg[j + 4] += dstIP[j][i];
		}
	}
	for (int i = 0; i < 8; i++)
		IPavg[i] /= 256;
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			IPvar[j] += pow(srcIP[j][i] - IPavg[j], 2);
			IPvar[j + 4] += pow(dstIP[j][i] - IPavg[j + 4], 2);
		}
	}
	for (int i = 0; i < 8; i++)
		IPvar[i] = (int)sqrt((double)IPvar[i] / 256);

	memset(portVar, 0, sizeof(portVar));
	memset(portAvg, 0, sizeof(portAvg));
	for (int i = 0; i < portCellNum - 1; i++)
	{
		portAvg[0] += srcPort[i];
		portAvg[1] += dstPort[i];
	}
	portAvg[0] /= (portCellNum - 1);
	portAvg[1] /= (portCellNum - 1);
	for (int i = 0; i < portCellNum - 1; i++)
	{
		portVar[0] += pow(srcPort[i] - portAvg[0], 2);
		portVar[1] += pow(dstPort[i] - portAvg[1], 2);
	}
	for (int i = 0; i < 2; i++)
		portVar[i] = (int)sqrt((double)portVar[i] / (portCellNum - 1));

	fprintf(fp, "V2\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\n", IPvar[0], IPvar[1], IPvar[2],
		IPvar[3], IPvar[4],
		IPvar[5], IPvar[6], IPvar[7], portVar[0], portVar[1]);
	fclose(fp);
	free(srcIP[0]), free(srcIP), free(dstIP[0]), free(dstIP), free(protocal);
}

void MultiLevelHash::printProtocolType(ACL_rules* data, char outputFileName[])
{
	int* protocal = (int*)calloc(257, sizeof(int));
	for (int i = 0; i < data->size; i++)
	{
		rule* p = data->list + i;
		if ((unsigned int)p->protocol[0] == 0)
			protocal[0]++;
		else protocal[(unsigned int)p->protocol[1]]++;
	}
	int count = 0;
	FILE* fp = fopen(outputFileName, "w");
	fprintf(fp, "%s: ", outputFileName);
	for (int i = 0; i < 257; i++)
	{
		if (protocal[i] > 0)
		{
			fprintf(fp, "%d:%d, ", i, protocal[i]);
			count++;
		}
	}
	fprintf(fp, " count=%d\n", count);
	printf("%s: %d protocols.\n", outputFileName, count);
	fclose(fp);
	free(protocal);
}

double MultiLevelHash::get_memory(Cell* c_list)
{
	size_t mem = CELL_SIZE * sizeof(Cell);
	for (int i = 0; i < CELL_SIZE; i++)
	{
		mem = mem + (c_list + i)->capacity * sizeof(data);
	}
//	printf("%lu B\n", mem);
	double res = (double)mem / 1048576.0;
	return res;
}

void MultiLevelHash::visualize(int c[], int size)
{
	long long int b1k = 0, b10k = 0, maxCycle = 0, maxEID, cycleSum = 0;
	long long int cn1 = 0, cn2 = 0, maxCheckNum = 0, maxCNEID, checkNumSum = 0;
	int eID_10k[10000];
	int eID_cn2[10000];
	for (int i = 0; i < size; i++)
	{
		if (c[i] >= 1000) b1k++;
		if (c[i] >= 10000)
		{
			eID_10k[b10k++] = i;
		}
		if (maxCycle < c[i])
		{
			maxCycle = c[i];
			maxEID = i;
		}
		cycleSum += c[i];

//		if (checkNum[i] >= 500) cn1++;
//		if (checkNum[i] >= 506) {
//			eID_cn2[cn2++] = i;
//		}
//		if (maxCheckNum < checkNum[i]) {
//			maxCheckNum = checkNum[i];
//			maxCNEID = i;
//		}
//		checkNumSum += checkNum[i];
	}
	printf("maxCycle= %d, eID= %d\n", maxCycle, maxEID);
	printf("avgCycle= %f\n", (double)cycleSum / size);
	printf(">=1k: %d(%f\%)\n", b1k, (double)b1k / size);
	printf(">=10k: %d(%f\%)\n", b10k, (double)b10k / size);
//	printf(">=10k eId:");
//	for(int i=0;i<b10k;i++){
//		printf(" %d",eID_10k[i]);
//	}
//	printf("\n\n");
//	printf("maxCheckNum= %d, eID= %d\n",maxCheckNum,maxCNEID);
//	printf("avgCheckNum= %f\n", (double)checkNumSum / size);
//	printf(">=500: %d(%f\%)\n", cn1, (double)cn1 / size);
//	printf(">=506: %d(%f\%)\n", cn2, (double)cn2 / size);
//	printf(">=506 eId:");
//	for(int i=0;i<cn2;i++){
//		printf(" %d",eID_cn2[i]);
//	}
	printf("\n\n\n");
}

