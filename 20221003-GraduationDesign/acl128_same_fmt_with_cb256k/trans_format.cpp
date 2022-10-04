//
// Created by swh on 22-7-17.
//

#include "data_structure.h"
#include <assert.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sched.h>
#include <sys/time.h>

void transform_rule(const char* fromFile, const char* toFile)
{
	FILE* ffrom = fopen(fromFile, "r");
	FILE* fto = fopen(toFile, "w");

	unsigned int sIp[5];
	unsigned int dIp[5];
	unsigned int sPort[2];
	unsigned int dPort[2];
	unsigned int protocol[2];
	int index;
	//	std::fstream aFile;
	//	aFile.open(filename, std::ios::out); // write,清空再写入
	//	if (aFile.is_open()) {
	//		for (int i = 0; i < n; i++) {
	//			for (int j = 0; j < d - 1; j++) {
	//				aFile << std::to_string(*data++) << ",";
	//			}
	//			aFile << std::to_string(*data++) << "\n";
	//		}
	//		aFile.close();
	//	} else {
	//		std::cout << "Open " << filename << " fails.\n";
	//	}
	int i = 0;
	while (fscanf(ffrom,
		"sip=%u.%u.%u.%u/%u,dip=%u.%u.%u.%u/"
		"%u,sport=%u:%u,dport=%u:%u,proto=%u,index=%u\n",
		&sIp[0], &sIp[1], &sIp[2], &sIp[3], &sIp[4], &dIp[0], &dIp[1],
		&dIp[2], &dIp[3], &dIp[4], &sPort[0], &sPort[1], &dPort[0],
		&dPort[1], &protocol[0], &index) != EOF)
	{
		if (protocol[0] == 0)
		{
			protocol[1] = 0;
		}
		else
		{
			protocol[1] = 0xFF;
		}

//		printf("sip=%u.%u.%u.%u/%u,dip=%u.%u.%u.%u/"
//			   "%u,sport=%u:%u,dport=%u:%u,proto=%u,index=%u\n",
//			sIp[0], sIp[1], sIp[2], sIp[3], sIp[4], dIp[0], dIp[1], dIp[2],
//			dIp[3], dIp[4], sPort[0], sPort[1], dPort[0], dPort[1], protocol[0],
//			index);

		fprintf(fto,
			"@%u.%u.%u.%u/%u\t%u.%u.%u.%u/%u\t%u : %u\t%u : "
			"%u\t0x%x/0x%x\t0x%x/0x%x\t\n",
			sIp[0], sIp[1], sIp[2], sIp[3], sIp[4], dIp[0], dIp[1], dIp[2],
			dIp[3], dIp[4], sPort[0], sPort[1], dPort[0], dPort[1], protocol[0],
			protocol[1], i, i);
		i++;
	}
	fclose(ffrom);
	fclose(fto);
}

void transform_tuple(const char* fromFile, const char* toFile)
{
	FILE* ffrom = NULL;
	ffrom = fopen(fromFile, "r");
	FILE* fto = NULL;
	fto = fopen(toFile, "w");
	message _m = { 0 };
	unsigned int sIp[4], dIp[4];
	int i = 0;
	while (fscanf(ffrom,
		"sip=%u.%u.%u.%u,dip=%u.%u.%u.%u,sport=%hu,dport=%hu,proto=%u,"
		"index=%u\n",
		&sIp[0], &sIp[1], &sIp[2], &sIp[3], &dIp[0], &dIp[1], &dIp[2],
		&dIp[3], &_m.source_port, &_m.destination_port, &_m.protocol,
		&_m.rule_id) != EOF)
	{

		unsigned int sip =
			sIp[0] * (1 << 24) + sIp[1] * (1 << 16) + sIp[2] * (1 << 8) + sIp[3];
		unsigned int dip =
			dIp[0] * (1 << 24) + dIp[1] * (1 << 16) + dIp[2] * (1 << 8) + dIp[3];
		int ruleId = _m.rule_id - 1;
		fprintf(fto, "%u\t%u\t%u\t%u\t%u\t%u\t%d\n", sip, dip, _m.source_port,
			_m.destination_port, _m.protocol, i, ruleId);
		//		printf("%u\t%u\t%u\t%u\t%u\n", ip_src, ip_des, _m.source_port,
		//_m.destination_port, _m.protocol);
	}
	fclose(ffrom);
	fclose(fto);
}

int main()
{
	int numDataSets = 6;
	char ruleFileName[numDataSets][100] = { "rules1.txt", "rules2.txt",
											"rules3.txt", "rules4.txt",
											"rules5.txt", "rules6.txt" };
	char headFileName[numDataSets][100] = { "tuples1.txt", "tuples2.txt",
											"tuples3.txt", "tuples4.txt",
											"tuples5.txt", "tuples6.txt" };

	std::string tuple_path = "", rule_path = "";

	for (int i = 0; i < 6; i++)
	{
		char fromFileName[100] = "../acl_128k_test/";
		strcat(fromFileName, ruleFileName[i]);
		std::string toFileName = "acl_rules" + std::to_string(i + 1) + "_cb.txt";
//		transform_rule(fromFileName, toFileName.c_str());

		rule_path += "\"" + toFileName + "\", ";

		strcpy(fromFileName, "../acl_128k_test/");
		strcat(fromFileName, headFileName[i]);
		toFileName = "acl_tuples" + std::to_string(i + 1) + "_cb.txt";
//		transform_tuple(fromFileName, toFileName.c_str());

		tuple_path += "\"" + toFileName + "\", ";
	}
	std::cout<<rule_path.substr(0, rule_path.length()-3)<<"\n";
	std::cout<<tuple_path.substr(0, tuple_path.length()-3)<<"\n";

	return 0;
}
