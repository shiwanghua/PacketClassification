//
// Created by swh on 22-10-4.
//

#include "ModelsTest.h"

// if shuffle rules, the result may be changed so cannot be verified (set DEBUG to false)
void ModelsTest::shuffleRules()
{
	rule* rp = rules->list;
	RandomGenerator rg;
	for (int ri = 0; ri < rules->size; ri++)
	{
		int rj = rg.rand_int(rules->size - ri) + ri;
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
	rules = nullptr;
	messages = nullptr;
}

ModelsTest::~ModelsTest()
{
	if (rules)
	{
		delete rules->list;
		delete rules;
	}
	if (messages)
	{
		delete messages->list;
		delete messages;
	}
	rules = nullptr;
	messages = nullptr;
}

void ModelsTest::readDatasets(int dno)
{
	if (rules)
	{
		delete rules->list;
		delete rules;
	}
	if (messages)
	{
		delete messages->list;
		delete messages;
	}
	rules = new ACL_rules;
	messages = new ACL_messages;

	read_rules_cbFormat(ruleFileName[dno], rules);
	printf("read rules file (%s %d)\n", ruleFileName[dno], rules->size);
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
			unsigned int ruleNo = bfs.brute_force_search(messages->list + i, rules);
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
			unsigned int ruleNo = bfs.brute_force_search(messages->list + i, rules);
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
	HEMBS hembs;
	clock_t clk = clock();

}

void ModelsTest::HEMBS_backward_test()
{
	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalMemorySizeMB = 0.0;
	double totalRules = 0;

	const string file_path = "output/HEMBS_b.txt";
	string content;

	for (int dno = 0; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		totalRules += rules->size;

		clock_t clk = clock();
		HEMBS hembs;
		hembs.backward_init_bitsets_IPv4(rules->size);
		double constructionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC;
		totalConstructionTimeUs += constructionTimeUs;

		clk = clock();
		for (int i = 0; i < rules->size; i++)
		{
			hembs.backward_bitsets_insert_IPv4(rules->list + i);
		}
		double avgInsertionTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / rules->size;
		totalAvgInsertionTimeUs += avgInsertionTimeUs;

		clk = clock();
		for (int i = 0; i < messages->size; i++)
		{
			unsigned int ruleNo = hembs.backward_bitsets_search_IPv4(messages->list + i, rules);
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
		totalMemorySizeMB += hembs.memory_size();

#if DEBUG
		printf("HEMBS-f dataset %d: constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us, memorySize= %.3f MB\n\n", \
        dno + 1, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs, hembs.memory_size());
#endif

		content += expID + "-D" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us insert_ip_layer= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs / 1000000.0)\
 + " s memory= " + Utils::Double2String(hembs.memory_size()) + " MB\n";
	}

	printf("\n\nTotal HEMBS-f: constructionTime= %.3f s, insertionTime= %.3f us, searchTime= %.3f us, memorySize= %.3f B/'\n\n", \
		totalConstructionTimeUs / 1000000.0 / numDataSets, totalAvgInsertionTimeUs / numDataSets, \
		totalAvgSearchTimeUs / numDataSets, totalMemorySizeMB * 1024.0 * 1024.0 / totalRules);

	content += expID + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us avgI= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us avgC= " + Utils::Double2String(totalConstructionTimeUs / 1000000.0 / numDataSets)\
 + " s avgM= " + Utils::Double2String(totalMemorySizeMB * 1024.0 * 1024.0 / totalRules) + " B/'\n";
	Utils::WriteData2Begin(file_path, content);

}






