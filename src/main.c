#include"core.h"
#include"read.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

int test1() {
	unsigned int a=32;
	printf("%d\n",a>>32);
	printf("%d\n",16-__builtin_ctz(65-1));
	printf("%d\n",16-__builtin_ctz(257-1));
//	unsigned char c[4];
//	unsigned int a=66;
//	c[0]=(unsigned char)a;
//	c[1]=0;
//	c[2]=0;
//	c[3]=0;
//	unsigned int s;
//	memcpy(&s, c, 4);
//	printf("%d\n",(unsigned int)c[0]);
//	printf("%d\n",s);
}

int main() {
//    test1();
	char ruleFileName[12][50] = {"acl1_256k.txt", "acl2_256k.txt", "acl3_256k.txt", "acl4_256k.txt", "acl5_256k.txt",
								 "fw1_256k.txt", "fw2_256k.txt", "fw3_256k.txt", "fw4_256k.txt", "fw5_256k.txt",
								 "ipc1_256k.txt", "ipc2_256k.txt"};
	char headFileName[12][50] = {"acl1_256k_trace.txt", "acl2_256k_trace.txt", "acl3_256k_trace.txt",
								 "acl4_256k_trace.txt", "acl5_256k_trace.txt", "fw1_256k_trace.txt",
								 "fw2_256k_trace.txt", "fw3_256k_trace.txt", "fw4_256k_trace.txt", "fw5_256k_trace.txt",
								 "ipc1_256k_trace.txt", "ipc2_256k_trace.txt"};
	double totalCycle=0;
	for (int q = 0; q < 12; q++) {
		ACL_rules datasets = {0, 0, 0};
		ACL_messages message_sets = {0, 0, 0};
		Cell *index = (Cell *) calloc(CELL_SIZE_solution3, sizeof(Cell));

		char tmpFileName[100]="classbench_256k/";
		strcat(tmpFileName,ruleFileName[q]);
		read_rules(tmpFileName, &datasets);

		strcpy(tmpFileName,"classbench_256k/");
		strcat(tmpFileName, headFileName[q]);
		read_messages(tmpFileName, &message_sets);

		for (int i = 0; i < datasets.size; i++)
			insert_solution3(index, datasets.list + i);

		printf("%s\n%d %d %d n_rule=%d, n_head=%d\n",ruleFileName[q], sizeof(data), sizeof(rule), sizeof(Cell),datasets.size, message_sets.size);
		printf("MemoryUse: %f MB\n", get_memory(index));

		strcpy(tmpFileName,"output/rule_distribution_");
		strcat(tmpFileName, ruleFileName[q]);
		analyse_log2(&datasets,tmpFileName);

		strcpy(tmpFileName,"output/CELL_SIZE_");
		strcat(tmpFileName, ruleFileName[q]);
		get_cell_size(index,tmpFileName,CELL_SIZE_solution3);

		int res = 0;
		double cycle = 0;
		int checkNum = 0;
		int cycleList[message_sets.size];
		int checkNumList[message_sets.size];
		FILE *res_fp;
		strcpy(tmpFileName,"output/match_cycle_");
		strcat(tmpFileName, ruleFileName[q]);
		res_fp = fopen(tmpFileName, "w");

		uint64_t time_1, time_2;
		time_1 = GetCPUCycle();
		struct timeval starttime,endtime;
		gettimeofday(&starttime,0);
//		clock_t start = clock();
		for (int i = 0; i < message_sets.size; i++) {
			res = match_with_log_solution3(index, &message_sets.list[i], &cycle);
//		res = match_with_log2_solution1(index, &message_sets.list[i], &cycle,&checkNum);
//			cycleList[i] = cycle;
//		checkNumList[i]=checkNum;
//			fprintf(res_fp, "message %d match_rule %d cycle %d\n", i, res, cycle);
		}
		time_2 = GetCPUCycle();
		cycle = (double)(time_2 - time_1)/message_sets.size;
		totalCycle+=cycle;
		printf("avgCycle= %f\n\n\n", cycle);
		gettimeofday(&endtime,0);
		double timeuse = 1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec;
		printf("avgMatchTime= %fus\n\n\n",timeuse/message_sets.size);
//		clock_t stop = clock();
//		double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
//		printf("Time elapsed in ms: %f\n\n\n", elapsed);
		fclose(res_fp);

//		visualize(cycleList, checkNumList, message_sets.size);

		// simple_match
//		printf("simple_match %s\n",ruleFileName[q]);
//		strcpy(tmpFileName,"output/simple_match_cycle_");
//		strcat(tmpFileName, ruleFileName[q]);
//		res_fp = fopen(tmpFileName, "w");
//		for (int i = 0; i < message_sets.size; i++) {
//			res = simple_match(&datasets, &message_sets.list[i], &cycle);
//			cycleList[i] = cycle;
//			fprintf(res_fp, "message %d match_rule %d cycle %d\n", i, res, cycle);
//		}
//		fclose(res_fp);
//		visualize(cycleList, checkNumList, message_sets.size);

		for (int i = 0; i < CELL_SIZE_solution3; i++)free(index[i].list);
		free(index);
		free(message_sets.list);
		free(datasets.list);
	}
	printf("\n\navgTotalCycle= %f\n",totalCycle/12);
	return 0;
}

