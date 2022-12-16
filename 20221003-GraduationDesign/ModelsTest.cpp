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
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgANDNum = 0;
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
		uint64_t checkNum = 0, and64Num = 0;
		clk = clock();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 2> debugInfo = hem_fbs.forward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			and64Num += debugInfo.at(1);
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
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgANDNum = (double)and64Num / messages->size;
		totalAvgANDNum += avgANDNum;
		totalAvgMemorySizeB += hem_fbs.calMemory() / acl_rules->size;

		printf("HEM-FBS dataset %d: constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us\nmemorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgANDNum= %.3f\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs,
			hem_fbs.calMemory() / 1024.0 / 1024.0, hem_fbs.calMemory() / acl_rules->size, avgCheckNum, avgANDNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs)\
 + " us memory= " + Utils::Double2String(hem_fbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) + " AND= "+Utils::Double2String(avgANDNum)+ "\n";
	}

	printf("\n\nExp%s HEM-FBS-a%d: constructTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us, "
		   "memorySize= %.3f B/'\ncheckNum= %.3f, and64Num= %.3f, ruleNum= %lu, msgNum= %lu\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, totalConstructionTimeUs / numDataSets, totalAvgInsertionTimeUs / numDataSets, \
        totalAvgSearchTimeUs / numDataSets, totalAvgMemorySizeB / numDataSets, \
        totalAvgCheckNum / numDataSets, totalAvgANDNum / numDataSets, totalRules, totalMessages);
#if DEBUG
	content+="DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES)\
 + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us avgI= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us avgCST= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us avgM= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' avgCEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " avgOR= " + Utils::Double2String(totalAvgANDNum / numDataSets) + "\n";
	Utils::WriteData2Begin(file_path, content);
}

void ModelsTest::HEMBS_backward_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalAvgMemorySizeB = 0.0;
	double totalAvgCheckNum = 0;
	double totalAvgORNum = 0;
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
		uint64_t checkNum = 0, or64Num = 0;
		clk = clock();
		for (int i = 0; i < messages->size; i++)
		{
#if DEBUG
			std::array<uint64_t, 2> debugInfo = hem_bbs.backward_bitsets_search_IPv4(messages->list + i, acl_rules->list, ruleNo);
			checkNum += debugInfo[0];
			or64Num += debugInfo.at(1);
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
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		double avgCheckNum = (double)checkNum / messages->size;
		totalAvgCheckNum += avgCheckNum;
		double avgORNum = (double)or64Num / messages->size;
		totalAvgORNum += avgORNum;
		totalAvgMemorySizeB += hem_bbs.calMemory() / acl_rules->size;

		printf("HEM-BBS dataset %d: constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us\nmemorySize= %.3f MB, avgMemorySize= %.3f B/', avgCheckNum= %.3f, avgORNum= %.3f\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs,
			hem_bbs.calMemory() / 1024.0 / 1024.0, hem_bbs.calMemory() / acl_rules->size, avgCheckNum, avgORNum);

		content += expID + "-d" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs)\
 + " us memory= " + Utils::Double2String(hem_bbs.calMemory() / 1024.0 / 1024.0) \
 + " MB check= " + Utils::Double2String(avgCheckNum) + " OR= "+Utils::Double2String(avgORNum)+"\n";
	}

	printf("\n\nExp%s HEM-BBS-a%d: constructTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us, "
		   "memorySize= %.3f B/'\ncheckNum= %.3f, or64Num= %.3f, ruleNum= %lu, msgNum= %lu\n\n", \
        expID.c_str(), HEM_BS_NUM_ATTR, totalConstructionTimeUs / numDataSets, totalAvgInsertionTimeUs / numDataSets, \
        totalAvgSearchTimeUs / numDataSets, totalAvgMemorySizeB / numDataSets, \
        totalAvgCheckNum / numDataSets, totalAvgORNum / numDataSets, totalRules, totalMessages);
#if DEBUG
	content+="DEBUG";
#endif
	content += "Exp" + expID + "-a" + to_string(HEM_BS_NUM_ATTR) + "-D" + to_string(DATASET_NO) + "-S"
			   + to_string(SHUFFLEMESSAGES)\
 + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us avgI= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us avgCST= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us avgM= " + Utils::Double2String(totalAvgMemorySizeB / numDataSets)\
 + " B/' avgCEK= " + Utils::Double2String(totalAvgCheckNum / numDataSets)\
 + " avgOR= " + Utils::Double2String(totalAvgORNum / numDataSets) + "\n";
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






