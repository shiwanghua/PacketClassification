#include"core.h"

void insert(Cell *c_list, rule *r) {
	rule *p = r;
	unsigned int c_id[LEVEL]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));

#if SIP_1 || SIP_2 || SIP_3 || SIP_4
	unsigned int s_mask = (unsigned int) (p->source_mask >> 3);
#endif
#if DIP_1 || DIP_2 || DIP_3 || DIP_4
	unsigned int d_mask = (unsigned int) (p->destination_mask >> 3);
#endif // DIP_1 || DIP_2 || DIP_3 || DIP_4

#if PROTO
	if ((unsigned int)p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else {
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
	if (p->source_port[0] >> SPORT_WIDTH == p->source_port[1] >> SPORT_WIDTH)c_id[SPORT_LAYER] = p->source_port[0] >> SPORT_WIDTH;
	else c_id[SPORT_LAYER] = SPORT_END_CELL;
#endif
#if DPORT
	if (p->destination_port[0] >> DPORT_WIDTH == p->destination_port[1] >> DPORT_WIDTH)c_id[DPORT_LAYER] = p->destination_port[0] >> DPORT_WIDTH;
	else c_id[DPORT_LAYER] = DPORT_END_CELL;
#endif

	int id = ((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3];
	//printf("%d %d\n", p->PRI, id);
	//for(int i=0;i<LEVEL;i++)printf("%d ", c_id[i]);
	//printf("\n");

//	if (d_mask == 32) {
//		int bitsetNo = (int) p->destination_ip[0];
//		int unitNo = p->PRI / 8;
//		int offset = p->PRI % 8;
//		for (int i = 0; i < 256; i++) {
//			if (i != bitsetNo)
//				bitset[i][unitNo] = bitset[i][unitNo] | 1 << offset;
//		}
//	}
	add_data_ordered(c_list + (((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3]), &_d);
}

void insert_cache(Cell *c_list, rule *p,unsigned char** bitset){
	unsigned int c_id[LEVEL]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));

#if SIP_1 || SIP_2 || SIP_3 || SIP_4
	unsigned int s_mask = (unsigned int) (p->source_mask >> 3);
#endif
#if DIP_1 || DIP_2 || DIP_3 || DIP_4
	unsigned int d_mask = (unsigned int) (p->destination_mask >> 3);
#endif // DIP_1 || DIP_2 || DIP_3 || DIP_4

#if PROTO
	if ((unsigned int)p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else {
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
	if (p->source_port[0] >> SPORT_WIDTH == p->source_port[1] >> SPORT_WIDTH)c_id[SPORT_LAYER] = p->source_port[0] >> SPORT_WIDTH;
	else c_id[SPORT_LAYER] = SPORT_END_CELL;
#endif
#if DPORT
	if (p->destination_port[0] >> DPORT_WIDTH == p->destination_port[1] >> DPORT_WIDTH)c_id[DPORT_LAYER] = p->destination_port[0] >> DPORT_WIDTH;
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

//	if(p->destination_mask>7){
//		int bitsetNo = (int) p->destination_ip[3];
//		int unitNo=p->PRI/8;
//		int offset=p->PRI&7;
//		for (int i = 0; i < 256; i++) {
//			if (i != bitsetNo)
//				bitset[i][unitNo] = bitset[i][unitNo]|1<<offset;
//		}
////		printf("%d %d %d\n",bitsetNo,unitNo,offset);
//	}

	if ((unsigned int) p->protocol[0] != 0){
		int bitsetNo=-1;
		int unitNo=p->PRI/8;
		int offset=p->PRI&7;
		switch ((unsigned int) p->protocol[1]) {
			case ICMP:
				bitsetNo=0;
				break;
			case IGMP:
				bitsetNo = 1;
				break;
			case GGP:
				bitsetNo = 2;
				break;
			case IP:
				bitsetNo = 3;
				break;
			case ST:
				bitsetNo = 4;
				break;
			case TCP:
				bitsetNo = 5;
				break;
			case CBT:
				bitsetNo = 6;
				break;
			case EGP:
				bitsetNo = 7;
				break;
			case UDP:
				bitsetNo = 8;
				break;
			case GRE:
				bitsetNo = 9;
				break;
			case ESP:
				bitsetNo = 10;
				break;
			case AH:
				bitsetNo = 11;
				break;
			case EIGRP:
				bitsetNo = 12;
				break;
			case OSPFIGP:
				bitsetNo = 13;
				break;
			default:
				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
				return;
		}
		for (int i = 0; i < 14; i++) {
			if(i!=bitsetNo)
				bitset[i][unitNo] = bitset[i][unitNo] | 1 << offset;
		}
	}
	add_data_ordered(c_list + (((c_id[0] * LAYER_1 + c_id[1]) * LAYER_2 + c_id[2]) * LAYER_3 + c_id[3]), &_d);
}

int match(Cell *c_list, message *m) {
	return -1;
}

int match_with_log(Cell *c_list, message *m, int *_cycle, MatchLog *log) {
//	uint64_t time_1, time_2;
//
//	time_1 = GetCPUCycle();
	Cell *_c = c_list;
	message *p = m;
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

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[0][i] * LAYER_1;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[1][j]) * LAYER_2;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[2][v]) * LAYER_3;
				for (int w = 0; w < 2; w++) {
					Cell *q = _c + id_3 + c_id[3][w];
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
					data *_d = q->list;

//					for (int u = 0; u < q->size; u++, ++_d){
//						*_cycle++;
//					}
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell


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
							32 - (unsigned int) _d->destination_mask;  // comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
							continue;  // if destination ip not match, check next

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele++;
#endif

						m_bit = 32 - (unsigned int) _d->source_mask;  // comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (m_bit != 32 && es_ip >> m_bit != _ip >> m_bit)
							continue;  // if source ip not match, check next

#if ENABLE_LOG
						log->ele++;
					log->list[cell_num].ele++;
#endif

						if (e_protocol != _d->protocol[1] && _d->protocol[0] != 0||ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port||es_port < _d->source_port[0] || _d->source_port[1] < es_port)
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

int match_with_log_cache(Cell* c_list, message* m,unsigned char** bitset,int* _cycle, MatchLog* log){
	//	uint64_t time_1, time_2;
//
//	time_1 = GetCPUCycle();
	Cell *_c = c_list;
	message *p = m;
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
//	int bitsetNo = (int) p->destination_ip[0];
	int bitsetNo;//=(int) p->destination_ip[3];
	switch (e_protocol) {
		case ICMP:
			bitsetNo = 0;
			break;
		case IGMP:
			bitsetNo = 1;
			break;
		case GGP:
			bitsetNo = 2;
			break;
		case IP:
			bitsetNo = 3;
			break;
		case ST:
			bitsetNo = 4;
			break;
		case TCP:
			bitsetNo = 5;
			break;
		case CBT:
			bitsetNo = 6;
			break;
		case EGP:
			bitsetNo = 7;
			break;
		case UDP:
			bitsetNo = 8;
			break;
		case GRE:
			bitsetNo = 9;
			break;
		case ESP:
			bitsetNo = 10;
			break;
		case AH:
			bitsetNo = 11;
			break;
		case EIGRP:
			bitsetNo = 12;
			break;
		case OSPFIGP:
			bitsetNo = 13;
			break;
	}
	unsigned char* bitset_=bitset[bitsetNo];

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


	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[0][i] * LAYER_1;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[1][j]) * LAYER_2;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[2][v]) * LAYER_3;
				for (int w = 0; w < 2; w++) {
					Cell *q = _c + id_3 + c_id[3][w];
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
					data *_d = q->list;
					unsigned int _ip;
					if(q->size>3000){
						for (int u = 0; u < q->size; u++, ++_d) { //check in cell


#if ENABLE_LOG
							log->rules++;
					log->list[cell_num].rules++;
					//__builtin_prefetch(_d + 4, 0);
					log->ele++;
					log->list[cell_num].ele++;
#endif

							if (res < _d->PRI)break; // check priority
							if(bitset_[_d->PRI>>3]&1<<(_d->PRI&7)) {
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
								32 - (unsigned int) _d->destination_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->destination_ip, 4);
							if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
								continue;  // if destination ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							m_bit = 32 - (unsigned int) _d->source_mask;  // comput the bit number need to move
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
					} else{
						for (int u = 0; u < q->size; u++, ++_d) { //check in cell


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
								32 - (unsigned int) _d->destination_mask;  // comput the bit number need to move
							memcpy(&_ip, _d->destination_ip, 4);
							if (m_bit != 32 && ed_ip >> m_bit != _ip >> m_bit)
								continue;  // if destination ip not match, check next

#if ENABLE_LOG
							log->ele++;
					log->list[cell_num].ele++;
#endif

							m_bit = 32 - (unsigned int) _d->source_mask;  // comput the bit number need to move
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

void get_cell_size(Cell *c) {
	int nullN = 0, maxV = 0;
	FILE *fp = NULL;
//	fp = fopen("cell_size.txt", "w");
//	fp = fopen(outputFileName, "w");
	for (int i = 0; i < CELL_SIZE; i++) {
//		fprintf(fp, "ID: %d size: %d\n", i, (c + i)->size);
		if ((c[i].size) == 0) nullN++;
		else if (c[i].size > maxV)maxV = c[i].size;
	}
//	fclose(fp);
	printf("maxCell: %d; nullCell: %d; cellNum: %d; nullPercent: %f\n", maxV, nullN, CELL_SIZE,
		   (double) nullN / CELL_SIZE);
}

/***************************************************/
/*
*   analyse dateset:
*   ��5Ԫ����з���������ip���ְ���ÿ���ֽڽ��з���
*   Э����Ϊ5�����֣�TCP��ICMP��UDP������Э�顢ͨ���
*   IP�㰴�ֽڽ��з�������Ϊ258�����֣�����0-255��Ӧ׼ȷ��ip��256��Ӧ��Χip��257��Ӧͨ��ip������Ϊ0
*   �˿ںŷ�Ϊ258�����֣�����0-255��Ӧ�������Ķ˿����䣬256��Ӧ�޷����������䣬257��Ӧͨ�����伴[0-65535]
*/
/***************************************************/
void analyse_log(ACL_rules *data) {
	int _log[11][258] = {0};

	for (int i = 0; i < data->size; i++) {
		rule *p = data->list + i;
		unsigned int c_id[11]; //index cell id
		unsigned int s_mask = (unsigned int) (p->source_mask >> 3);
		unsigned int d_mask = (unsigned int) (p->destination_mask >> 3);

		if ((unsigned int) p->protocol[0] == 0)c_id[0] = 4;
		else {
			switch ((unsigned int) p->protocol[1]) {
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
		if (p->source_mask == 0) {
			c_id[1] = 257;
			c_id[2] = 257;
			c_id[3] = 257;
			c_id[4] = 257;
		} else {
			switch (s_mask) {
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
		if (p->destination_mask == 0) {
			c_id[5] = 257;
			c_id[6] = 257;
			c_id[7] = 257;
			c_id[8] = 257;
		} else {
			switch (d_mask) {
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
		if (p->source_port[0] == 0 && p->source_port[1] == (unsigned int) 65535) c_id[9] = 257;
		else if (p->source_port[0] == p->source_port[1] || p->source_port[0] >> 8 == p->source_port[1] >> 8)
			c_id[9] = p->source_port[0] >> 8;
		else c_id[9] = 256;
		if (p->destination_port[0] == 0 && p->destination_port[1] == (unsigned int) 65535) c_id[10] = 257;
		else if (p->destination_port[0] == p->destination_port[1] ||
				 p->destination_port[0] >> 8 == p->destination_port[1] >> 8)
			c_id[10] = p->source_port[0] >> 8;
		else c_id[10] = 256;

		for (int j = 0; j < 11; j++) {
			_log[j][c_id[j]]++;
		}
	}

	FILE *fp = NULL;
	fp = fopen("analyse_data.txt", "w");
	fprintf(fp, "0 ");
	for (int j = 0; j < 5; j++)
		fprintf(fp, "%d ", _log[0][j]);
	fprintf(fp, "\n");
	for (int i = 1; i < 11; i++) {
		fprintf(fp, "%d ", i);
		for (int j = 0; j < 258; j++)
			fprintf(fp, "%d ", _log[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

double get_memory(Cell *c_list) {
	size_t mem = CELL_SIZE * sizeof(Cell);
	for (int i = 0; i < CELL_SIZE; i++) {
		mem = mem + (c_list + i)->capacity * sizeof(data);
	}
//	printf("%lu B\n", mem);
	double res = (double) mem / 1048576.0;
	return res;
}

void visualize(int c[], int size) {
	long long int b1k = 0, b10k = 0, maxCycle = 0, maxEID, cycleSum = 0;
	long long int cn1 = 0, cn2 = 0, maxCheckNum = 0, maxCNEID, checkNumSum = 0;
	int eID_10k[10000];
	int eID_cn2[10000];
	for (int i = 0; i < size; i++) {
		if (c[i] >= 1000) b1k++;
		if (c[i] >= 10000) {
			eID_10k[b10k++] = i;
		}
		if (maxCycle < c[i]) {
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
	printf("avgCycle= %f\n", (double) cycleSum / size);
	printf(">=1k: %d(%f\%)\n", b1k, (double) b1k / size);
	printf(">=10k: %d(%f\%)\n", b10k, (double) b10k / size);
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

