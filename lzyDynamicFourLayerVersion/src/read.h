#ifndef __READ_H_
#define __READ_H_
#include"tool.h"

void read_rules(const char* file_name, ACL_rules* rules);
void read_messages(const char* file_name, ACL_messages* messages);

void read_rules_bitset(const char* file_name, ACL_rules* rules);
#endif // !__READ_H_
