//
// Created by swh on 22-10-4.
//

#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Utils {
 public:
	static void WriteData2Begin(string fileName, string text);
	static void WriteData2End(string fileName, string text);

	static string RemoveBlank(std::string str);
	static string Int2String(int value);
	static string Double2String(double value);

	static double CalVariance(double a[], uint32_t);

//	static inline void bitsetOr(bitset<subs>& b1, const bitset<subs>& b2); // b1=b1|b2;

//	static uint64_t GetCPUCycle();
};

inline uint64_t GetCPUCycle()
{
#ifdef __x86_64__
	unsigned int lo, hi;
	__asm__ __volatile__("lfence" : : : "memory");
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
#elif __aarch64__
	uint64_t v = 0;
	asm volatile("isb" : : : "memory");
	asm volatile("mrs %0, cntvct_el0" : "=r"(v));
	return v;
#else
	printf("unknown arch\n");
	return 0;
#endif
}

#endif //_UTILS_H_
