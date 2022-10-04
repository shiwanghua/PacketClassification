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
		c->capacity += 8;
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

void add_data_ordered(Cell *c, data *d) {
	if (c->size >= c->capacity) {
		c->capacity += 4;
		data *p = (data *) realloc(c->list, c->capacity * sizeof(data));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		} else {
			c->list = p;
		}
	}
	int i = 0,j=c->size-1,m;
	while (i <= j){
		m=i+((j-i)>>1);
		if (c->list[m].PRI < d->PRI)
			i=m+1;
		else j=m-1;
	}
	memmove(c->list + i + 1, c->list + i, sizeof(data) * (c->size - i));
	memcpy(c->list + i, d, sizeof(data));
	c->size++;
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