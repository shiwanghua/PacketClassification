#include<iostream>
#include "ModelsTest.h"
using namespace std;

int main()
{
	srand((int)time(0));
	ModelsTest mt;
//	mt.generateGroundthTruthInTupleFile();
//	mt.BruteForceSearch_test();
	mt.HEMBS_forward_test();
	mt.HEMBS_backward_test();
//	mt.TamaSearch_test();
	cout << "DONE.\n";
	return 0;
}