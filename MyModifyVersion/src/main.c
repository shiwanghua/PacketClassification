#include"core.h"
#include"read.h"
#include <unistd.h>
#include <sys/time.h>

int test1() {
	srand((int) time(0));
//	unsigned int a=32;
//	printf("%d\n",a>>32);
//	printf("%d\n",16-__builtin_ctz(65-1));
//	printf("%d\n",16-__builtin_ctz(257-1));
//	int a,b;
//	printf("%p\n%p",&a,&b);
//	unsigned char c[4];
//	printf("%p\n%p\n",c,&c[1]);
//	unsigned int a=66;
//	c[0]=(unsigned char)a;
//	c[1]=0;
//	c[2]=0;
//	c[3]=0;
//	unsigned int s;
//	memcpy(&s, c, 4);
//	printf("%d\n",(unsigned int)c[0]);
//	printf("%d\n",s);
//	printf("%u\n",dip >> dm << dm);
//	x = rand();
//	x=0x80000001;
//	printf("%x\n", x);
//	dip = x % (unsigned int) (1 << 31);
//	printf("%x\n", dip);

//	unsigned int dip = 0x3ffffff, dm = 25, x,sip=3562079352,sm=2;
//	printf("%x\n", sip);
//	printf("%x\n", sip>>sm);
//	printf("%x\n", sip>>sm<<sm);
//	printf("%u\n\n", sip>>sm<<sm);
//
//	dip=rand();
//	printf("%x\n", dip);
//	printf("%x\n", dip<<(32-sm));
//	printf("%x\n\n", dip<<(32-sm)>>(32-sm));
//	sip = (sip >> sm << sm) + (dip << (32 - sm) >> (32 - sm));
//	printf("%x\n", sip);
//	printf("%u\n", sip);

//	int q=0x80000000;
//	q=q>>31;
//	printf("%x\n",q);

//	int a[50000]={1,2,3,4,5};
//	for(int i=0;i<50000;i++)
//		a[i]=i;
//	memmove(a+10000 ,a,40000*sizeof(int));
//	printf("%d ",a[9999]);
//	printf("%d ",a[10000]);
//	printf("%d ",a[45678]);

/*	printf("%d\n", sizeof(unsigned long));
	printf("%d\n", sizeof(unsigned long int));
	printf("%d\n", sizeof(unsigned long long));
	printf("%d\n", sizeof(unsigned long long int));
	printf("%d\n", sizeof(int64_t)); // signed long int*/
//	printf("%d",sizeof(unsigned int64_t));
	ACL_rules datasets = {0, 0, 0};
	ACL_messages message_sets = {0, 0, 0};
	read_rules("classbench_256k_s/acl1_256k.txt", &datasets);
	read_messages("classbench_256k_s/acl1_256k_trace.txt", &message_sets);
	double iCycle, oneMatch=0,memoryAccess=0;
	unsigned int m_bit, r_ip;
	int num = 1000000, count;
	message p = message_sets.list[rand() % message_sets.size];
	rule r = datasets.list[rand() % datasets.size];
	count = 0;
	iCycle = GetCPUCycle();
	for (int i = 0; i < num; i++) {

//		iCycle = GetCPUCycle();
		rule r = datasets.list[rand() % datasets.size];
		message p = message_sets.list[rand() % message_sets.size];
//		memoryAccess+=(double) GetCPUCycle() - iCycle;

//		iCycle = GetCPUCycle();

		unsigned int p_sip, p_dip, r_ip;
		memcpy(&p_sip, p.source_ip, 4);
		memcpy(&p_dip, p.destination_ip, 4);
		m_bit = 32 - (unsigned int) r.source_mask;
		memcpy(&r_ip, r.source_ip, 4);
		if (m_bit != 32 && p_sip >> m_bit != r_ip >> m_bit)count++;  //if source ip not match, check next
		m_bit = 32 - (unsigned int) r.destination_mask;  //comput the bit number need to move
		memcpy(&r_ip, r.destination_ip, 4);
		if (m_bit != 32 && p_dip >> m_bit != r_ip >> m_bit)count++;  //if destination ip not match, check next
		if (p.source_port < r.source_port[0] || r.source_port[1] < p.source_port)
			count++;  //if source port not match, check next
		if (p.destination_port < r.destination_port[0] ||
			r.destination_port[1] < p.destination_port)
			count++;  //if destination port not match, check next
		if (r.protocol[0] != 0 && p.protocol != (unsigned int) r.protocol[1]) count++;
//		oneMatch += (double) GetCPUCycle() - iCycle;
	}
	oneMatch+=(double) GetCPUCycle() - iCycle;
	printf("count=%d, num=%d, oneMessageMatchOneRule= %f\n\nmemoryAccess=%f\n", count, num,oneMatch / num,memoryAccess/(2*num));


}

int main() {
	srand((int) time(0));
//	test1();
//	generateMessage();
	char ruleFileName[12][50] = {"acl1_256k.txt", "acl2_256k.txt", "acl3_256k.txt", "acl4_256k.txt", "acl5_256k.txt",
								 "fw1_256k.txt", "fw2_256k.txt", "fw3_256k.txt", "fw4_256k.txt", "fw5_256k.txt",
								 "ipc1_256k.txt", "ipc2_256k.txt"};
	char headFileName[12][50] = {"acl1_256k_trace.txt", "acl2_256k_trace.txt", "acl3_256k_trace.txt",
								 "acl4_256k_trace.txt", "acl5_256k_trace.txt", "fw1_256k_trace.txt",
								 "fw2_256k_trace.txt", "fw3_256k_trace.txt", "fw4_256k_trace.txt", "fw5_256k_trace.txt",
								 "ipc1_256k_trace.txt", "ipc2_256k_trace.txt"};
	double totalMatchCycle = 0;
	double totalInsertCycle = 0;

	for (int q = 0; q < 12; q++) {
		ACL_rules datasets = {0, 0, 0};
		ACL_messages message_sets = {0, 0, 0};
		Cell2 *index = (Cell2 *) calloc(CELL_SIZE_solution4, sizeof(Cell2));

#if useShuffleRule // 这里其实不用分,两个文件夹下的规则集是一样的
		char tmpFileName[100] = "classbench_256k_s/";
#else
		char tmpFileName[100] = "classbench_256k/";
#endif
		strcat(tmpFileName, ruleFileName[q]);
		read_rules(tmpFileName, &datasets);

#if useShuffleRule
		for (int i = 0, to; i < datasets.size; i++) {
			to = rand() % datasets.size;
			rule r = datasets.list[i];
			datasets.list[i] = datasets.list[to];
			datasets.list[to] = r;
		}
#endif

#if useShuffleHeader
		strcpy(tmpFileName, "classbench_256k_s/");
#else
		strcpy(tmpFileName, "classbench_256k/");
#endif
		strcat(tmpFileName, headFileName[q]);
		read_messages(tmpFileName, &message_sets);

//		int numUnit = (datasets.size + sizeof(char) - 1) / sizeof(char);
		unsigned char **bitset = (unsigned char **) malloc(14 * sizeof(unsigned char *));
//		for (int i = 0; i < 14; i++) {
//			bitset[i] = (unsigned char *) calloc(numUnit, sizeof(unsigned char));
//		}

		double iCycle = GetCPUCycle();;
		for (int i = 0; i < datasets.size; i++)
			insert_solution4(index, datasets.list + i, bitset);
		iCycle = ((double) GetCPUCycle() - iCycle) / datasets.size;
		totalInsertCycle += iCycle;
		printf("%s\navgInsertCycle= %f\n%d %d %d n_rule=%d, n_head=%d\n", ruleFileName[q], iCycle, sizeof(data),
			   sizeof(rule), sizeof(Cell),
			   datasets.size, message_sets.size);
		printf("MemoryUse: %f MB\n", get_memory2(index));

//		strcpy(tmpFileName, "output/rule_distribution_");
//		strcat(tmpFileName, ruleFileName[q]);
//		analyse_log2(&datasets, tmpFileName);

		strcpy(tmpFileName, "output/CELL_SIZE_");
		strcat(tmpFileName, ruleFileName[q]);
		get_cell_size2(index, tmpFileName, CELL_SIZE_solution4);

		int res = 0;
		int mCycle = 0;
		double setCycle = 0;
//		int checkNum = 0;
		int cycleList[message_sets.size];
		int checkNumList[message_sets.size];
		FILE *res_fp;
		strcpy(tmpFileName, "output/match_cycle_");
		strcat(tmpFileName, ruleFileName[q]);
		res_fp = fopen(tmpFileName, "w");

		uint64_t time_1, time_2;
		time_1 = GetCPUCycle();
		struct timeval starttime, endtime;
		gettimeofday(&starttime, 0);
//		clock_t start = clock();
		for (int i = 0; i < message_sets.size; i++) {
			res = match_with_log_solution4(index, &message_sets.list[i], &mCycle, bitset);
//			setCycle += mCycle;
//		res = match_with_log2_solution1(index, &message_sets.list[i], &cycle,&checkNum);
//			cycleList[i] = mCycle;
//		checkNumList[i]=checkNum;
//			fprintf(res_fp, "message %d match_rule %d cycle %d\n", i, res, mCycle);
		}
		time_2 = GetCPUCycle();
		totalMatchCycle += (time_2 - time_1) / message_sets.size;
		printf("avgMatchCycle= %f\n\n", (double) (time_2 - time_1) / message_sets.size);
//		totalMatchCycle += setCycle / message_sets.size;

		gettimeofday(&endtime, 0);
		double timeuse = 1000000 * (endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
		printf("avgMatchTime= %fus\n\n", timeuse / message_sets.size);
//		clock_t stop = clock();
//		double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
//		printf("Time elapsed in ms: %f\n\n\n", elapsed);
		fclose(res_fp);
		visualize(cycleList, checkNumList, message_sets.size);

		// simple_match
//		printf("simple_match %s\n",ruleFileName[q]);
//		strcpy(tmpFileName,"output/simple_match_cycle_");
//		strcat(tmpFileName, ruleFileName[q]);
//		res_fp = fopen(tmpFileName, "w");
//		for (int i = 0; i < message_sets.size; i++) {
//			res = simple_match(&datasets, &message_sets.list[i], &cycle);
////			cycleList[i] = cycle;
//			fprintf(res_fp, "message %d match_rule %d cycle %d\n", i, res, cycle);
//		}
//		fclose(res_fp);
//		visualize(cycleList, checkNumList, message_sets.size);

		for (int i = 0; i < CELL_SIZE_solution4; i++)free(index[i].list);
		free(index);
		free(message_sets.list);
		free(datasets.list);
	}
//	FILE *fp =  fopen("output/TotalStatistic.txt", "w");
//	fprintf(fp, "\ntotalSrcMask=0: %d\n", totalSrcMaskType[0]);
//	for (int i = 1; i < 33; i++) {
//		fprintf(fp, "%2d: %4d\t", i, totalSrcMaskType[i]);
//		if (i % 8 == 0) fprintf(fp, "\n");
//	}
//	fprintf(fp, "\n\ntotalDstMask=0: %d\n", totalDstMaskType[0]);
//	for (int i = 1; i < 33; i++) {
//		fprintf(fp, "%2d: %4d\t", i, totalDstMaskType[i]);
//		if (i % 8 == 0) fprintf(fp, "\n");
//	}
//	fclose(fp);
	printf("\n\navgTotalCycle= %f\n", totalMatchCycle / 12);
	printf("\n\navgInsertCycle= %f\n", totalInsertCycle / 12);
	return 0;
}

