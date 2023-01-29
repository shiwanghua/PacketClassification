//
// Created by swh on 22-10-4.
//

#include "ModelsTest.h"

// if shuffle rules, the result may be changed so cannot be verified (set VERIFICATION to false)
void ModelsTest::shuffleRules()
{
	rule* rp = acl_rules->list;
	RandomGenerator rg;
	for (int ri = 0; ri < acl_rules->size; ri++)
	{
		int rj = rg.rand_int(acl_rules->size - ri) + ri;
		swap(rp[ri], rp[rj]);
	}
}

void ModelsTest::shuffleMessages()
{
	message* mp = messages->list;
	RandomGenerator rg;
	for (int mi = 0; mi < messages->size; mi++)
	{
		int mj = rg.rand_int(messages->size - mi) + mi;
		swap(mp[mi], mp[mj]);
	}
}

ModelsTest::ModelsTest()
{
	acl_rules = nullptr;
	messages = nullptr;
}

ModelsTest::~ModelsTest()
{
	if (acl_rules)
	{
		delete acl_rules->list;
		delete acl_rules;
	}
	if (messages)
	{
		delete messages->list;
		delete messages;
	}
	acl_rules = nullptr;
	messages = nullptr;
}

void ModelsTest::readDatasets(int dno)
{
	if (acl_rules)
	{
		delete acl_rules->list;
		delete acl_rules;
	}
	if (messages)
	{
		delete messages->list;
		delete messages;
	}
	acl_rules = new ACL_rules;
	messages = new ACL_messages;

	read_rules_cbFormat(ruleFileName[dno], acl_rules);
	printf("read rules file (%s %d)\n", ruleFileName[dno], acl_rules->size);
#if SHUFFLERULES
	shuffleRules();
#endif

	read_messages_cbFormat(headFileName[dno], messages);
	printf("read_messages (%s %d)\n", headFileName[dno], messages->size);
#if SHUFFLEMESSAGES
	shuffleMessages();
#endif
}

void ModelsTest::generateGroundthTruthInTupleFile()
{
	double totalAvgSearchTimeUs = 0.0;
	for (int dno = 0; dno < numDataSets; dno++)
	{
		char toFilePath[200] = "gt_";
		strcat(toFilePath, headFileName[dno]);
		FILE* fmsg = fopen(toFilePath, "w");
		readDatasets(dno);
		clock_t clk = clock();
		BruteForceSearch bfs;
		message* msgb = messages->list;
		for (int i = 0; i < messages->size; i++)
		{
			unsigned int ruleNo = bfs.brute_force_search(messages->list + i, acl_rules);
			unsigned int sip =
				msgb[i].source_ip[3] * (1 << 24) + msgb[i].source_ip[2] * (1 << 16) + msgb[i].source_ip[1] * (1 << 8)
				+ msgb[i].source_ip[0];
			unsigned int dip =
				msgb[i].destination_ip[3] * (1 << 24) + msgb[i].destination_ip[2] * (1 << 16)
				+ msgb[i].destination_ip[1] * (1 << 8) + msgb[i].destination_ip[0];
			fprintf(fmsg, "%u\t%u\t%hu\t%hu\t%u\t%u\t%d\n", \
            sip, dip, msgb[i].source_port, msgb[i].destination_port, msgb[i].protocol, i, ruleNo);
		}
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;

		fclose(fmsg);
#if DEBUG
		printf("BruteForceSearch dataset %d: searchTime= %.3f us\n\n", dno + 1, avgSearchTimeUs);
#endif
	}

#if DEBUG
	printf("\n\nTotal BruteForceSearch: searchTime= %.3f us\n\n", totalAvgSearchTimeUs / numDataSets);
#endif
}

void ModelsTest::BruteForceSearch_test()
{
	double totalAvgSearchTimeUs = 0.0;

	const string file_path = "output/BruteForceSearch.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		clock_t clk = clock();
		BruteForceSearch bfs;
		for (int i = 0; i < messages->size; i++)
		{
			unsigned int ruleNo = bfs.brute_force_search(messages->list + i, acl_rules);
#if VERIFICATION
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;

#if DEBUG
		printf("BruteForceSearch dataset %d: searchTime= %.3f us\n\n", dno + 1, avgSearchTimeUs);
#endif

		content += expID + "-D" + to_string(dno + 1) + ": search= " + Utils::Double2String(avgSearchTimeUs) + " us\n";
	}

	printf("\n\nTotal BruteForceSearch: searchTime= %.3f us\n\n", totalAvgSearchTimeUs / numDataSets);

	content += expID + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets) + " us\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::HEMBS_forward_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgCycleNum = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgANDNum = 0;
	double totalAvgCMPNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/HEM_FBS.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += acl_rules->size;
		totalMessages += messages->size;

		clock_t clk = clock();
		HEMBS hem_fbs;
		hem_fbs.forward_init_bitsets_IPv4(acl_rules->size);
		double constructionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeUs += constructionTimeUs;

		clk = clock();
		for (int i = 0; i < acl_rules->size; i++)
			hem_fbs.forward_bitsets_insert_IPv4(acl_rules->list + i);
		double avgInsertionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
		totalAvgInsertionTimeUs += avgInsertionTimeUs;

		uint32_t ruleNo;
		uint64_t checkNum = 0, and64Num = 0, cmpNum = 0;
		clk = clock();
		uint64_t cycleNo = GetCPUCycle();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 3> debugInfo = hem_fbs.forward_bitsets_search_IPv4(
				messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			and64Num += debugInfo.at(1);
			cmpNum += debugInfo[2];
#else
			hem_fbs.forward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
#endif
#if VERIFICATION
			//			if (messages->list[i].rule_id == (unsigned int)-1)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			//			if (messages->list[i].rule_id == 0)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchCycleNum = (double)(GetCPUCycle() - cycleNo) / messages->size;
		totalAvgCycleNum += avgSearchCycleNum;
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgANDNum = (double)and64Num / messages->size;
		totalAvgANDNum += avgANDNum;
		double avgCMPNum = (double)cmpNum / messages->size;
		totalAvgCMPNum += avgCMPNum;
		totalAvgMemorySizeB += hem_fbs.calMemory() / acl_rules->size;

		printf("HEM-FBS dataset %d: constructT= %.3f us, insertT= %.3f us, searchT= %.3f us, cycleNum= %.3f\n"
			   "memorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgANDNum= %.3f, avgCMPNum= %.3f\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs, avgSearchCycleNum,
			hem_fbs.calMemory() / 1024.0 / 1024.0,
			hem_fbs.calMemory() / acl_rules->size, avgCheckNum, avgANDNum, avgCMPNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us cycle= " + Utils::Double2String(avgSearchCycleNum)\
 + " insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs)\
 + " us memory= " + Utils::Double2String(hem_fbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) + " AND= " + Utils::Double2String(avgANDNum) \
 + " CMP= " + Utils::Double2String(avgCMPNum) + "\n";
	}

	printf("\nExp%s HEM-FBS-a%d-CW%d: constructTime= %.3f us, insertionTime= %.3f us, searchT= %.3f us, cycle= %.3f\n"
		   "memSize= %.3f B/', checkNum= %.3f, and64Num= %.3f, cmpNum= %.3f, ruleNum= %lu, msgNum= %lu\n\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, \
        totalConstructionTimeUs / numDataSets, totalAvgInsertionTimeUs / numDataSets, \
        totalAvgSearchTimeUs / numDataSets, totalAvgCycleNum / numDataSets, totalAvgMemorySizeB / numDataSets, \
        totalAvgCheckNum / numDataSets,
		totalAvgANDNum / numDataSets, totalAvgCMPNum / numDataSets, totalRules, totalMessages);
#if DEBUG
	content += "DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES) + "-CW" + to_string(HEM_BS_PORT_CELLWIDTH)\
 + " AVG: S= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us Cyc= " + Utils::Double2String(totalAvgCycleNum / numDataSets)\
 + " I= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us CST= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us M= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' CEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " AND= " + Utils::Double2String(totalAvgANDNum / numDataSets) \
 + " CMP= " + Utils::Double2String(totalAvgCMPNum / numDataSets) + "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::HEMBS_backward_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgCycleNum = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgORNum = 0;
	double totalAvgCMPNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/HEM_BBS.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += acl_rules->size;
		totalMessages += messages->size;

		clock_t clk = clock();
		HEMBS hem_bbs;
		hem_bbs.backward_init_bitsets_IPv4(acl_rules->size);
		double constructionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeUs += constructionTimeUs;

		clk = clock();
		for (int i = 0; i < acl_rules->size; i++)
			hem_bbs.backward_bitsets_insert_IPv4(acl_rules->list + i);
		double avgInsertionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
		totalAvgInsertionTimeUs += avgInsertionTimeUs;

		uint32_t ruleNo;
		uint64_t checkNum = 0, or64Num = 0, cmpNum = 0;
		clk = clock();
		uint64_t cycleNo = GetCPUCycle();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 3> debugInfo = hem_bbs.backward_bitsets_search_IPv4(
				messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			or64Num += debugInfo.at(1);
			cmpNum += debugInfo.at(2);
#else
			hem_bbs.backward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
#endif
#if VERIFICATION
			//			if (messages->list[i].rule_id == (unsigned int)-1)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			//			if (messages->list[i].rule_id == 0)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchCycleNum = (double)(GetCPUCycle() - cycleNo) / messages->size;
		totalAvgCycleNum += avgSearchCycleNum;
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgORNum = (double)or64Num / messages->size;
		totalAvgORNum += avgORNum;
		double avgCMPNum = (double)cmpNum / messages->size;
		totalAvgCMPNum += avgCMPNum;
		totalAvgMemorySizeB += hem_bbs.calMemory() / acl_rules->size;

		printf("HEM-BBS dataset %d: constructT= %.3f us, insertT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
			   "memorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgORNum= %.3f, avgCMPNum= %.3f\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs, avgSearchCycleNum,
			hem_bbs.calMemory() / 1024.0 / 1024.0,
			hem_bbs.calMemory() / acl_rules->size, avgCheckNum, avgORNum, avgCMPNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us cycle= " + Utils::Double2String(avgSearchCycleNum)\
 + " insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs)\
 + " us memory= " + Utils::Double2String(hem_bbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) + " OR= " + Utils::Double2String(avgORNum) + " CMP= "
				   + Utils::Double2String(avgCMPNum) + "\n";
	}

	printf("\nExp%s HEM-BBS-a%d-CW%d: constructT= %.3f us, insertT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
		   "memorySize= %.3f B/' checkNum= %.3f, or64Num= %.3f, cmpNum= %.3f, ruleNum= %lu, msgNum= %lu\n\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, \
        totalConstructionTimeUs / numDataSets, totalAvgInsertionTimeUs / numDataSets, \
        totalAvgSearchTimeUs / numDataSets, totalAvgCycleNum / numDataSets, totalAvgMemorySizeB / numDataSets, \
        totalAvgCheckNum / numDataSets,
		totalAvgORNum / numDataSets, totalAvgCMPNum / numDataSets, totalRules, totalMessages);
#if DEBUG
	content += "DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES) + "-CW" + to_string(HEM_BS_PORT_CELLWIDTH)\
 + ": S= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us Cyc= " + Utils::Double2String(totalAvgCycleNum / numDataSets)\
 + " I= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us CST= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us M= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' CEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " OR= " + Utils::Double2String(totalAvgORNum / numDataSets)\
 + " CMP= " + Utils::Double2String(totalAvgCMPNum / numDataSets) + "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::HEMBS_aggregate_forward_test()
{
	double totalConstructionTimeMs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgCycleNum = 0.0;
	double totalAvgUpdateTimeUs = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgANDNum = 0;
	double totalAvgCMPNum = 0;
	double totalAvgAggBingoNum = 0;
	double totalAvgAggFailNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/HEM_AFBS.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += acl_rules->size;
		totalMessages += messages->size;

		clock_t clk = clock();
		HEMBS hem_afbs;
		hem_afbs.aggregate_forward_init_bitsets_IPv4(acl_rules->size);

		for (int i = 0; i < acl_rules->size; i++)
			hem_afbs.aggregate_forward_bitsets_insert_IPv4(acl_rules->list + i);
		double constructionTimeMs = (double)(clock() - clk) * 1000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeMs += constructionTimeMs;
		totalAvgMemorySizeB += hem_afbs.calMemory() / acl_rules->size;

		uint32_t ruleNo;
		uint64_t checkNum = 0, and64Num = 0, cmpNum = 0, aggBingo = 0, aggFail = 0;
		uint64_t cycleNo = GetCPUCycle();
		clk = clock();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 5> debugInfo = hem_afbs.aggregate_forward_bitsets_search_IPv4(
				messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			and64Num += debugInfo.at(1);
			cmpNum += debugInfo[2];
			aggBingo += debugInfo[3];
			aggFail += debugInfo[4];
#else
			hem_afbs.aggregate_forward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
#endif
#if VERIFICATION
			//			if (messages->list[i].rule_id == (unsigned int)-1)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			//			if (messages->list[i].rule_id == 0)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgSearchCycleNum = (double)(GetCPUCycle() - cycleNo) / messages->size;
		totalAvgCycleNum += avgSearchCycleNum;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgANDNum = (double)and64Num / messages->size;
		totalAvgANDNum += avgANDNum;
		double avgCMPNum = (double)cmpNum / messages->size;
		totalAvgCMPNum += avgCMPNum;
		double avgAggBingoNum = (double)aggBingo / messages->size;
		totalAvgAggBingoNum += avgAggBingoNum;
		double avgAggFailNum = (double)aggFail / messages->size;
		totalAvgAggFailNum += avgAggFailNum;

#if UPDATETEST
		int rand_update[acl_rules->size];
		RandomGenerator rg;
		for (int ra = 0; ra < acl_rules->size; ra++)
		{ //1000000
			rand_update[ra] = rg.rand_int(2); //0:insert 1:delete
		}
		clk = clock();
		for (int ra = 0; ra < acl_rules->size; ra++)
		{
			if (rand_update[ra] == 0)
			{ //0:insert
				hem_afbs.aggregate_forward_bitsets_insert_IPv4(acl_rules->list + ra);
			}
			else
			{//1:delete
				hem_afbs.aggregate_forward_bitsets_delete_IPv4(acl_rules->list + ra);
			}
		}
#else
		clk=clock();
#endif
		double avgUpdateTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
		totalAvgUpdateTimeUs += avgUpdateTimeUs;

		printf("HEM-AFBS-a%d-CW%d-k%d dataset %d: constructT= %.3f ms, searchT= %.3f us, cycleNum= %.3f, updateT= %.3f us\n"
			   "memorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgANDNum= %.3f, avgCMPNum= %.3f\n"
			   "avgAggBingo= %.3f, avgAggFail= %.3f\n\n", \
        HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, AGGREGATE_RATIO, dno + 1, \
        constructionTimeMs, avgSearchTimeUs, avgSearchCycleNum, avgUpdateTimeUs, \
            hem_afbs.calMemory() / 1024.0 / 1024.0, hem_afbs.calMemory() / acl_rules->size,
			avgCheckNum, avgANDNum, avgCMPNum, avgAggBingoNum, avgAggFailNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us cycle= " + Utils::Double2String(avgSearchCycleNum)\
 + " update= " + Utils::Double2String(avgUpdateTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeMs)\
 + " ms memory= " + Utils::Double2String(hem_afbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) + " and= " + Utils::Double2String(avgANDNum) \
 + " cmp= " + Utils::Double2String(avgCMPNum) \
 + " bingo= " + Utils::Double2String(avgAggBingoNum)\
 + " fail= " + Utils::Double2String(avgAggFailNum) + "\n";
	}

	printf("\nExp%s HEM-AFBS-a%d-CW%d-k%d: constructT= %.3f ms, updateT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
		   "checkNum= %.3f, and64Num= %.3f, cmpNum= %.3f, bingo= %.3f, fail= %.3f\n"
		   "memorySize= %.3f B/' ruleNum= %lu, msgNum= %lu\n\n\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, AGGREGATE_RATIO,
		totalConstructionTimeMs / numDataSets, totalAvgUpdateTimeUs / numDataSets,
		totalAvgSearchTimeUs / numDataSets, totalAvgCycleNum / numDataSets, \
        totalAvgCheckNum / numDataSets, totalAvgANDNum / numDataSets, totalAvgCMPNum / numDataSets, \
        totalAvgAggBingoNum / numDataSets, totalAvgAggFailNum / numDataSets, \
        totalAvgMemorySizeB / numDataSets, totalRules, totalMessages);
#if DEBUG
	content += "DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES) + "-CW" + to_string(HEM_BS_PORT_CELLWIDTH) + "-k"
			   + to_string(AGGREGATE_RATIO)\
 + " AVG: S= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us Cyc= " + Utils::Double2String(totalAvgCycleNum / numDataSets)\
 + " Udt= " + Utils::Double2String(totalAvgUpdateTimeUs / numDataSets)\
 + " us CST= " + Utils::Double2String(totalConstructionTimeMs / numDataSets)\
 + " ms M= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' CEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " AND= " + Utils::Double2String(totalAvgANDNum / numDataSets) \
 + " CMP= " + Utils::Double2String(totalAvgCMPNum / numDataSets) \
 + " Bingo= " + Utils::Double2String(totalAvgAggBingoNum / numDataSets)\
 + " Fail= " + Utils::Double2String(totalAvgAggFailNum / numDataSets) + "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::HEMBS_RLE_forward_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgCycleNum = 0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgPlusOneNum = 0;
	double totalAvgCMPNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/HEM_RFBS.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += acl_rules->size;
		totalMessages += messages->size;

		clock_t clk = clock();
		HEMBS hem_rfbs;
		hem_rfbs.RLE_forward_init_bitsets_IPv4(acl_rules->size);
		double constructionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;

		clk = clock();
		for (int i = 0; i < acl_rules->size; i++)
			hem_rfbs.RLE_forward_bitsets_insert_IPv4(acl_rules->list + i);
		double avgInsertionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
		totalAvgInsertionTimeUs += avgInsertionTimeUs;

		clk = clock();
		hem_rfbs.RLE_forward_construction_IPv4();
		constructionTimeUs += (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeUs += constructionTimeUs;

		uint32_t ruleNo;
		uint64_t checkNum = 0, plusOneNum = 0, cmpNum = 0;
		clk = clock();
		uint64_t cycleNo = GetCPUCycle();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 3> debugInfo = hem_rfbs.RLE_forward_bitsets_search_IPv4(
				messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			plusOneNum += debugInfo.at(1);
			cmpNum += debugInfo[2];
#else
			hem_rfbs.RLE_forward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
#endif
#if VERIFICATION
			//			if (messages->list[i].rule_id == (unsigned int)-1)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			//			if (messages->list[i].rule_id == 0)
			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchCycleNum = (double)(GetCPUCycle() - cycleNo) / messages->size;
		totalAvgCycleNum += avgSearchCycleNum;
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgPlusOneNum = (double)plusOneNum / messages->size;
		totalAvgPlusOneNum += avgPlusOneNum;
		double avgCMPNum = (double)cmpNum / messages->size;
		totalAvgCMPNum += avgCMPNum;
		totalAvgMemorySizeB += hem_rfbs.calMemory() / acl_rules->size;

		printf("HEM-RFBS-Incre%d dataset %d: constructT= %.3f us, insertT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
			   "memorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgPlus1Num= %.3f, avgCMPNum= %.3f\n\n\n", \
        INCREMENT, dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs, avgSearchCycleNum,
			hem_rfbs.calMemory() / 1024.0 / 1024.0, hem_rfbs.calMemory() / acl_rules->size,
			avgCheckNum, avgPlusOneNum, avgCMPNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us cycle=  " + Utils::Double2String(avgSearchCycleNum) \
 + " insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs)\
 + " us memory= " + Utils::Double2String(hem_rfbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) \
 + " plus1= " + Utils::Double2String(avgPlusOneNum) \
 + " cmp= " + Utils::Double2String(avgCMPNum) + "\n";
	}

	printf("\nExp%s HEM-RFBS-a%d-CW%d-Incre%d: constructT= %.3f us, insertT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
		   "checkNum= %.3f, plus1Num= %.3f, cmpNum= %.3f, memorySize= %.3f B/' ruleNum= %lu, msgNum= %lu\n\n\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, INCREMENT,
		totalConstructionTimeUs / numDataSets, totalAvgInsertionTimeUs / numDataSets,
		totalAvgSearchTimeUs / numDataSets, totalAvgCycleNum / numDataSets, totalAvgCheckNum / numDataSets,
		totalAvgPlusOneNum / numDataSets, totalAvgCMPNum / numDataSets, \
        totalAvgMemorySizeB / numDataSets, totalRules, totalMessages);
#if DEBUG
	content += "DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES) + "-CW" + to_string(HEM_BS_PORT_CELLWIDTH) + "-Incre" + to_string(INCREMENT)\
 + " AVG: S= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us Cyc= " + Utils::Double2String(totalAvgCycleNum / numDataSets)\
 + " I= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us CST= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us M= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' CEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " AND= " + Utils::Double2String(totalAvgPlusOneNum / numDataSets) \
 + " CMP= " + Utils::Double2String(totalAvgCMPNum / numDataSets) + "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::TamaSearch_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgRuleSizes = 0;
	double totalAvgMinusNum = 0;
	double totalAvgCmpNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/TamaSearch.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += acl_rules->size;
		totalMessages += messages->size;

		clock_t clk = clock();
		TamaSearch ts;
		ts.initize(acl_rules->size);
		double constructionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeUs += constructionTimeUs;

		clk = clock();
		for (int i = 0; i < acl_rules->size; i++)
		{
			ts.insert(acl_rules->list + i);
		}
		double avgInsertionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
		totalAvgInsertionTimeUs += avgInsertionTimeUs;
		printf("TamaSearch Construction and Insertion Finish.\n");

		clk = clock();
		for (int i = 0; i < messages->size; i++)
		{
			unsigned int ruleNo = ts.search(messages->list[i], acl_rules);
#if VERIFICATION
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Tama Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
				exit(0);
			}
#endif
		}
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		totalAvgMemorySizeB += ts.calMemory() / acl_rules->size;

#if DEBUG
		totalAvgMinusNum += ts.totalMinusNum / messages->size;
		totalAvgCmpNum += ts.totalCmpNum / messages->size;
		totalAvgRuleSizes += ts.statistics();
		printf("TamaSearch dataset %d: constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us\nmemorySize= %.3f MB, avgMemorySize= %.3f B/', "
			   "avgMinusNum= %.2f, avgCmpNum= %.2f\n\n", \
		dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs,
			ts.calMemory() / 1024.0 / 1024.0, ts.calMemory() / acl_rules->size,
			(double)ts.totalMinusNum / messages->size, (double)ts.totalCmpNum / messages->size);
#else
		printf("TamaSearch dataset %d: constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us\n"
			   "memorySize= %.3f MB, avgMemorySize= %.3f B/'\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs,
			ts.calMemory() / 1024.0 / 1024.0, ts.calMemory() / acl_rules->size);
#endif

		content += expID + "-D" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs / 1000000.0)\
 + " s memory= " + Utils::Double2String(ts.calMemory() / 1024.0 / 1024.0) + " MB";
#if DEBUG
		content += ", avgMinusNum= " + Utils::Double2String((double)ts.totalMinusNum / messages->size);
		content + ", avgCmpNum= " + Utils::Double2String((double)ts.totalCmpNum / messages->size);
#endif
		content += "\n";
	}

	printf("\n\nTotal TamaSearch: constructionTime= %.3f s, insertionTime= %.3f us, searchTime= %.3f us\n"
		   "avgMemorySize= %.3f B/', avgRuleSize= %.2f, totalRules= %lu, totalMsg= %lu,  \n"
		   "avgMinusNum= %.2f, avgCmpNum= %.2f\n\n", \
        totalConstructionTimeUs / 1000000.0 / numDataSets,
		totalAvgInsertionTimeUs / numDataSets, totalAvgSearchTimeUs / numDataSets, \
        totalAvgMemorySizeB / numDataSets,
		totalAvgRuleSizes / numDataSets, totalRules, totalMessages,
		totalAvgMinusNum / numDataSets, totalAvgCmpNum / numDataSets);

	content += expID + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us avgI= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us avgC= " + Utils::Double2String(totalConstructionTimeUs / 1000000.0 / numDataSets)\
 + " s avgM= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets) + " B/'";
#if DEBUG
	content += ", avgMinusNum= " + Utils::Double2String(totalAvgMinusNum / numDataSets);
#if TAMA_PRIORITY_CHECK
	content+=", avgCmpNum= "+Utils::Double2String(totalAvgCmpNum / numDataSets);
#endif
#endif
	content += "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::PTTree_test(){ // 未实现！
	double totalConstructionTimeMs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgCycleNum = 0.0;
	double totalAvgUpdateTimeUs = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgANDNum = 0;
	double totalAvgCMPNum = 0;
	double totalAvgAggBingoNum = 0;
	double totalAvgAggFailNum = 0;
	uint64_t totalRules = 0;
	uint64_t totalMessages = 0;

	const string file_path = "output/PTTree.txt";
	string content;

//	for (int dno = 0; dno < numDataSets; dno++)
//	{
//		readDatasets(dno);
//		totalRules += acl_rules->size;
//		totalMessages += messages->size;
//
//		clock_t clk = clock();
//		PTtree pttree;
//		pttree.aggregate_forward_init_bitsets_IPv4(acl_rules->size);
//
//		for (int i = 0; i < acl_rules->size; i++)
//			pttree.aggregate_forward_bitsets_insert_IPv4(acl_rules->list + i);
//		double constructionTimeMs = (double)(clock() - clk) * 1000.0 / CLOCKS_PER_SEC;
//		totalConstructionTimeMs += constructionTimeMs;
//		totalAvgMemorySizeB += pttree.calMemory() / acl_rules->size;
//
//		uint32_t ruleNo;
//		uint64_t checkNum = 0, and64Num = 0, cmpNum = 0, aggBingo = 0, aggFail = 0;
//		clk = clock();
//		uint64_t cycleNo = GetCPUCycle();
//		for (int i = 0; i < messages->size; i++)
//		{
//#if DEBUG
//			std::array<uint64_t, 5> debugInfo = pttree.aggregate_forward_bitsets_search_IPv4(
//				messages->list + i, acl_rules->list, ruleNo);
//			checkNum += debugInfo[0];
//			and64Num += debugInfo.at(1);
//			cmpNum += debugInfo[2];
//			aggBingo += debugInfo[3];
//			aggFail += debugInfo[4];
//#else
//			pttree.aggregate_forward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
//#endif
//#if VERIFICATION
//			//			if (messages->list[i].rule_id == (unsigned int)-1)
//			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
//			//			if (messages->list[i].rule_id == 0)
//			//				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
//			if (messages->list[i].rule_id != ruleNo)
//			{
//				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
//				exit(0);
//			}
//#endif
//		}
//		double avgSearchCycleNum = (double)(GetCPUCycle() - cycleNo) / messages->size;
//		totalAvgCycleNum += avgSearchCycleNum;
//		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
//		totalAvgSearchTimeUs += avgSearchTimeUs;
//		double avgCheckNum = (double)checkNum / messages->size;
//		totalAvgCheckNum += avgCheckNum;
//		double avgANDNum = (double)and64Num / messages->size;
//		totalAvgANDNum += avgANDNum;
//		double avgCMPNum = (double)cmpNum / messages->size;
//		totalAvgCMPNum += avgCMPNum;
//		double avgAggBingoNum = (double)aggBingo / messages->size;
//		totalAvgAggBingoNum += avgAggBingoNum;
//		double avgAggFailNum = (double)aggFail / messages->size;
//		totalAvgAggFailNum += avgAggFailNum;
//
////		int rand_update[acl_rules->size];
////		RandomGenerator rg;
////		for (int ra = 0; ra < acl_rules->size; ra++)
////		{ //1000000
////			rand_update[ra] = rg.rand_int(2); //0:insert 1:delete
////		}
////		clk = clock();
////		for (int ra = 0; ra < acl_rules->size; ra++)
////		{
////			if (rand_update[ra] == 0)
////			{ //0:insert
////				pttree.aggregate_forward_bitsets_insert_IPv4(acl_rules->list + ra);
////			}
////			else
////			{//1:delete
////				pttree.aggregate_forward_bitsets_delete_IPv4(acl_rules->list + ra);
////			}
////		}
//		double avgUpdateTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / acl_rules->size;
//		totalAvgUpdateTimeUs += avgUpdateTimeUs;
//
//		printf("HEM-AFBS-a%d-CW%d-k%d dataset %d: constructT= %.3f ms, searchT= %.3f us, cycleNum= %.3f, updateT= %.3f us\n"
//			   "memorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgANDNum= %.3f, avgCMPNum= %.3f\n"
//			   "avgAggBingo= %.3f, avgAggFail= %.3f\n\n", \
//        HEM_BS_NUM_ATTR, HEM_BS_PORT_CELLWIDTH, AGGREGATE_RATIO, dno + 1, \
//        constructionTimeMs, avgSearchTimeUs, avgSearchCycleNum, avgUpdateTimeUs, \
//            pttree.calMemory() / 1024.0 / 1024.0, pttree.calMemory() / acl_rules->size,
//			avgCheckNum, avgANDNum, avgCMPNum, avgAggBingoNum, avgAggFailNum);
//
//		content += expID + "-d" + to_string(dno + 1) \
// + ": search= " + Utils::Double2String(avgSearchTimeUs)\
// + " us cycle= " + Utils::Double2String(avgSearchCycleNum)\
// + " update= " + Utils::Double2String(avgUpdateTimeUs)\
// + " us construct= " + Utils::Double2String(constructionTimeMs)\
// + " ms memory= " + Utils::Double2String(pttree.calMemory() / 1024.0 / 1024.0) \
// + " MB check= " + Utils::Double2String(avgCheckNum) + " and= " + Utils::Double2String(avgANDNum) \
// + " cmp= " + Utils::Double2String(avgCMPNum) \
// + " bingo= " + Utils::Double2String(avgAggBingoNum)\
// + " fail= " + Utils::Double2String(avgAggFailNum) + "\n";
//	}
//
//	printf("\nExp%s HEM-AFBS-k%d-a%d: constructT= %.3f ms, updateT= %.3f us, searchT= %.3f us, cycle= %.3f\n"
//		   "checkNum= %.3f, and64Num= %.3f, cmpNum= %.3f, bingo= %.3f, fail= %.3f\n"
//		   "memorySize= %.3f B/' ruleNum= %lu, msgNum= %lu\n\n\n\n", \
//        expID.c_str(), AGGREGATE_RATIO, HEM_BS_NUM_ATTR,
//		totalConstructionTimeMs / numDataSets, totalAvgUpdateTimeUs / numDataSets,
//		totalAvgSearchTimeUs / numDataSets, totalAvgCycleNum / numDataSets, \
//        totalAvgCheckNum / numDataSets, totalAvgANDNum / numDataSets, totalAvgCMPNum / numDataSets, \
//        totalAvgAggBingoNum / numDataSets, totalAvgAggFailNum / numDataSets, \
//        totalAvgMemorySizeB / numDataSets, totalRules, totalMessages);
//#if DEBUG
//	content += "DEBUG";
//#endif
//	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
//			   + to_string(SHUFFLEMESSAGES) + "-CW" + to_string(HEM_BS_PORT_CELLWIDTH) + "-k"
//			   + to_string(AGGREGATE_RATIO)\
// + " AVG: S= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
// + " us Cyc= " + Utils::Double2String(totalAvgCycleNum / numDataSets)\
// + " Udt= " + Utils::Double2String(totalAvgUpdateTimeUs / numDataSets)\
// + " us CST= " + Utils::Double2String(totalConstructionTimeMs / numDataSets)\
// + " ms M= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
// + " B/' CEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
// + " AND= " + Utils::Double2String(totalAvgANDNum / numDataSets) \
// + " CMP= " + Utils::Double2String(totalAvgCMPNum / numDataSets) \
// + " Bingo= " + Utils::Double2String(totalAvgAggBingoNum / numDataSets)\
// + " Fail= " + Utils::Double2String(totalAvgAggFailNum / numDataSets) + "\n";
//	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::IP_visualization()
{
	string srcPercentStr = "srcMaskPercent=[\n";
	string dstPercentStr = "dstMaskPercent=[\n";
	string srcVarianceStr = "srcMaskVariance=[";
	string dstVarianceStr = "dstMaskVariance=[";
	for (uint32_t dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		double srcMaskNum[33], dstMaskNum[33];
		memset(srcMaskNum, 0, 33 * sizeof(double));
		memset(dstMaskNum, 0, 33 * sizeof(double));

		for (uint32_t i = 0; i < acl_rules->size; i++)
		{
			const rule& r = acl_rules->list[i];
			srcMaskNum[(uint32_t)r.source_mask]++;
			dstMaskNum[(uint32_t)r.destination_mask]++;
		}
		srcVarianceStr += Utils::Double2String(Utils::CalVariance(srcMaskNum, 33));
		dstVarianceStr += Utils::Double2String(Utils::CalVariance(dstMaskNum, 33));

		srcPercentStr.append("  [");
		dstPercentStr.append("  [");
		for (uint32_t m = 0; m < 32; m++)
		{
			srcMaskNum[m] /= (double)acl_rules->size;
			dstMaskNum[m] /= (double)acl_rules->size;
			srcPercentStr += Utils::Double2String(srcMaskNum[m]) + ", ";
			dstPercentStr += Utils::Double2String(dstMaskNum[m]) + ", ";
		}
		srcMaskNum[32] /= (double)acl_rules->size;
		dstMaskNum[32] /= (double)acl_rules->size;
		srcPercentStr += Utils::Double2String(srcMaskNum[32]) + "]";
		dstPercentStr += Utils::Double2String(dstMaskNum[32]) + "]";

		if (dno + 1 != numDataSets)
		{
			srcPercentStr.append(",");
			dstPercentStr.append(",");
			srcVarianceStr.append(",");
			dstVarianceStr.append(",");
		}
		srcPercentStr.append("\n");
		dstPercentStr.append("\n");
	}
	srcPercentStr.append("]\n");
	dstPercentStr.append("]\n");
	srcVarianceStr.append("]\n");
	dstVarianceStr.append("]\n");
	std::cout << srcPercentStr << "\n" << srcVarianceStr << "\n\n" << dstPercentStr << "\n" << dstVarianceStr << "\n";
}

void ModelsTest::Protocol_visualization()
{
	string protocolStr = "protocolPercent=[\n";
	string protocolVarianceStr = "protocolVariance=[";
	for (uint32_t dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		double protocolNum[NUM_PROTOCOL + 1];
		memset(protocolNum, 0, (NUM_PROTOCOL + 1) * sizeof(double));

		for (uint32_t i = 0; i < acl_rules->size; i++)
		{
			const rule& r = acl_rules->list[i];
			if ((unsigned int)r.protocol[1] == 0)
				protocolNum[0]++;
			else
			{
				switch ((unsigned int)r.protocol[1])
				{
				case ICMP:
					protocolNum[1]++;
					break;
				case IGMP:
					protocolNum[2]++;
					break;
				case GGP:
					protocolNum[3]++;
					break;
				case IP:
					protocolNum[4]++;
					break;
				case ST:
					protocolNum[5]++;
					break;
				case TCP:
					protocolNum[6]++;
					break;
				case CBT:
					protocolNum[7]++;
					break;
				case EGP:
					protocolNum[8]++;
					break;
				case UDP:
					protocolNum[9]++;
					break;
				case RSVP:
					protocolNum[10]++;
					break;
				case GRE:
					protocolNum[11]++;
					break;
				case ESP:
					protocolNum[12]++;
					break;
				case AH:
					protocolNum[13]++;
					break;
				case EIGRP:
					protocolNum[14]++;
					break;
				case OSPFIGP:
					protocolNum[15]++;
					break;
				case ISIS:
					protocolNum[16]++;
					break;
				default:
					fprintf(stderr, "Rule %d Error - unknown rule protocol %u !\n", r.PRI, r.protocol[1]);
					exit(0);
				}
			}
		}

		protocolVarianceStr += Utils::Double2String(Utils::CalVariance(protocolNum, NUM_PROTOCOL + 1));

		protocolStr.append("  [");
		for (uint32_t m = 0; m < NUM_PROTOCOL; m++)
		{
			protocolNum[m] /= (double)acl_rules->size;
			protocolStr += Utils::Double2String(protocolNum[m]) + ", ";
		}
		protocolNum[NUM_PROTOCOL] /= (double)acl_rules->size;
		protocolStr += Utils::Double2String(protocolNum[NUM_PROTOCOL]) + "]";

		if (dno + 1 != numDataSets)
		{
			protocolStr.append(",");
			protocolVarianceStr.append(",");
		}
		protocolStr.append("\n");
	}
	protocolStr.append("]\n");
	protocolVarianceStr.append("]\n");
	std::cout << protocolStr << "\n" << protocolVarianceStr << "\n\n";
}

void ModelsTest::Port_subspace_visualization()
{
	string srcPercentStr = "srcPortSubspacePercent=[\n";
	string dstPercentStr = "dstPortSubspacePercent=[\n";
	string srcVarianceStr = "srcPortSubspaceVariance=[";
	string dstVarianceStr = "dstPortSubspaceVariance=[";
	for (uint32_t dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		double srcPortCellNum[HEM_BS_NUM_PORT_BITSET], dstPortCellNum[HEM_BS_NUM_PORT_BITSET];
		memset(srcPortCellNum, 0, HEM_BS_NUM_PORT_BITSET * sizeof(double));
		memset(dstPortCellNum, 0, HEM_BS_NUM_PORT_BITSET * sizeof(double));

		for (uint32_t i = 0; i < acl_rules->size; i++)
		{
			const rule& r = acl_rules->list[i];
			for (uint32_t ci = (uint32_t)r.source_port[0] / HEM_BS_PORT_CELLWIDTH;
				 ci <= (uint32_t)r.source_port[1] / HEM_BS_PORT_CELLWIDTH; ci++)
				srcPortCellNum[ci]++;
			for (uint32_t ci = (uint32_t)r.destination_port[0] / HEM_BS_PORT_CELLWIDTH;
				 ci <= (uint32_t)r.destination_port[1] / HEM_BS_PORT_CELLWIDTH; ci++)
				dstPortCellNum[ci]++;
		}
		srcVarianceStr += Utils::Double2String(Utils::CalVariance(srcPortCellNum, HEM_BS_NUM_PORT_BITSET));
		dstVarianceStr += Utils::Double2String(Utils::CalVariance(dstPortCellNum, HEM_BS_NUM_PORT_BITSET));

		srcPercentStr.append("  [");
		dstPercentStr.append("  [");
		for (uint32_t m = 0; m < HEM_BS_NUM_PORT_BITSET - 1; m++)
		{
			srcPortCellNum[m] /= (double)acl_rules->size;
			dstPortCellNum[m] /= (double)acl_rules->size;
			srcPercentStr += Utils::Double2String(srcPortCellNum[m]) + ", ";
			dstPercentStr += Utils::Double2String(dstPortCellNum[m]) + ", ";
		}
		srcPortCellNum[HEM_BS_NUM_PORT_BITSET - 1] /= (double)acl_rules->size;
		dstPortCellNum[HEM_BS_NUM_PORT_BITSET - 1] /= (double)acl_rules->size;
		srcPercentStr += Utils::Double2String(srcPortCellNum[HEM_BS_NUM_PORT_BITSET - 1]) + "]";
		dstPercentStr += Utils::Double2String(dstPortCellNum[HEM_BS_NUM_PORT_BITSET - 1]) + "]";

		if (dno + 1 != numDataSets)
		{
			srcPercentStr.append(",");
			dstPercentStr.append(",");
			srcVarianceStr.append(",");
			dstVarianceStr.append(",");
		}
		srcPercentStr.append("\n");
		dstPercentStr.append("\n");
	}
	srcPercentStr.append("]\n");
	dstPercentStr.append("]\n");
	srcVarianceStr.append("]\n");
	dstVarianceStr.append("]\n");
	std::cout << srcPercentStr << "\n" << srcVarianceStr << "\n\n" << dstPercentStr << "\n" << dstVarianceStr << "\n";
}

void ModelsTest::Port_width_visualization()
{
	string srcPercentStr = "srcPortWidthPercent=[\n";
	string dstPercentStr = "dstPortWidthPercent=[\n";
	string srcVarianceStr = "srcPortWidthVariance=[";
	string dstVarianceStr = "dstPortWidthVariance=[";
	string srcW0Str = "srcPortW0Per=[";
	string dstW0Str = "dstPortW0Per=[";
	string srcW1Str = "srcPortW1Per=[";
	string dstW1Str = "dstPortW1Per=[";

	for (uint32_t dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		double srcPortWidthNum[HEM_BS_NUM_PORT_BITSET], dstPortWidthNum[HEM_BS_NUM_PORT_BITSET];
		memset(srcPortWidthNum, 0, HEM_BS_NUM_PORT_BITSET * sizeof(double));
		memset(dstPortWidthNum, 0, HEM_BS_NUM_PORT_BITSET * sizeof(double));
		double srcw0 = 0, srcw1 = 0, dstw0 = 0, dstw1 = 0;

		for (uint32_t i = 0; i < acl_rules->size; i++)
		{
			const rule& r = acl_rules->list[i];
			srcPortWidthNum[((uint32_t)r.source_port[1] - (uint32_t)r.source_port[0]) / HEM_BS_PORT_CELLWIDTH]++;
			dstPortWidthNum[((uint32_t)r.destination_port[1] - (uint32_t)r.destination_port[0])
							/ HEM_BS_PORT_CELLWIDTH]++;
			if (r.source_port[0] == r.source_port[1]) srcw0++;
			else if (r.source_port[0] == 0 && r.source_port[1] == 65535) srcw1++;
			if (r.destination_port[0] == r.destination_port[1]) dstw0++;
			else if (r.destination_port[0] == 0 && r.destination_port[1] == 65535) dstw1++;
		}
		srcVarianceStr += Utils::Double2String(Utils::CalVariance(srcPortWidthNum, HEM_BS_NUM_PORT_BITSET));
		dstVarianceStr += Utils::Double2String(Utils::CalVariance(dstPortWidthNum, HEM_BS_NUM_PORT_BITSET));
		srcW0Str += Utils::Double2String(srcw0 / acl_rules->size);
		srcW1Str += Utils::Double2String(srcw1 / acl_rules->size);
		dstW0Str += Utils::Double2String(dstw0 / acl_rules->size);
		dstW1Str += Utils::Double2String(dstw1 / acl_rules->size);

		srcPercentStr.append("  [");
		dstPercentStr.append("  [");
		for (uint32_t m = 0; m < HEM_BS_NUM_PORT_BITSET - 1; m++)
		{
			srcPortWidthNum[m] /= (double)acl_rules->size;
			dstPortWidthNum[m] /= (double)acl_rules->size;
			srcPercentStr += Utils::Double2String(srcPortWidthNum[m]) + ", ";
			dstPercentStr += Utils::Double2String(dstPortWidthNum[m]) + ", ";
		}
		srcPortWidthNum[HEM_BS_NUM_PORT_BITSET - 1] /= (double)acl_rules->size;
		dstPortWidthNum[HEM_BS_NUM_PORT_BITSET - 1] /= (double)acl_rules->size;
		srcPercentStr += Utils::Double2String(srcPortWidthNum[HEM_BS_NUM_PORT_BITSET - 1]) + "]";
		dstPercentStr += Utils::Double2String(dstPortWidthNum[HEM_BS_NUM_PORT_BITSET - 1]) + "]";

		if (dno + 1 != numDataSets)
		{
			srcPercentStr.append(",");
			dstPercentStr.append(",");
			srcVarianceStr.append(",");
			dstVarianceStr.append(",");
			srcW0Str.append(",");
			srcW1Str.append(",");
			dstW0Str.append(",");
			dstW1Str.append(",");
		}
		srcPercentStr.append("\n");
		dstPercentStr.append("\n");
	}
	srcPercentStr.append("]\n");
	dstPercentStr.append("]\n");
	srcVarianceStr.append("]\n");
	dstVarianceStr.append("]\n");
	srcW0Str.append("]\n");
	srcW1Str.append("]\n");
	dstW0Str.append("]\n");
	dstW1Str.append("]\n");
	std::cout << srcPercentStr << "\n" << srcVarianceStr << "\n\n" << dstPercentStr << "\n" << dstVarianceStr << "\n\n";
	std::cout << srcW0Str << "\n" << srcW1Str << "\n" << dstW0Str << "\n" << dstW1Str << "\n";
}

void ModelsTest::FBS_bitsets_visualization()
{
	string outStr = "aggregatePercent=[\n";
	for (int dno = 0; dno < numDataSets; dno++)
	{
		outStr.append("\t");
		readDatasets(dno);

		HEMBS hem_fbs;
		hem_fbs.forward_init_bitsets_IPv4(acl_rules->size);

		for (int i = 0; i < acl_rules->size; i++)
			hem_fbs.forward_bitsets_insert_IPv4(acl_rules->list + i);

		hem_fbs.forward_bitsets_visualization(outStr);
		if (dno + 1 != numDataSets)
			outStr.append(",\n");
	}
	std::cout << outStr << "\n]\n";
}
