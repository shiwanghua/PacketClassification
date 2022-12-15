#ifndef __READ_H_
#define __READ_H_
#include"tool.h"

void read_rules_cbFormat(const char* file_name, ACL_rules* rules);
void read_messages_cbFormat(const char* file_name, ACL_messages* messages);
void read_messages_gt_cbFormat(const char* file_name, ACL_messages* messages,std::vector<int>& gt);

void read_rules_ycFormat(const char* file_name, ACL_rules* rules);
void read_rules_bitset_ycFormat(const char* file_name, ACL_rules* rules); // source_ip，destination_ip 没有把IP反过来存
void read_messages_ycFormat(const char* file_name, ACL_messages* messages);

#endif // !__READ_H_
