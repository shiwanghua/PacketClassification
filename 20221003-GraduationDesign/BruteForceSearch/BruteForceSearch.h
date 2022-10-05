//
// Created by swh on 22-10-5.
//

#ifndef _BRUTEFORCESEARCH_H_
#define _BRUTEFORCESEARCH_H_

#include "data_structure.h"
#include <cstring>

class BruteForceSearch
{
 public:
	unsigned int brute_force_search(const message* list, const ACL_rules* rules);
};

#endif //_BRUTEFORCESEARCH_H_
