#include<iostream>
#include "run_models.h"
using namespace std;

int main(){
	srand((int)time(0));
	ModelsTest mt;
	mt.HEMBS_backward_test();
	cout<<"DONE.\n";
	return 0;
}