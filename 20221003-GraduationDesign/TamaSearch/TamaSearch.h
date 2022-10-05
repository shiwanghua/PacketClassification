//
// Created by swh on 22-10-4.
//

#ifndef _TAMASEARCH_H_
#define _TAMASEARCH_H_

#include "data_structure.h"
#include <vector>

using namespace std;

class TamaSearch
{
	int nodeCounter;
	vector<int> lchild, rchild,mid;
	vector<vector<vector<int>>> data;
	vector<int> counter;

	void initiate(int p, int l, int r, int lvl);

	int median(int l, int r);

	void insert(int p, int att, int subID, int l, int r, int low, int high, int lvl);

	bool deleteSubscription(int p, int att, int subID, int l, int r, int low, int high, int lvl);

	void match_accurate(int p, int att, int l, int r, const int value, int lvl, const ACL_rules*);

	void match_vague(int p, int att, int l, int r, const int value, int lvl);
 public:
	TamaSearch();
	~TamaSearch();

	void initize(int numRule);

	void insert(const rule*);

	bool deleteRule(const rule*);

	unsigned  int accurate_search(const message &pub, const ACL_rules* rules);

	unsigned  int vague_search(const message &pub, const ACL_rules* rules);

	double calMemory();

};

#endif //_TAMASEARCH_H_
