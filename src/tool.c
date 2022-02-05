#include"tool.h"

void add_rule(ACL_rules* rules, rule* r) {
	if (rules->size < rules->capacity) {
		memcpy(rules->list + rules->size, r, sizeof(rule));
		rules->size++;
	}
	else {
		rules->capacity += 8;
		rule* p = (rule*)realloc(rules->list, rules->capacity * sizeof(rule));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		}
		else {
			rules->list = p;
			memcpy(rules->list + rules->size, r, sizeof(rule));
			rules->size++;
		}
	}
}

void add_data(Cell* c, data* d)
{
	if (c->size < c->capacity) {
		memcpy(c->list + c->size, d, sizeof(data));
		c->size++;
	}
	else {
		c->capacity += 4;
		data* p = (data*)realloc(c->list, c->capacity * sizeof(data));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		}
		else {
			c->list = p;
			memcpy(c->list + c->size, d, sizeof(data));
			c->size++;
		}
	}
}

void add_message(ACL_messages* messages, message* m) 
{
	if (messages->size < messages->capacity) {
		memcpy(messages->list + messages->size, m, sizeof(message));
		messages->size++;
	}
	else {
		messages->capacity += 8;
		message* p = (message*)realloc(messages->list, messages->capacity * sizeof(message));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		}
		else {
			messages->list = p;
			memcpy(messages->list + messages->size, m, sizeof(message));
			messages->size++;
		}
	}
}

uint64_t inline GetCPUCycle()
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