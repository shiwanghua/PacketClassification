#ifndef __READ_H_
#define __READ_H_
#include"tool.h"

void read_rules(const char* file_name, ACL_rules* rules);
void read_messages(const char* file_name, ACL_messages* messages);

#endif // !__READ_H_
