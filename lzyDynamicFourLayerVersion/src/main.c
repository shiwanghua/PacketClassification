#define _GNU_SOURCE

#include <sched.h>

#include"core.h"
#include"read.h"
#include <sys/time.h>
#include <assert.h>

void test()
{
	srand((int)time(0));
	ACL_rules datasets = { 0, 0, 0 };
	ACL_messages message_sets = { 0, 0, 0 };
	read_rules("classbench_256k_s/acl1_256k.txt", &datasets);
	read_messages("classbench_256k_s/acl1_256k_trace.txt", &message_sets);
	double iCycle, oneMatch = 0, memoryAccess = 0;
	unsigned int m_bit, r_ip;
	int num = 1000000, count;
	message p = message_sets.list[rand() % message_sets.size];
	rule r = datasets.list[rand() % datasets.size];
	count = 0;
	iCycle = GetCPUCycle();
	for (int i = 0; i < num; i++)
	{

//		iCycle = GetCPUCycle();
//		rule r = datasets.list[rand() % datasets.size];
//		message p = message_sets.list[rand() % message_sets.size];
//		memoryAccess+=(double) GetCPUCycle() - iCycle;

		iCycle = GetCPUCycle();

		unsigned int p_sip, p_dip, r_ip;
		memcpy(&p_sip, p.source_ip, 4);
		memcpy(&p_dip, p.destination_ip, 4);
		m_bit = 32 - (unsigned int)r.source_mask;
		memcpy(&r_ip, r.source_ip, 4);
		if (m_bit != 32 && p_sip >> m_bit != r_ip >> m_bit)count++;  //if source ip not match, check next
		m_bit = 32 - (unsigned int)r.destination_mask;  //comput the bit number need to move
		memcpy(&r_ip, r.destination_ip, 4);
		if (m_bit != 32 && p_dip >> m_bit != r_ip >> m_bit)count++;  //if destination ip not match, check next
		if (p.source_port < r.source_port[0] || r.source_port[1] < p.source_port)
			count++;  //if source port not match, check next
		if (p.destination_port < r.destination_port[0] ||
			r.destination_port[1] < p.destination_port)
			count++;  //if destination port not match, check next
		if (r.protocol[0] != 0 && p.protocol != (unsigned int)r.protocol[1]) count++;
		oneMatch += (double)GetCPUCycle() - iCycle;
	}
//	oneMatch+=(double) GetCPUCycle() - iCycle;
	printf("count=%d, num=%d, oneMessageMatchOneRule= %f\n\nmemoryAccess=%f\n", count, num, oneMatch / num,
		memoryAccess / (2 * num));

}

int main()
{
	int core_id = 8;

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core_id, &mask);
	sched_setaffinity(0, sizeof(mask), &mask);

//	test();
	srand((int)time(0));
#if PROTO
	printf("protocol %d\n", PROTO_LAYER);
#endif
#if SIP_1
	printf("s_ip_1 %d %d %d %d\n", SIP_1_LAYER, SIP_SIZE_1, SIP_EDN_CELL_1, SIP_WIDTH_1);
#endif
#if SIP_2
	printf("s_ip_2 %d %d %d %d\n", SIP_2_LAYER, SIP_SIZE_2, SIP_EDN_CELL_2, SIP_WIDTH_2);
#endif
#if SIP_3
	printf("s_ip_3 %d %d %d %d\n", SIP_3_LAYER, SIP_SIZE_3, SIP_EDN_CELL_3, SIP_WIDTH_3);
#endif
#if SIP_4
	printf("s_ip_4 %d\n", SIP_4_LAYER);
#endif
#if DIP_1
	printf("d_ip_1 %d %d %d %d\n", DIP_1_LAYER, DIP_SIZE_1, DIP_EDN_CELL_1, DIP_WIDTH_1);
#endif
#if DIP_2
	printf("d_ip_2 %d %d %d %d\n", DIP_2_LAYER, DIP_SIZE_2, DIP_EDN_CELL_2, DIP_WIDTH_2);
#endif
#if DIP_3
	printf("d_ip_3 %d %d %d %d\n", DIP_3_LAYER, DIP_SIZE_3, DIP_EDN_CELL_3, DIP_WIDTH_3);
#endif
#if DIP_4
	printf("d_ip_4 %d %d %d %d\n", DIP_4_LAYER, DIP_SIZE_4, DIP_EDN_CELL_4, DIP_WIDTH_4);
#endif
#if SPORT
	printf("s_port %d %d %d %d\n", SPORT_LAYER, SPORT_SIZE, SPORT_END_CELL, SPORT_WIDTH);
#endif
#if DPORT
	printf("d_port %d %d %d %d\n", DPORT_LAYER, DPORT_SIZE, DPORT_END_CELL, DPORT_WIDTH);
#endif
	printf("%d %d %d %d\n\n", LAYER_0, LAYER_1, LAYER_2, LAYER_3);

	/*
	char ruleFileName[12][50] = {"acl1_256k.txt", "acl2_256k.txt", "acl3_256k.txt", "acl4_256k.txt", "acl5_256k.txt",
								 "fw1_256k.txt", "fw2_256k.txt", "fw3_256k.txt", "fw4_256k.txt", "fw5_256k.txt",
								 "ipc1_256k.txt", "ipc2_256k.txt"};
	char headFileName[12][50] = {"acl1_256k_trace.txt", "acl2_256k_trace.txt", "acl3_256k_trace.txt",
								 "acl4_256k_trace.txt", "acl5_256k_trace.txt", "fw1_256k_trace.txt",
								 "fw2_256k_trace.txt", "fw3_256k_trace.txt", "fw4_256k_trace.txt", "fw5_256k_trace.txt",
								 "ipc1_256k_trace.txt", "ipc2_256k_trace.txt"};
	*/


//	char ruleFileName[numDataSets][100] = {"", "rules2.txt", "rules3.txt", "rules4.txt", "rules5.txt", "rules6.txt"};
//	char headFileName[numDataSets][100] = {"", "tuples2.txt", "tuples3.txt", "tuples4.txt", "tuples5.txt", "tuples6.txt"};

	const int numDataSets = 17;
	char ruleFileName[numDataSets][50] = { "acl128_same_fmt_with_cb256k/acl_rules2_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_rules3_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_rules4_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_rules5_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_rules6_cb.txt",
										   "classbench_256k/acl1_256k.txt", "classbench_256k/acl2_256k.txt",
										   "classbench_256k/acl3_256k.txt", "classbench_256k/acl4_256k.txt",
										   "classbench_256k/acl5_256k.txt", "classbench_256k/fw1_256k.txt",
										   "classbench_256k/fw2_256k.txt", "classbench_256k/fw3_256k.txt",
										   "classbench_256k/fw4_256k.txt", "classbench_256k/fw5_256k.txt",
										   "classbench_256k/ipc1_256k.txt", "classbench_256k/ipc2_256k.txt"};
	char headFileName[numDataSets][50] = { "acl128_same_fmt_with_cb256k/acl_tuples2_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_tuples3_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_tuples4_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_tuples5_cb.txt",
										   "acl128_same_fmt_with_cb256k/acl_tuples6_cb.txt",
										   "classbench_256k/acl1_256k_trace.txt", "classbench_256k/acl2_256k_trace.txt",
										   "classbench_256k/acl3_256k_trace.txt",
										   "classbench_256k/acl4_256k_trace.txt", "classbench_256k/acl5_256k_trace.txt",
										   "classbench_256k/fw1_256k_trace.txt",
										   "classbench_256k/fw2_256k_trace.txt", "classbench_256k/fw3_256k_trace.txt",
										   "classbench_256k/fw4_256k_trace.txt",
										   "classbench_256k/fw5_256k_trace.txt", "classbench_256k/ipc1_256k_trace.txt",
										   "classbench_256k/ipc2_256k_trace.txt"};
	double totalMatchCycle = 0;
	double totalInsertCycle = 0;

	for (int q = 0; q < numDataSets; q++)
	{
		ACL_rules datasets = { 0, 0, 0 };
		ACL_messages message_sets = { 0, 0, 0 };
//		Cell *index;
//		index = (Cell *) calloc(CELL_SIZE, sizeof(Cell));

//#if useShuffleRule == 0
//		char tmpFileName[100] = "classbench_256k_s/";
//#elif useShuffleRule == 1
//		char tmpFileName[100] = "classbench_256k/";
//#else
//		char tmpFileName[100] = "acl_128k_test/";
//#endif
//		strcat(tmpFileName, ruleFileName[q]);
//		read_rules(tmpFileName, &datasets);
		read_rules_bitset(ruleFileName[q], &datasets);

/*
#if useShuffleRule
		for (int i = 0, to; i < datasets.size; i++) {
			to = rand() % datasets.size;
			rule r = datasets.list[i];
			datasets.list[i] = datasets.list[to];
			datasets.list[to] = r;
		}
#endif
*/

//		strcpy(tmpFileName, "output/rule_distribution_");
//		strcat(tmpFileName, ruleFileName[q]);
//		analyse_log2(&datasets, tmpFileName);

//		strcpy(tmpFileName, "output/rule_protocol_");
//		strcat(tmpFileName, ruleFileName[q]);
//		printProtocolType(&datasets, tmpFileName);

//#if useShuffleHeader == 0
//		strcpy(tmpFileName, "classbench_256k_s/");
//#elif useShuffleHeader == 1
//		strcpy(tmpFileName, "classbench_256k/");
//#else
//		strcpy(tmpFileName, "acl_128k_test/");
//#endif
//		strcat(tmpFileName, headFileName[q]);
		read_messages(headFileName[q], &message_sets);

//		int numUnit = (datasets.size + sizeof(char) - 1) / sizeof(char);
//		unsigned char ** bitset=(unsigned char **) malloc(256 * sizeof(unsigned char*));
//		for(int i=0;i<256;i++){
//			bitset[i]=(unsigned char *) calloc(numUnit, sizeof(unsigned char));
//		}

		unsigned long** bitset;
		init_bitset_IPv4(datasets.size, &bitset);

		double insertCycle = GetCPUCycle();
		for (int i = 0; i < datasets.size; i++)
//			insert(index, datasets.list + i);
//			insert_cache(index, datasets.list + i,bitset);
			insert_bitset_forward_IPv4(datasets.list + i, bitset);
		insertCycle = ((double)GetCPUCycle() - insertCycle) / datasets.size;
		totalInsertCycle += insertCycle;
		printf("%s\navgInsertCycle= %f\n%d %d %d n_rule=%d, n_head=%d\n", ruleFileName[q], insertCycle, sizeof(data),
			sizeof(rule), sizeof(Cell),
			datasets.size, message_sets.size);
//		printf("MemoryUse: %f MB\n", get_memory(index));
//		get_cell_size(index);

//		visualize_bitsets(bitset);

#if ENABLE_ANALYSE
		//analyse_log(&datasets);
#endif
		unsigned long checkNum = 0;
		int res = 0;
		int intervalRule = 1;
		double setCycle = 0;
		int oneCycle = 0;
		int cycleList[message_sets.size];
		MatchLog match_log;
		match_log.list = (LogInCell*)malloc(16 * sizeof(LogInCell));
		FILE* res_fp = NULL;
		strcpy(tmpFileName, "output/match_cycle_");
		strcat(tmpFileName, ruleFileName[q]);
		res_fp = fopen(tmpFileName, "w");
		struct timeval starttime, endtime;
		gettimeofday(&starttime, 0);
		uint64_t time_1, time_2;
		time_1 = GetCPUCycle();
//		clock_t start = clock();
		for (int i = 0; i < message_sets.size; i += intervalRule)
		{
//			res = match_with_log(index, message_sets.list + i, &oneCycle, &match_log);
//			res = match_with_log_cache(index, message_sets.list + i, bitset, &oneCycle, &match_log);
			res = match_bitset_forward_IPv4(datasets.list - 1, message_sets.list + i, bitset, &oneCycle, &checkNum);
			if (res != (message_sets.list + i)->rule_id)
			{
				if (res == -1)
				{
					if ((message_sets.list + i)->rule_id != 0)
						printf("%d fails: %d !=%d.\n", i, res, (message_sets.list + i)->rule_id);
					assert((message_sets.list + i)->rule_id == 0);
				}
				else
				{
					printf("%d header fails: %d !=%d.\n", i, res, (message_sets.list + i)->rule_id);
					assert(0);
				}
			}

//			res = match_with_log_cache(index, message_sets.list + i,bitset, &oneCycle, &match_log);
//			setCycle+=oneCycle;
//			cycleList[i] = oneCycle;
#if ENABLE_LOG
			fprintf(res_fp, "message %d match_rule %d cycle %f check_rules %d check_element %d\n", i + 100, res, cycle / 100.0, match_log.rules, match_log.ele);
		for (int j = 0; j < 16; j++)
			fprintf(res_fp, "\tid %d %d %d %d %d size %d CRul %d CEle %d match %d\n",
				match_log.list[j].id, match_log.list[j].layer[0], match_log.list[j].layer[1], match_log.list[j].layer[2], match_log.list[j].layer[3],
				match_log.list[j].size, match_log.list[j].rules, match_log.list[j].ele, match_log.list[j].match);
#else
//			fprintf(res_fp, "message %d match_rule %d cycle %f\n", i, res, oneCycle);
#endif
		}
		time_2 = GetCPUCycle();
		setCycle = (double)(time_2 - time_1);
		totalMatchCycle += setCycle / message_sets.size;
		printf("avgCycle= %f\n\n", setCycle / message_sets.size);
		gettimeofday(&endtime, 0);
		double timeuse = 1000000 * (endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
		printf("avgCheckNum= %.2f\n", (double)checkNum / message_sets.size);
		printf("avgMatchTime= %fus\n\n", timeuse / message_sets.size);
//		clock_t stop = clock();
//		double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
//		printf("Time elapsed in ms: %f\n\n\n", elapsed);
		fclose(res_fp);

//		visualize(cycleList, message_sets.size);

		//for (int i = 0; i < CELL_SIZE; i++)free(index[i].list);  // free cell list
		//free(index);  // free cell
		free(message_sets.list); // free message list
		free(datasets.list);  // free dataset list
		free(match_log.list);  // free log list
	}
	printf("\n\navgTotalCycle= %f\n", totalMatchCycle / numDataSets);
	printf("\n\navgInsertCycle= %f\n", totalInsertCycle / numDataSets);
	return 0;
}

