//
// Created by swh on 22-10-4.
//

#include "Utils.h"

void Utils::WriteData2Begin(string fileName, string text) {
	ifstream ifs(fileName); // 读文件
	if (!ifs) {
		cout << "Create file failed!" << endl;
	}
	string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
	ifs.close();
	ofstream fileStream;
	fileStream.open(fileName.c_str(), ios::binary | ios::ate);
	fileStream.seekp(0, ios::beg);
	fileStream << text << endl << content; // 插入到文件头部
	fileStream << flush;
	fileStream.close();
}

void Utils::WriteData2End(string fileName, string text) {
	ofstream fileStream;
	fileStream.open(fileName.c_str(), ios::binary | ios::app); //  插入到文件末尾
	fileStream << text; // 插入到文件头部
	fileStream << flush;
	fileStream.close();
}

string Utils::RemoveBlank(std::string str) {
	int strLen = str.size() - 1;
	while (strLen >= 0 && str[strLen] == ' ')
		strLen--;
	if (strLen >= 0)
		return str.substr(0, strLen + 1);
	else return "";
}

string Utils::Int2String(int value) {
	string result;
	ostringstream out;
	out << value;
	result = out.str();
	return RemoveBlank(result);
}

string Utils::Double2String(double value) {
	string result;
	char buffer[200];
	sprintf(buffer, "%lf ", value);
	result.assign(buffer);
	return RemoveBlank(result);
}

double Utils::CalVariance(double a[], uint32_t n){
	double avg = 0.0, var = 0.0;
	for(uint32_t i=0;i<n;i++)
		avg+=a[i];
	avg/=n;
	for(uint32_t i=0;i<n;i++)
		var+=(avg-a[i])*(avg-a[i]);
	return var/n;
}