#include"core.h"

// solution1: sourceIP_1 && sourceIP_2 && destinationPort && protocal
void insert_solution1(Cell *c_list, rule *p) {
	unsigned int c_id[LEVEL_solution1]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));
	_d.source_mask = (unsigned short) p->source_mask;
	_d.destination_mask = (unsigned short) p->destination_mask;

	if ((unsigned int) p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else {
		switch ((unsigned int) p->protocol[1]) {
			case ICMP:
				c_id[PROTO_LAYER] = 0;
				break;
			case IGMP:
				c_id[PROTO_LAYER] = 1;
				break;
			case GGP:
				c_id[PROTO_LAYER] = 2;
				break;
			case IP:
				c_id[PROTO_LAYER] = 3;
				break;
			case ST:
				c_id[PROTO_LAYER] = 4;
				break;
			case TCP:
				c_id[PROTO_LAYER] = 5;
				break;
			case CBT:
				c_id[PROTO_LAYER] = 6;
				break;
			case EGP:
				c_id[PROTO_LAYER] = 7;
				break;
			case UDP:
				c_id[PROTO_LAYER] = 8;
				break;
			case GRE:
				c_id[PROTO_LAYER] = 9;
				break;
			case ESP:
				c_id[PROTO_LAYER] = 10;
				break;
			case AH:
				c_id[PROTO_LAYER] = 11;
				break;
			case EIGRP:
				c_id[PROTO_LAYER] = 12;
				break;
			case OSPFIGP:
				c_id[PROTO_LAYER] = 13;
				break;
			default:
				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
				c_id[PROTO_LAYER] = PROTO_END_CELL;
				return;
		}
	}
	unsigned int s_mask = (unsigned int) (p->source_mask >> 3);
	switch (s_mask) {
		case 0: // 0-7
			c_id[srcIP_LAYER_1] = srcIP_END_CELL_1;
			c_id[srcIP_LAYER_2] = srcIP_END_CELL_2;
			break;
		case 1: // 8-15
			c_id[srcIP_LAYER_1] = p->source_ip[3] >> srcIP_WIDTH_1;
			c_id[srcIP_LAYER_2] = srcIP_END_CELL_2;
			break;
		default:
			c_id[srcIP_LAYER_1] = p->source_ip[3] >> srcIP_WIDTH_1;
			c_id[srcIP_LAYER_2] = p->source_ip[2] >> srcIP_WIDTH_2;
			break;
	}
	if (p->destination_port[0] == p->destination_port[1])c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
	else if (p->destination_port[0] >> PORT_WIDTH == p->destination_port[1] >> PORT_WIDTH)
		c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
	else c_id[PORT_LAYER] = PORT_END_CELL;

//	int id = ((c_id[0] * srcIP_SIZE_2 + c_id[1]) * PORT_SIZE + c_id[2]) * PROTO_SIZE + c_id[3];
//	printf("%d %d\n", p->PRI, id);
//	for(int i=0;i<LEVEL_solution1;i++)printf("%d ", c_id[i]);
//	printf("\n");

	add_data_ordered(c_list + ((c_id[0] * srcIP_SIZE_2 + c_id[1]) * PORT_SIZE + c_id[2]) * PROTO_SIZE + c_id[3], &_d);
}

// solution2: destinationIP_2 && destinationIP_3 && destinationPort && protocal
void insert_solution2(Cell *c_list, rule *p) {
	unsigned int c_id[LEVEL_solution2]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));
	_d.source_mask = (unsigned short) p->source_mask;
	_d.destination_mask = (unsigned short) p->destination_mask;

	if ((unsigned int) p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
	else {
		switch ((unsigned int) p->protocol[1]) {
			case ICMP:
				c_id[PROTO_LAYER] = 0;
				break;
			case IGMP:
				c_id[PROTO_LAYER] = 1;
				break;
			case GGP:
				c_id[PROTO_LAYER] = 2;
				break;
			case IP:
				c_id[PROTO_LAYER] = 3;
				break;
			case ST:
				c_id[PROTO_LAYER] = 4;
				break;
			case TCP:
				c_id[PROTO_LAYER] = 5;
				break;
			case CBT:
				c_id[PROTO_LAYER] = 6;
				break;
			case EGP:
				c_id[PROTO_LAYER] = 7;
				break;
			case UDP:
				c_id[PROTO_LAYER] = 8;
				break;
			case GRE:
				c_id[PROTO_LAYER] = 9;
				break;
			case ESP:
				c_id[PROTO_LAYER] = 10;
				break;
			case AH:
				c_id[PROTO_LAYER] = 11;
				break;
			case EIGRP:
				c_id[PROTO_LAYER] = 12;
				break;
			case OSPFIGP:
				c_id[PROTO_LAYER] = 13;
				break;
			default:
				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
				c_id[PROTO_LAYER] = PROTO_END_CELL;
				return;
		}
	}
	unsigned int s_mask = (unsigned int) (p->destination_mask >> 3);
	switch (s_mask) {
		case 0: // 0-7
		case 1: // 8-15
			c_id[dstIP_LAYER_1] = dstIP_END_CELL_1;
			c_id[dstIP_LAYER_2] = dstIP_END_CELL_2;
			break;
		case 2:
			c_id[dstIP_LAYER_1] = p->destination_ip[2] >> dstIP_WIDTH_1;
			c_id[dstIP_LAYER_2] = dstIP_END_CELL_2;
			break;
		default:
			c_id[dstIP_LAYER_1] = p->destination_ip[2] >> dstIP_WIDTH_1;
			c_id[dstIP_LAYER_2] = p->destination_ip[1] >> dstIP_WIDTH_2;
			break;
	}
	if (p->destination_port[0] == p->destination_port[1])c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
	else if (p->destination_port[0] >> PORT_WIDTH == p->destination_port[1] >> PORT_WIDTH)
		c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
	else c_id[PORT_LAYER] = PORT_END_CELL;

//	int id = ((c_id[0] * srcIP_SIZE_2 + c_id[1]) * PORT_SIZE + c_id[2]) * PROTO_SIZE + c_id[3];
//	printf("%d %d\n", p->PRI, id);
//	for(int i=0;i<LEVEL_solution1;i++)printf("%d ", c_id[i]);
//	printf("\n");

	add_data_ordered(c_list + ((c_id[0] * dstIP_SIZE_2 + c_id[1]) * PORT_SIZE + c_id[2]) * PROTO_SIZE + c_id[3], &_d);
}

// solution3: protocal && sourceIP_2 && destinationIP_2 && destinationIP_3
void insert_solution3(Cell *c_list, rule *p) {
	unsigned int c_id[4]; //index cell id
	data _d;
	memcpy(&_d, p, sizeof(data));
	_d.source_mask = (unsigned short) p->source_mask;
	_d.destination_mask = (unsigned short) p->destination_mask;

	if ((unsigned int) p->protocol[0] == 0)c_id[layer1] = LAYER1_END_CELL;
	else {
		switch ((unsigned int) p->protocol[1]) {
			case ICMP:
				c_id[layer1] = 0;
				break;
			case IGMP:
				c_id[layer1] = 1;
				break;
			case GGP:
				c_id[layer1] = 2;
				break;
			case IP:
				c_id[layer1] = 3;
				break;
			case ST:
				c_id[layer1] = 4;
				break;
			case TCP:
				c_id[layer1] = 5;
				break;
			case CBT:
				c_id[layer1] = 6;
				break;
			case EGP:
				c_id[layer1] = 7;
				break;
			case UDP:
				c_id[layer1] = 8;
				break;
			case GRE:
				c_id[layer1] = 9;
				break;
			case ESP:
				c_id[layer1] = 10;
				break;
			case AH:
				c_id[layer1] = 11;
				break;
			case EIGRP:
				c_id[layer1] = 12;
				break;
			case OSPFIGP:
				c_id[layer1] = 13;
				break;
			default:
				fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
				c_id[layer1] = PROTO_END_CELL;
				return;
		}
	}
	unsigned int s_mask = (unsigned int) (p->source_mask >> 3);
	if (s_mask < 2)
		c_id[layer2] = LAYER2_END_CELL;
	else
		c_id[layer2] = p->source_ip[2] >> LAYER2_WIDTH;
	s_mask = (unsigned int) (p->destination_mask >> 3);
	switch (s_mask) {
		case 0: // 0-7
		case 1: // 8-15
			c_id[layer3] = LAYER3_END_CELL;
			c_id[layer4] = LAYER4_END_CELL;
			break;
		case 2:
			c_id[layer3] = p->destination_ip[2] >> LAYER3_WIDTH;
			c_id[layer4] = LAYER4_END_CELL;
			break;
		default:
			c_id[layer3] = p->destination_ip[2] >> LAYER3_WIDTH;
			c_id[layer4] = p->destination_ip[1] >> LAYER4_WIDTH;
			break;
	}
//	if (p->destination_port[0] == p->destination_port[1])c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
//	else if (p->destination_port[0] >> PORT_WIDTH == p->destination_port[1] >> PORT_WIDTH)
//		c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
//	else c_id[PORT_LAYER] = PORT_END_CELL;

//	int id = ((c_id[0] * srcIP_SIZE_2 + c_id[1]) * PORT_SIZE + c_id[2]) * PROTO_SIZE + c_id[3];
//	printf("%d %d\n", p->PRI, id);
//	for(int i=0;i<LEVEL_solution1;i++)printf("%d ", c_id[i]);
//	printf("\n");

	add_data_ordered(c_list + ((c_id[0] * LAYER2_SIZE + c_id[1]) * LAYER3_SIZE + c_id[2]) * LAYER4_SIZE + c_id[3], &_d);
}

// solution4: sourceIP_2 && destinationIP_2 && sourceIP_1 && destinationIP_3
void insert_solution4(Cell2 *c_list, rule *p, unsigned char **bitset) {
	unsigned int c_id[LEVEL_solution4]; //index cell id
	data2 _d;
	memcpy(&_d, p, sizeof(data2));

	unsigned int mask_8 = (unsigned int) (p->source_mask >> 3);
	switch (mask_8) {
		case 0: // 0-7
			c_id[layer1] = LAYER1_END_CELL;
			c_id[layer3] = LAYER3_END_CELL;
			break;
		case 1: // 8-15
			c_id[layer1] = LAYER1_END_CELL;
			c_id[layer3] = p->source_ip[3] >> LAYER3_WIDTH;
			break;
		case 2:
		case 3:
		case 4:
			c_id[layer1] = p->source_ip[2] >> LAYER1_WIDTH;
			c_id[layer3] = p->source_ip[3] >> LAYER3_WIDTH;
			break;
	}
	mask_8 = (unsigned int) (p->destination_mask >> 3);
	switch (mask_8) {
		case 0: // 0-7
		case 1: // 8-15
			c_id[layer2] = LAYER2_END_CELL;
			c_id[layer4] = LAYER4_END_CELL;
			break;
		case 2:
			c_id[layer2] = p->destination_ip[2] >> LAYER2_WIDTH;
			c_id[layer4] = LAYER4_END_CELL;
			break;
		case 3:
		case 4:
			c_id[layer2] = p->destination_ip[2] >> LAYER2_WIDTH;
			c_id[layer4] = p->destination_ip[1] >> LAYER4_WIDTH;
			break;
	}

//	if (mask_8 == 4) {
//		int bitsetNo = (int) p->destination_ip[0];
//		int unitNo=p->PRI/8;
//		int offset=p->PRI&7;
//		for (int i = 0; i < 256; i++) {
//			if (i != bitsetNo)
//				bitset[i][unitNo] = bitset[i][unitNo]|1<<offset;
//		}
//	}
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

	add_data_ordered2(c_list + ((c_id[0] * LAYER2_SIZE + c_id[1]) * LAYER3_SIZE + c_id[2]) * LAYER4_SIZE + c_id[3],
					  &_d);
}


int simple_match(ACL_rules *a, message *p, int *_cycle) {
	uint64_t time_1, time_2;
	time_1 = GetCPUCycle();
	unsigned int p_sip, p_dip, r_ip;
	memcpy(&p_sip, p->source_ip, 4);
	memcpy(&p_dip, p->destination_ip, 4);
	unsigned int p_sip_mv[32], p_dip_mv[32];
	for (int i = 0; i < 32; i++) {
		p_sip_mv[i] = p_sip >> i;
		p_dip_mv[i] = p_dip >> i;
	}
//	if (p_sip_mv[32]) {
//		printf("simple_match source IP mov32 = %u", p_sip_mv[32]);
//	}
	for (int i = 0; i < a->size; i++) {
		unsigned int m_bit = 32 - (unsigned int) a->list[i].source_mask;
		memcpy(&r_ip, a->list[i].source_ip, 4);
		if (m_bit != 32 && p_sip_mv[m_bit] != r_ip >> m_bit)continue;  //if source ip not match, check next
		m_bit = 32 - (unsigned int) a->list[i].destination_mask;  //comput the bit number need to move
		memcpy(&r_ip, a->list[i].destination_ip, 4);
		if (m_bit != 32 && p_dip_mv[m_bit] != r_ip >> m_bit)continue;  //if destination ip not match, check next
		if (p->source_port < a->list[i].source_port[0] || a->list[i].source_port[1] < p->source_port)
			continue;  //if source port not match, check next
		if (p->destination_port < a->list[i].destination_port[0] ||
			a->list[i].destination_port[1] < p->destination_port)
			continue;  //if destination port not match, check next
		if (a->list[i].protocol[0] != 0 && p->protocol != (unsigned int) a->list[i].protocol[1]) continue;
		time_2 = GetCPUCycle();
		*_cycle = (int) (time_2 - time_1);
		return i;
	}
	time_2 = GetCPUCycle();
	*_cycle = (int) (time_2 - time_1);
	return -1;
}

int match_solution1(Cell *_c, message *p) {
	unsigned int e_protocol, es_ip, ed_ip;
	unsigned short es_port, ed_port;
	e_protocol = p->protocol;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	es_port = p->source_port;
	ed_port = p->destination_port;

	unsigned int c_id[LEVEL_solution1][2];
	switch ((unsigned int) p->protocol) {
		case ICMP:
			c_id[PROTO_LAYER][0] = 0;
			break;
		case IGMP:
			c_id[PROTO_LAYER][0] = 1;
			break;
		case GGP:
			c_id[PROTO_LAYER][0] = 2;
			break;
		case IP:
			c_id[PROTO_LAYER][0] = 3;
			break;
		case ST:
			c_id[PROTO_LAYER][0] = 4;
			break;
		case TCP:
			c_id[PROTO_LAYER][0] = 5;
			break;
		case CBT:
			c_id[PROTO_LAYER][0] = 6;
			break;
		case EGP:
			c_id[PROTO_LAYER][0] = 7;
			break;
		case UDP:
			c_id[PROTO_LAYER][0] = 8;
			break;
		case GRE:
			c_id[PROTO_LAYER][0] = 9;
			break;
		case ESP:
			c_id[PROTO_LAYER][0] = 10;
			break;
		case AH:
			c_id[PROTO_LAYER][0] = 11;
			break;
		case EIGRP:
			c_id[PROTO_LAYER][0] = 12;
			break;
		case OSPFIGP:
			c_id[PROTO_LAYER][0] = 13;
			break;
		default:
			//fprintf(stderr, "Message Error - unknown message protocol %u!\n", e_protocol);
			c_id[PROTO_LAYER][0] = PROTO_END_CELL;
			break;
	}
	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
	c_id[srcIP_LAYER_1][0] = p->source_ip[3] >> srcIP_WIDTH_1;
	c_id[srcIP_LAYER_1][1] = srcIP_END_CELL_1;
	c_id[srcIP_LAYER_2][0] = p->source_ip[2] >> srcIP_WIDTH_2;
	c_id[srcIP_LAYER_2][1] = srcIP_END_CELL_2;
	c_id[PORT_LAYER][0] = p->destination_port >> PORT_WIDTH;
	c_id[PORT_LAYER][1] = PORT_END_CELL;

	int res = 0x7FFFFFFF;
	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[0][i] * srcIP_SIZE_2;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[1][j]) * PORT_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[2][v]) * PROTO_SIZE;
				for (int w = 0; w < 2; w++) {
					int id_4 = id_3 + c_id[3][w];
					int _size = _c[id_4].size;
					if (_size == 0)continue;
					data *_list = _c[id_4].list;
					unsigned int _ip;
					for (int u = 0; u < _size; u++) { //check in cell
						data *_d = _list + u;
						if (res < _d->PRI)break;
						unsigned int m_bit = 32 - (unsigned int) _d->source_mask;  //compute the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (es_ip >> m_bit != _ip >> m_bit)continue;  //if source ip not match, check next
						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (ed_ip >> m_bit != _ip >> m_bit)continue;  //if destination ip not match, check next
						if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  //if source port not match, check next
						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
							continue;  //if destination port not match, check next
						res = _d->PRI;
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;
	return res;
}

int match_with_log_solution1(Cell *_c, message *p, int *_cycle) {
//	uint64_t time_1, time_2;
//	time_1 = GetCPUCycle();
	unsigned int e_protocol, es_ip, ed_ip;
	unsigned short es_port, ed_port;
	e_protocol = p->protocol;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	es_port = p->source_port;
	ed_port = p->destination_port;

	unsigned int c_id[LEVEL_solution1][2];

	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
	c_id[srcIP_LAYER_1][0] = p->source_ip[3] >> srcIP_WIDTH_1;
	c_id[srcIP_LAYER_1][1] = srcIP_END_CELL_1;
	c_id[srcIP_LAYER_2][0] = p->source_ip[2] >> srcIP_WIDTH_2;
	c_id[srcIP_LAYER_2][1] = srcIP_END_CELL_2;
	c_id[PORT_LAYER][0] = p->destination_port >> PORT_WIDTH;
	c_id[PORT_LAYER][1] = PORT_END_CELL;
	switch ((unsigned int) p->protocol) {
		case ICMP:
			c_id[PROTO_LAYER][0] = 0;
			break;
		case IGMP:
			c_id[PROTO_LAYER][0] = 1;
			break;
		case GGP:
			c_id[PROTO_LAYER][0] = 2;
			break;
		case IP:
			c_id[PROTO_LAYER][0] = 3;
			break;
		case ST:
			c_id[PROTO_LAYER][0] = 4;
			break;
		case TCP:
			c_id[PROTO_LAYER][0] = 5;
			break;
		case CBT:
			c_id[PROTO_LAYER][0] = 6;
			break;
		case EGP:
			c_id[PROTO_LAYER][0] = 7;
			break;
		case UDP:
			c_id[PROTO_LAYER][0] = 8;
			break;
		case GRE:
			c_id[PROTO_LAYER][0] = 9;
			break;
		case ESP:
			c_id[PROTO_LAYER][0] = 10;
			break;
		case AH:
			c_id[PROTO_LAYER][0] = 11;
			break;
		case EIGRP:
			c_id[PROTO_LAYER][0] = 12;
			break;
		case OSPFIGP:
			c_id[PROTO_LAYER][0] = 13;
			break;
		default:
			//fprintf(stderr, "Message Error - unknown message protocol %u!\n", e_protocol);
			c_id[PROTO_LAYER][0] = PROTO_END_CELL;
			break;
	}

	int res = 0x7FFFFFFF;

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[0][i] * srcIP_SIZE_2;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[1][j]) * PORT_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[2][v]) * PROTO_SIZE;
				for (int w = 0; w < 2; w++) {
					//int id_4 = id_3 + c_id[3][w];
					Cell *q = _c + id_3 + c_id[3][w];
					//int _size = _c[id_4].size;
					if (q->size == 0)continue;
					//data* _list = _c[id_4].list;
					data *_d = q->list;
					unsigned int _ip;
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell
						if (res < _d->PRI)break;
						unsigned int m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (es_ip >> m_bit != _ip >> m_bit)continue;  //if source ip not match, check next
						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (ed_ip >> m_bit != _ip >> m_bit)continue;  //if destination ip not match, check next
						if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  //if source port not match, check next
						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
							continue;  //if destination port not match, check next
						res = _d->PRI;
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;

//	time_2 = GetCPUCycle();

//	*_cycle = (int) (time_2 - time_1);
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

int match_with_log2_solution1(Cell *_c, message *p, int *_cycle, int *checkNum) {
	*checkNum = 0;
//	uint64_t time_1, time_2;
//	time_1 = GetCPUCycle();
	unsigned int e_protocol, es_ip, ed_ip;
	unsigned short es_port, ed_port;
	e_protocol = p->protocol;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	es_port = p->source_port;
	ed_port = p->destination_port;

	unsigned int c_id[LEVEL_solution1][2];

	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
	c_id[srcIP_LAYER_1][0] = p->source_ip[3] >> srcIP_WIDTH_1;
	c_id[srcIP_LAYER_1][1] = srcIP_END_CELL_1;
	c_id[srcIP_LAYER_2][0] = p->source_ip[2] >> srcIP_WIDTH_2;
	c_id[srcIP_LAYER_2][1] = srcIP_END_CELL_2;
	c_id[PORT_LAYER][0] = p->destination_port >> PORT_WIDTH;
	c_id[PORT_LAYER][1] = PORT_END_CELL;
	switch ((unsigned int) p->protocol) {
		case ICMP:
			c_id[PROTO_LAYER][0] = 0;
			break;
		case IGMP:
			c_id[PROTO_LAYER][0] = 1;
			break;
		case GGP:
			c_id[PROTO_LAYER][0] = 2;
			break;
		case IP:
			c_id[PROTO_LAYER][0] = 3;
			break;
		case ST:
			c_id[PROTO_LAYER][0] = 4;
			break;
		case TCP:
			c_id[PROTO_LAYER][0] = 5;
			break;
		case CBT:
			c_id[PROTO_LAYER][0] = 6;
			break;
		case EGP:
			c_id[PROTO_LAYER][0] = 7;
			break;
		case UDP:
			c_id[PROTO_LAYER][0] = 8;
			break;
		case GRE:
			c_id[PROTO_LAYER][0] = 9;
			break;
		case ESP:
			c_id[PROTO_LAYER][0] = 10;
			break;
		case AH:
			c_id[PROTO_LAYER][0] = 11;
			break;
		case EIGRP:
			c_id[PROTO_LAYER][0] = 12;
			break;
		case OSPFIGP:
			c_id[PROTO_LAYER][0] = 13;
			break;
		default:
			fprintf(stderr, "Message Error - unknown message protocol %u!\n", e_protocol);
			c_id[PROTO_LAYER][0] = PROTO_END_CELL;
			break;
	}

	int res = 0x7FFFFFFF;

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[0][i] * srcIP_SIZE_2;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[1][j]) * PORT_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[2][v]) * PROTO_SIZE;
				for (int w = 0; w < 2; w++) {
					//int id_4 = id_3 + c_id[3][w];
					Cell *q = _c + id_3 + c_id[3][w];
					//int _size = _c[id_4].size;
					if (q->size == 0)continue;
					*checkNum = *checkNum + q->size;
					//data* _list = _c[id_4].list;
					data *_d = q->list;
					unsigned int _ip;
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell
						if (res < _d->PRI)break;
						unsigned int m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (es_ip >> m_bit != _ip >> m_bit) continue;  //if source ip not match, check next
						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (ed_ip >> m_bit != _ip >> m_bit) continue;  //if destination ip not match, check next
						if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  //if source port not match, check next
						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
							continue;  //if destination port not match, check next
						res = _d->PRI;
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;

//	time_2 = GetCPUCycle();

//	*_cycle = (int) (time_2 - time_1);
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

int match_with_log_solution2(Cell *_c, message *p, int *_cycle) {
//	uint64_t time_1, time_2;
//	time_1 = GetCPUCycle();
	unsigned short es_port = p->source_port, ed_port = p->destination_port;
	unsigned int e_protocol = p->protocol, es_ip, ed_ip;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	unsigned int c_id[LEVEL_solution2][2];

	c_id[PROTO_LAYER][1] = PROTO_END_CELL;
	c_id[dstIP_LAYER_1][0] = p->destination_ip[2] >> dstIP_WIDTH_1;
	c_id[dstIP_LAYER_1][1] = dstIP_END_CELL_1;
	c_id[dstIP_LAYER_2][0] = p->destination_ip[1] >> dstIP_WIDTH_2;
	c_id[dstIP_LAYER_2][1] = dstIP_END_CELL_2;
	c_id[PORT_LAYER][0] = p->destination_port >> PORT_WIDTH;
	c_id[PORT_LAYER][1] = PORT_END_CELL;
	switch ((unsigned int) p->protocol) {
		case ICMP:
			c_id[PROTO_LAYER][0] = 0;
			break;
		case IGMP:
			c_id[PROTO_LAYER][0] = 1;
			break;
		case GGP:
			c_id[PROTO_LAYER][0] = 2;
			break;
		case IP:
			c_id[PROTO_LAYER][0] = 3;
			break;
		case ST:
			c_id[PROTO_LAYER][0] = 4;
			break;
		case TCP:
			c_id[PROTO_LAYER][0] = 5;
			break;
		case CBT:
			c_id[PROTO_LAYER][0] = 6;
			break;
		case EGP:
			c_id[PROTO_LAYER][0] = 7;
			break;
		case UDP:
			c_id[PROTO_LAYER][0] = 8;
			break;
		case GRE:
			c_id[PROTO_LAYER][0] = 9;
			break;
		case ESP:
			c_id[PROTO_LAYER][0] = 10;
			break;
		case AH:
			c_id[PROTO_LAYER][0] = 11;
			break;
		case EIGRP:
			c_id[PROTO_LAYER][0] = 12;
			break;
		case OSPFIGP:
			c_id[PROTO_LAYER][0] = 13;
			break;
		default:
//			fprintf(stderr, "Solution2 Message Error - unknown message protocol %u!\n", e_protocol);
			c_id[PROTO_LAYER][0] = PROTO_END_CELL;
			break;
	}

	int res = 0x7FFFFFFF;

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[dstIP_LAYER_1][i] * dstIP_SIZE_2;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[dstIP_LAYER_2][j]) * PORT_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[PORT_LAYER][v]) * PROTO_SIZE;
				for (int w = 0; w < 2; w++) {
					//int id_4 = id_3 + c_id[3][w];
					Cell *q = _c + id_3 + c_id[PROTO_LAYER][w];
					//int _size = _c[id_4].size;
					if (q->size == 0)continue;
					//data* _list = _c[id_4].list;
					data *_d = q->list;
					unsigned int _ip;
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell
						if (res < _d->PRI)break;
						unsigned int m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (es_ip >> m_bit != _ip >> m_bit)continue;  //if source ip not match, check next
						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (ed_ip >> m_bit != _ip >> m_bit)continue;  //if destination ip not match, check next
						if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  //if source port not match, check next
						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
							continue;  //if destination port not match, check next
						res = _d->PRI;
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;

//	time_2 = GetCPUCycle();

//	*_cycle = (int) (time_2 - time_1);
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

// solution3: protocal && sourceIP_2 && destinationIP_2 && destinationIP_3
int match_with_log_solution3(Cell *_c, message *p, int *_cycle) {
//	uint64_t time_1, time_2;
//	time_1 = GetCPUCycle();
	unsigned short es_port = p->source_port, ed_port = p->destination_port;
	unsigned int e_protocol = p->protocol, es_ip, ed_ip;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	unsigned int p_sip_mv[33], p_dip_mv[33];
	for (int i = 0; i < 33; i++) {
		p_sip_mv[i] = es_ip >> i;
		p_dip_mv[i] = ed_ip >> i;
	}

	unsigned int c_id[LEVEL_solution3][2];

	c_id[layer1][1] = PROTO_END_CELL;
	c_id[layer2][0] = p->source_ip[2] >> LAYER2_WIDTH;
	c_id[layer2][1] = LAYER2_END_CELL;
	c_id[layer3][0] = p->destination_ip[2] >> LAYER3_WIDTH;
	c_id[layer3][1] = LAYER3_END_CELL;
	c_id[layer4][0] = p->destination_ip[1] >> LAYER4_WIDTH;
	c_id[layer4][1] = LAYER4_END_CELL;
	switch ((unsigned int) p->protocol) {
		case ICMP:
			c_id[layer1][0] = 0;
			break;
		case IGMP:
			c_id[layer1][0] = 1;
			break;
		case GGP:
			c_id[layer1][0] = 2;
			break;
		case IP:
			c_id[layer1][0] = 3;
			break;
		case ST:
			c_id[layer1][0] = 4;
			break;
		case TCP:
			c_id[layer1][0] = 5;
			break;
		case CBT:
			c_id[layer1][0] = 6;
			break;
		case EGP:
			c_id[layer1][0] = 7;
			break;
		case UDP:
			c_id[layer1][0] = 8;
			break;
		case GRE:
			c_id[layer1][0] = 9;
			break;
		case ESP:
			c_id[layer1][0] = 10;
			break;
		case AH:
			c_id[layer1][0] = 11;
			break;
		case EIGRP:
			c_id[layer1][0] = 12;
			break;
		case OSPFIGP:
			c_id[layer1][0] = 13;
			break;
		default:
//			fprintf(stderr, "Solution3 Message Error - unknown message protocol %u!\n", e_protocol);
			c_id[layer1][0] = PROTO_END_CELL;
			break;
	}

	int res = 0x7FFFFFFF;

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[layer1][i] * LAYER2_SIZE;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[layer2][j]) * LAYER3_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[layer3][v]) * LAYER4_SIZE;
				for (int w = 0; w < 2; w++) {
					Cell *q = _c + id_3 + c_id[layer4][w];
					if (q->size == 0)continue;
					data *_d = q->list;
					unsigned int _ip;
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell
						if (res < _d->PRI)break;
						unsigned int m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (m_bit != 32 && p_sip_mv[m_bit] != _ip >> m_bit)
							continue;  //if source ip not match, check next
						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (m_bit != 32 && p_dip_mv[m_bit] != _ip >> m_bit)
							continue;  //if destination ip not match, check next
						if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
							continue;  //if source port not match, check next
						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
							continue;  //if destination port not match, check next
						res = _d->PRI;
						break;
					}
				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;

//	time_2 = GetCPUCycle();

//	*_cycle = (int) (time_2 - time_1);
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

int hit = 0;
int miss = 0;

// solution4: sourceIP_2 && destinationIP_2 && sourceIP_1 && destinationIP_3
int match_with_log_solution4(Cell2 *_c, message *p, int *_cycle, unsigned char **bitset) {
	uint64_t time_1, time_2;
	time_1 = GetCPUCycle();
	unsigned char e_protocol = p->protocol;
	unsigned short es_port = p->source_port, ed_port = p->destination_port;
	unsigned int es_ip, ed_ip;
	memcpy(&es_ip, p->source_ip, 4);
	memcpy(&ed_ip, p->destination_ip, 4);
	unsigned int p_sip_mv[32], p_dip_mv[32];
	for (int i = 0; i < 32; i++) {
		p_sip_mv[i] = es_ip >> i;
		p_dip_mv[i] = ed_ip >> i;
	}

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
//	unsigned char* bitset_=bitset[bitsetNo];

	unsigned int c_id[LEVEL_solution4][2];

	c_id[layer1][0] = p->source_ip[2] >> LAYER1_WIDTH;
	c_id[layer1][1] = LAYER1_END_CELL;
	c_id[layer2][0] = p->destination_ip[2] >> LAYER2_WIDTH;
	c_id[layer2][1] = LAYER2_END_CELL;
	c_id[layer3][0] = p->source_ip[3] >> LAYER3_WIDTH;
	c_id[layer3][1] = LAYER3_END_CELL;
	c_id[layer4][0] = p->destination_ip[1] >> LAYER4_WIDTH;
	c_id[layer4][1] = LAYER4_END_CELL;

	int res = 0x7FFFFFFF;

	for (int i = 0; i < 2; i++) {
		int id_1 = c_id[layer1][i] * LAYER2_SIZE;
		for (int j = 0; j < 2; j++) {
			int id_2 = (id_1 + c_id[layer2][j]) * LAYER3_SIZE;
			for (int v = 0; v < 2; v++) {
				int id_3 = (id_2 + c_id[layer3][v]) * LAYER4_SIZE;
				for (int w = 0; w < 2; w++) {
					Cell2 *q = _c + id_3 + c_id[layer4][w];
					if (q->size == 0)continue;
					data2 *_d = q->list;
					unsigned int _ip, m_bit;
//					if(q->size>3000){
//						for (int u = 0; u < q->size; u++, ++_d) { //check in cell
//							if (res < _d->PRI)break;
//
//							if(bitset_[_d->PRI>>3]&1<<(_d->PRI&7)) {
//								hit++;
////								if(hit%1000000==0)
////								printf("if Cell%d %d, hit=%d\n",id_3 + c_id[3][w],_d->PRI,hit);
//								continue;
//							}
////							else{
////								miss++;
////								if(miss%1000000==0)
////								printf("else Cell%d %d, miss=%d\n",id_3 + c_id[3][w],_d->PRI,miss);
////							}
//
//							if (_d->protocol[0] != 0 && e_protocol != _d->protocol[1]) continue;
//
//							if (es_port < _d->source_port[0] || _d->source_port[1] < es_port)
//								continue;  //if source port not match, check next
//							if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port)
//								continue;  //if destination port not match, check next

//							 m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
//							memcpy(&_ip, _d->source_ip, 4);
//							if (m_bit != 32 && p_sip_mv[m_bit] != _ip >> m_bit)
//								continue;  //if source ip not match, check next
//
//							m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
//							memcpy(&_ip, _d->destination_ip, 4);
//							if (m_bit != 32 && p_dip_mv[m_bit] != _ip >> m_bit)
//								continue;  //if destination ip not match, check next

//							res = _d->PRI;
//							break;
//						}
//					}else{
					for (int u = 0; u < q->size; u++, ++_d) { //check in cell
//						if (res < _d->PRI)break;

//						if (_d->protocol[0] != 0 && e_protocol != _d->protocol[1]) continue;

						m_bit = 32 - (unsigned int) _d->destination_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->destination_ip, 4);
						if (m_bit != 32 && p_dip_mv[m_bit] != _ip >> m_bit)
//						if (m_bit != 32 && es_ip>>m_bit != _ip >> m_bit)
							continue;  //if destination ip not match, check next

						m_bit = 32 - (unsigned int) _d->source_mask;  //comput the bit number need to move
						memcpy(&_ip, _d->source_ip, 4);
						if (m_bit != 32 && p_sip_mv[m_bit] != _ip >> m_bit)
//						if (m_bit != 32 && ed_ip>>m_bit != _ip >> m_bit)
							continue;  //if source ip not match, check next

						if (ed_port < _d->destination_port[0] || _d->destination_port[1] < ed_port||es_port < _d->source_port[0] || _d->source_port[1] < es_port||_d->protocol[0] != 0 && e_protocol != _d->protocol[1])
							continue;  //if destination or source port not match, check next

						res = _d->PRI;
						break;
					}
//					}

				}
			}
		}
	}

	if (res == 0x7FFFFFFF)res = -1;

	time_2 = GetCPUCycle();

	*_cycle = (int) (time_2 - time_1);
	//printf("matching instruction cycle : %d\n", instruction_cycle);

	return res;
}

void get_cell_size(Cell *c, char outputFileName[], int cellSize) {
	int nullN = 0, maxV = 0;
	FILE *fp = NULL;
//	fp = fopen(outputFileName, "w");
	for (int i = 0; i < CELL_SIZE_solution3; i++) {
//		fprintf(fp, "ID: %d size: %d\n", i, (c + i)->size);
		if ((c[i].size) == 0) nullN++;
		else if (c[i].size > maxV)maxV = c[i].size;
	}
//	fclose(fp);
	printf("maxCell: %d; nullCell: %d; cellNum: %d; nullPercent: %f\n", maxV, nullN, CELL_SIZE_solution3,
		   (double) nullN / cellSize);
}

void get_cell_size2(Cell2 *c, char outputFileName[], int cellSize) {
	int nullN = 0, maxV = 0;
	FILE *fp = NULL;
//	fp = fopen(outputFileName, "w");
	for (int i = 0; i < CELL_SIZE_solution4; i++) {
//		fprintf(fp, "ID: %d size: %d\n", i, (c + i)->size);
		if ((c[i].size) == 0) nullN++;
		else if (c[i].size > maxV)maxV = c[i].size;
	}
//	fclose(fp);
	printf("maxCell: %d; nullCell: %d; cellNum: %d; nullPercent: %f\n", maxV, nullN, CELL_SIZE_solution3,
		   (double) nullN / cellSize);
}

void analyse_log(ACL_rules *data) {
	int _log[LEVEL_solution1][srcIP_SIZE_2] = {0};

	for (int i = 0; i < data->size; i++) {
		rule *p = data->list + i;
		unsigned int c_id[LEVEL_solution1]; //index cell id
		unsigned int s_mask = (unsigned int) (p->source_mask >> 3);

		if ((unsigned int) p->protocol[0] == 0)c_id[PROTO_LAYER] = PROTO_END_CELL;
		else {
			switch ((unsigned int) p->protocol[1]) {
				case ICMP:
					c_id[PROTO_LAYER] = 0;
					break;
				case IGMP:
					c_id[PROTO_LAYER] = 1;
					break;
				case GGP:
					c_id[PROTO_LAYER] = 2;
					break;
				case IP:
					c_id[PROTO_LAYER] = 3;
					break;
				case ST:
					c_id[PROTO_LAYER] = 4;
					break;
				case TCP:
					c_id[PROTO_LAYER] = 5;
					break;
				case CBT:
					c_id[PROTO_LAYER] = 6;
					break;
				case EGP:
					c_id[PROTO_LAYER] = 7;
					break;
				case UDP:
					c_id[PROTO_LAYER] = 8;
					break;
				case GRE:
					c_id[PROTO_LAYER] = 9;
					break;
				case ESP:
					c_id[PROTO_LAYER] = 10;
					break;
				case AH:
					c_id[PROTO_LAYER] = 11;
					break;
				case EIGRP:
					c_id[PROTO_LAYER] = 12;
					break;
				case OSPFIGP:
					c_id[PROTO_LAYER] = 13;
					break;
				default:
					fprintf(stderr, "Rule %d Error - unknown message protocol %u !\n", p->PRI, p->protocol[1]);
					return;
			}
		}
		switch (s_mask) {
			case 0:
				c_id[srcIP_LAYER_1] = srcIP_END_CELL_1;//int checkNum[],
				c_id[srcIP_LAYER_2] = srcIP_END_CELL_2;
				break;
			case 1:
				c_id[srcIP_LAYER_1] = p->source_ip[3] >> srcIP_WIDTH_1;
				c_id[srcIP_LAYER_2] = srcIP_END_CELL_2;
				break;
			default:
				c_id[srcIP_LAYER_1] = p->source_ip[3] >> srcIP_WIDTH_1;
				c_id[srcIP_LAYER_2] = p->source_ip[2] >> srcIP_WIDTH_2;
				break;
		}
		if (p->destination_port[0] == p->destination_port[1])c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
		else if (p->destination_port[0] >> PORT_WIDTH == p->destination_port[1] >> PORT_WIDTH)
			c_id[PORT_LAYER] = p->destination_port[0] >> PORT_WIDTH;
		else c_id[PORT_LAYER] = PORT_END_CELL;

		for (int j = 0; j < LEVEL_solution1; j++) {
			_log[j][c_id[j]]++;
		}
	}

	FILE *fp = NULL;
	fp = fopen("analyse_data.txt", "w");
	for (int i = 0; i < LEVEL_solution1; i++) {
		fprintf(fp, "%d ", i);
		for (int j = 0; j < srcIP_SIZE_2; j++)
			fprintf(fp, "%d ", _log[i][j]);
		fprintf(fp, "\n");
	}
	fclose(fp);
}

// Rule Distribution
void analyse_log2(ACL_rules *data, char outputFileName[]) {
	int *protocal = (int *) calloc(15, sizeof(int));

	void *storage = (void *) calloc(4 * 257, sizeof(int));
	int **srcIP = (void **) malloc(4 * sizeof(void *));
	for (int i = 0; i < 4; i++) {
		srcIP[i] = storage + i * 257 * sizeof(int); // 服务器环境
	}
	storage = (void *) calloc(4 * 257, sizeof(int));
	int **dstIP = (void **) malloc(4 * sizeof(void *));
	for (int i = 0; i < 4; i++) {
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
	int srcPort[portCellNum];
	int dstPort[portCellNum];
	memset(srcPort, 0, sizeof(srcPort));
	memset(dstPort, 0, sizeof(dstPort));

	for (int i = 0; i < data->size; i++) {
		rule *p = data->list + i;

		if ((unsigned int) p->protocol[0] == 0) protocal[0]++;
		else protocal[(unsigned int) p->protocol[1]]++;

		srcMaskType[(unsigned int) (p->source_mask)]++;
		switch ((unsigned int) (p->source_mask >> 3)) {
			case 0:
				srcIP[0][256]++;
				break;
			case 1:
				srcIP[0][(unsigned int) p->source_ip[3]]++;
				srcIP[1][256]++;
				break;
			case 2:
				srcIP[0][(unsigned int) p->source_ip[3]]++;
				srcIP[1][(unsigned int) p->source_ip[2]]++;
				srcIP[2][256]++;
				break;
			case 3:
				srcIP[0][(unsigned int) p->source_ip[3]]++;
				srcIP[1][(unsigned int) p->source_ip[2]]++;
				srcIP[2][(unsigned int) p->source_ip[1]]++;
				srcIP[3][256]++;
				break;
			case 4:
				srcIP[0][(unsigned int) p->source_ip[3]]++;
				srcIP[1][(unsigned int) p->source_ip[2]]++;
				srcIP[2][(unsigned int) p->source_ip[1]]++;
				srcIP[3][(unsigned int) p->source_ip[0]]++;
				break;
			default:
				printf("Error src_mask: %d\n", p->source_mask);
		}

		dstMaskType[(unsigned int) p->destination_mask]++;
		switch ((unsigned int) (p->destination_mask >> 3)) {
			case 0:
				dstIP[0][256]++;
				break;
			case 1:
				dstIP[0][(unsigned int) p->destination_ip[3]]++;
				dstIP[1][256]++;
				break;
			case 2:
				dstIP[0][(unsigned int) p->destination_ip[3]]++;
				dstIP[1][(unsigned int) p->destination_ip[2]]++;
				dstIP[2][256]++;
				break;
			case 3:
				dstIP[0][(unsigned int) p->destination_ip[3]]++;
				dstIP[1][(unsigned int) p->destination_ip[2]]++;
				dstIP[2][(unsigned int) p->destination_ip[1]]++;
				dstIP[3][256]++;
				break;
			case 4:
				dstIP[0][(unsigned int) p->destination_ip[3]]++;
				dstIP[1][(unsigned int) p->destination_ip[2]]++;
				dstIP[2][(unsigned int) p->destination_ip[1]]++;
				dstIP[3][(unsigned int) p->destination_ip[0]]++;
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
	}

	long long int protocolAvg = 0, protocalVar = 0, protocolNum = 0;
	for (int i = 0; i < 15; i++)
		if (protocal[i] > 0) {
			protocolAvg += protocal[i];
			protocolNum++;
		}
	protocolAvg /= protocolNum;
	for (int i = 0; i < 15; i++)
		if (protocal[i] > 0)
			protocalVar += pow(protocal[i] - protocolAvg, 2);
	protocalVar = sqrt(protocalVar / protocolNum);

	long long int IPvar[8], IPavg[8];
	memset(IPvar, 0, sizeof(IPvar));
	memset(IPavg, 0, sizeof(IPavg));
	for (int i = 0; i < 257; i++) {
		for (int j = 0; j < 4; j++) {
			IPavg[j] += srcIP[j][i];
			IPavg[j + 4] += dstIP[j][i];
		}
	}
	for (int i = 0; i < 8; i++)
		IPavg[i] /= 257;
	for (int i = 0; i < 257; i++) {
		for (int j = 0; j < 4; j++) {
			IPvar[j] += pow(srcIP[j][i] - IPavg[j], 2);
			IPvar[j + 4] += pow(dstIP[j][i] - IPavg[j + 4], 2);
		}
	}
	for (int i = 0; i < 8; i++)
		IPvar[i] = (int) sqrt((double) IPvar[i] / 257);

	long long int portVar[2], portAvg[2];
	memset(portVar, 0, sizeof(portVar));
	memset(portAvg, 0, sizeof(portAvg));
	for (int i = 0; i < portCellNum; i++) {
		portAvg[0] += srcPort[i];
		portAvg[1] += dstPort[i];
	}
	portAvg[0] /= portCellNum;
	portAvg[1] /= portCellNum;
	for (int i = 0; i < 257; i++) {
		portVar[0] += pow(srcPort[i] - portAvg[0], 2);
		portVar[1] += pow(dstPort[i] - portAvg[1], 2);
	}
	for (int i = 0; i < 2; i++)
		portVar[i] = (int) sqrt((double) portVar[i] / portCellNum);

	FILE *fp = NULL;
	fp = fopen(outputFileName, "w");
	fprintf(fp, "Total Rules: %d\n\n", data->size);
	for (int i = 0; i < 15; i++) {
		if (i > 0 && i % 5 == 0)fprintf(fp, "\n");
		fprintf(fp, "P%d=%-6d\t", i, protocal[i]);
	}
	fprintf(fp, "\nprotocalVar= %d\n\n", protocalVar);

	fprintf(fp, "srcMask=0: %d\n", srcMaskType[0]);
	for (int i = 1; i < 33; i++) {
		fprintf(fp, "%2d: %4d\t", i, srcMaskType[i]);
		if (i % 8 == 0) fprintf(fp, "\n");
	}
	fprintf(fp, "\ndstMask=0: %d\n", srcMaskType[0]);
	for (int i = 1; i < 33; i++) {
		fprintf(fp, "%2d: %4d\t", i, dstMaskType[i]);
		if (i % 8 == 0) fprintf(fp, "\n");
	}
//	for(int i=0;i<33;i++){
//		totalSrcMaskType[i]+=srcMaskType[i];
//		totalDstMaskType[i]+=dstMaskType[i];
//	}

	fprintf(fp, "\nIP\tsrc1\tsrc2\tsrc3\tsrc4\tdst1\tdst2\tdst3\tdst4\tsrcPort\tdstPort\n");
	for (int i = 0; i < MAX(257, portCellNum); i++) {
		if (i < MIN(257, portCellNum))
			fprintf(fp, "%d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\n", i, srcIP[0][i], srcIP[1][i],
					srcIP[2][i], srcIP[3][i], dstIP[0][i], dstIP[1][i], dstIP[2][i], dstIP[3][i], srcPort[i],
					dstPort[i]);
		else if (i < 257)
			fprintf(fp, "%d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\t%4d\n", i, srcIP[0][i], srcIP[1][i], srcIP[2][i],
					srcIP[3][i], dstIP[0][i], dstIP[1][i], dstIP[2][i], dstIP[3][i]);
		else
			fprintf(fp, "%d\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t%4d\t%4d\n", i, srcPort[i], dstPort[i]);
	}
	fprintf(fp, "var\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld\t%4ld", IPvar[0], IPvar[1], IPvar[2],
			IPvar[3], IPvar[4],
			IPvar[5], IPvar[6], IPvar[7], portVar[0], portVar[1]);

	fclose(fp);
	free(srcIP[0]), free(srcIP), free(dstIP[0]), free(dstIP), free(protocal);
}

double get_memory(Cell *c_list) {
	size_t mem = CELL_SIZE_solution4 * sizeof(Cell);
	for (int i = 0; i < CELL_SIZE_solution4; i++) {
		mem = mem + (c_list + i)->capacity * sizeof(data);
	}
//	printf("%u B\n", mem);
	double res = (double) mem / 1024.0 / 1024.0;
	return res;
}

double get_memory2(Cell2 *c_list) {
	size_t mem = CELL_SIZE_solution4 * sizeof(Cell2);
	for (int i = 0; i < CELL_SIZE_solution4; i++) {
		mem = mem + (c_list + i)->capacity * sizeof(data2);
	}
//	printf("%u B\n", mem);
	double res = (double) mem / 1024.0 / 1024.0;
	return res;
}

void visualize(int c[], int checkNum[], int size) {
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

		if (checkNum[i] >= 500) cn1++;
		if (checkNum[i] >= 506) {
			eID_cn2[cn2++] = i;
		}
		if (maxCheckNum < checkNum[i]) {
			maxCheckNum = checkNum[i];
			maxCNEID = i;
		}
		checkNumSum += checkNum[i];
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