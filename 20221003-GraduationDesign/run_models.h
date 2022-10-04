//
// Created by swh on 22-10-4.
//

#ifndef _RUN_MODELS_H_
#define _RUN_MODELS_H_

#include "HEMBitsetsSearch/HEMBS.h"
#include "MultiLevelHash/MultiLevelHash.h"
#include "TamaSearch/TamaSearch.h"
#include"io/read.h"
#include "io/constant.h"
#include "io/Utils.h"
#include <string>

using namespace std;

class ModelsTest
{
	static constexpr int numDataSets=17;
	char ruleFileName[numDataSets][50] { "acl128_same_fmt_with_cb256k/acl_rules2_cb.txt",
										 "acl128_same_fmt_with_cb256k/acl_rules3_cb.txt",
										 "acl128_same_fmt_with_cb256k/acl_rules4_cb.txt",
										 "acl128_same_fmt_with_cb256k/acl_rules5_cb.txt",
										 "acl128_same_fmt_with_cb256k/acl_rules6_cb.txt",
										 "classbench_256k/acl1_256k.txt", "classbench_256k/acl2_256k.txt",
										 "classbench_256k/acl3_256k.txt", "classbench_256k/acl4_256k.txt",
										 "classbench_256k/acl5_256k.txt", "classbench_256k/fw1_256k.txt",
										 "classbench_256k/fw2_256k.txt", "classbench_256k/fw3_256k.txt",
										 "classbench_256k/fw4_256k.txt", "classbench_256k/fw5_256k.txt",
										 "classbench_256k/ipc1_256k.txt", "classbench_256k/ipc2_256k.txt" };
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
										   "classbench_256k/ipc2_256k_trace.txt" };
	ACL_rules* rules;
	ACL_messages* messages;
#if VERIFICATION
	std::vector<int> ground_truth;
#endif

 public:
	ModelsTest();
	~ModelsTest();

	void readDatasets(int dno);

	void HEMBS_forward_test();
	void HEMBS_backward_test();
};

#endif //_RUN_MODELS_H_
