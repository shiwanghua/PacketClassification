#include<iostream>
#include "ModelsTest.h"
using namespace std;

int main()
{
	srand((int)time(0));
	ModelsTest mt;
//	mt.generateGroundthTruthInTupleFile();
//	mt.BruteForceSearch_test();
	// mt.HEMBS_forward_test();
	// mt.HEMBS_backward_test();
	mt.HEMBS_aggregate_forward_test();
//	mt.HEMBS_RLE_forward_test();
//	mt.TamaSearch_test();

	// mt.IP_visualization();
	// mt.Protocol_visualization();
	// mt.Port_subspace_visualization();
	// mt.Port_width_visualization();
	// mt.FBS_bitsets_visualization();
	cout << "DONE.\n";
	return 0;
}