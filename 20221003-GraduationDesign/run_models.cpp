//
// Created by swh on 22-10-4.
//

#include "run_models.h"

ModelsTest::ModelsTest()
{
	rules = nullptr;
	messages = nullptr;
}

ModelsTest::~ModelsTest()
{
	if (rules) delete rules;
	if (messages) delete messages;
	rules = nullptr;
	messages = nullptr;
}

void ModelsTest::readDatasets(int dno)
{
	if (rules) delete rules;
	if (messages) delete messages;
	rules = new ACL_rules;
	messages = new ACL_messages;

	read_rules_cbFormat(ruleFileName[dno], rules);
	printf("read rules file (%s %d)\n", ruleFileName[dno], rules->size);

	read_messages_cbFormat(headFileName[dno], messages);
	printf("read_messages (%s %d)\n", headFileName[dno], messages->size);
}

void ModelsTest::HEMBS_forward_test()
{
	HEMBS hembs;
	clock_t clk = clock();

}

void ModelsTest::HEMBS_backward_test()
{
	HEMBS hembs;

	double totalConstructionTimeUs = 0.0;
	double totalAvgInsertionTimeUs = 0.0;
	double totalAvgSearchTimeUs = 0.0;
	double totalMemorySizeMB = 0.0;

	const string file_path = "output/HEMBS_b.txt";
	string content;

	for (int dno = 1; dno < numDataSets; dno++)
	{
		readDatasets(dno);
		clock_t clk = clock();
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
			if (messages->list[i].rule_id == (unsigned int)-1)
			{
				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			}
			else if (messages->list[i].rule_id == 0)
			{
				printf("msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			}
			if (messages->list[i].rule_id != ruleNo)
			{
				printf("Error result: msg %u matches rule %u while the result is %u\n", i, messages->list[i].rule_id, ruleNo);
			}
#endif
		}
		double avgSearchTimeUs = (double)(clock() - clk) * 1000000.0 / CLOCKS_PER_SEC / messages->size;
		totalAvgSearchTimeUs += avgSearchTimeUs;
		totalMemorySizeMB += hembs.memory_size();

#if DEBUG
		printf("HEMBS-f msg %s (%d/%d): constructionTime= %.3f us, insertionTime= %.3f us, searchTime= %.3f us, memorySize= %.3f MB\n\n", \
        headFileName[dno], rules->size, messages->size, constructionTimeUs, avgInsertionTimeUs, avgSearchTimeUs, hembs.memory_size());
#endif

		content += expID + "-D" + to_string(dno + 1) \
 + ": search= " + Utils::Double2String(avgSearchTimeUs)\
 + " us insert= " + Utils::Double2String(avgInsertionTimeUs)\
 + " us construct= " + Utils::Double2String(constructionTimeUs / 1000000.0)\
 + " s memory= " + Utils::Double2String(hembs.memory_size()) + " MB\n";
	}

	content += expID + ": avgS= " + Utils::Double2String(totalAvgSearchTimeUs / numDataSets)\
 + " us avgI= " + Utils::Double2String(totalAvgInsertionTimeUs / numDataSets)\
 + " us avgC= " + Utils::Double2String(totalConstructionTimeUs / numDataSets)\
 + " us avgM= " + Utils::Double2String(totalMemorySizeMB / numDataSets) + " MB\n";
	Utils::WriteData2Begin(file_path, content);

}

