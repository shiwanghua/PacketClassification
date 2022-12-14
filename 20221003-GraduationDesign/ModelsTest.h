//
// Created by swh on 22-10-4.
//

#ifndef _MODELSTEST_H_
#define _MODELSTEST_H_

#include "BruteForceSearch.h"
#include "HEMBS.h"
#include "MultiLevelHash.h"
#include "TamaSearch.h"
#include"io/read.h"
#include "io/constant.h"
#include "io/Utils.h"
#include "io/random.h"
#include <string>
#include <iostream>

using namespace std;

class ModelsTest
{
#if DATASET_NO==1
	// Datasets1: 第一、二批原始数据 history data
	static constexpr int numDataSets = 17;
	char ruleFileName[numDataSets][60] { "acl128_same_fmt_with_cb256k/acl_rules2_cb.txt",
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
	char headFileName[numDataSets][80] = { "acl128_same_fmt_with_cb256k/acl_tuples2_cb.txt",
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
#elif DATASET_NO==2
	// Datasets2: trace_generator a=1, b=0.8, scale=5
	static constexpr int numDataSets = 17;
	char ruleFileName[numDataSets][60] { "acl128_same_fmt_with_cb256k/acl_rules2_cb.txt",
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
	char headFileName[numDataSets][80] = { "acl128_same_fmt_with_cb256k_b0.8/acl_rules2_cb_trace.txt",
										   "acl128_same_fmt_with_cb256k_b0.8/acl_rules3_cb_trace.txt",
										   "acl128_same_fmt_with_cb256k_b0.8/acl_rules4_cb_trace.txt",
										   "acl128_same_fmt_with_cb256k_b0.8/acl_rules5_cb_trace.txt",
										   "acl128_same_fmt_with_cb256k_b0.8/acl_rules6_cb_trace.txt",
										   "classbench_256k_b0.8/acl1_256k_trace.txt", "classbench_256k_b0.8/acl2_256k_trace.txt",
										   "classbench_256k_b0.8/acl3_256k_trace.txt",
										   "classbench_256k_b0.8/acl4_256k_trace.txt", "classbench_256k_b0.8/acl5_256k_trace.txt",
										   "classbench_256k_b0.8/fw1_256k_trace.txt",
										   "classbench_256k_b0.8/fw2_256k_trace.txt", "classbench_256k_b0.8/fw3_256k_trace.txt",
										   "classbench_256k_b0.8/fw4_256k_trace.txt",
										   "classbench_256k_b0.8/fw5_256k_trace.txt", "classbench_256k_b0.8/ipc1_256k_trace.txt",
										   "classbench_256k_b0.8/ipc2_256k_trace.txt" };
#elif DATASET_NO==3
	// Datasets3: trace_generator a=1, b=0~1.0, scale=10
	static constexpr int numDataSets = 11;
	char ruleFileName[numDataSets][60]{ "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt",
										 "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt",
										 "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt",
										 "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt",
										 "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt", "cb_ipc2/ipc2_256k.txt" };
	char headFileName[numDataSets][80] = { "cb_ipc2/ipc2_256k_trace_b0.0.txt", "cb_ipc2/ipc2_256k_trace_b0.1.txt",
											"cb_ipc2/ipc2_256k_trace_b0.2.txt", "cb_ipc2/ipc2_256k_trace_b0.3.txt",
											"cb_ipc2/ipc2_256k_trace_b0.4.txt", "cb_ipc2/ipc2_256k_trace_b0.5.txt",
											"cb_ipc2/ipc2_256k_trace_b0.6.txt", "cb_ipc2/ipc2_256k_trace_b0.7.txt",
											"cb_ipc2/ipc2_256k_trace_b0.8.txt", "cb_ipc2/ipc2_256k_trace_b0.9.txt",
											"cb_ipc2/ipc2_256k_trace_b1.0.txt" };
#endif

	ACL_rules* rules;
	ACL_messages* messages;

	void shuffleRules();
	void shuffleMessages();

 public:
	ModelsTest();
	~ModelsTest();

	void readDatasets(int dno);
	void generateGroundthTruthInTupleFile(); // In the last of each row of trace file, give the answer.

	void BruteForceSearch_test();

	void HEMBS_forward_test();
	void HEMBS_backward_test();

	void TamaSearch_test();
};

#endif //_MODELSTEST_H_
