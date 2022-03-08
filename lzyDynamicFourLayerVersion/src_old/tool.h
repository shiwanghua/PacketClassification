#ifndef __TOOL_H_
#define __TOOL_H_
#include<string.h>
#include<malloc.h>
#include <stdint.h>
#include"data_structure.h"
#include<stdlib.h>
#include<time.h>

void add_rule(ACL_rules* rules, rule* r);
void add_data(Cell* c, data* d);
void add_data_ordered(Cell *c, data *d);
void add_message(ACL_messages* messages, message* m);

uint64_t GetCPUCycle();

#endif // !__C_VECTOR_H_
