#include"tool.h"

void generateMessage() {
	char ruleFileName[12][50] = {"acl1_256k.txt", "acl2_256k.txt", "acl3_256k.txt", "acl4_256k.txt", "acl5_256k.txt",
								 "fw1_256k.txt", "fw2_256k.txt", "fw3_256k.txt", "fw4_256k.txt", "fw5_256k.txt",
								 "ipc1_256k.txt", "ipc2_256k.txt"};
	char headFileName[12][50] = {"acl1_256k_trace.txt", "acl2_256k_trace.txt", "acl3_256k_trace.txt",
								 "acl4_256k_trace.txt", "acl5_256k_trace.txt", "fw1_256k_trace.txt",
								 "fw2_256k_trace.txt", "fw3_256k_trace.txt", "fw4_256k_trace.txt", "fw5_256k_trace.txt",
								 "ipc1_256k_trace.txt", "ipc2_256k_trace.txt"};
	srand((int) time(0));
	for (int q = 0; q < 12; q++) {
		ACL_rules datasets = {0, 0, 0};
		ACL_messages message_sets = {0, 0, 0};

		char tmpFileName[100] = "classbench_256k/";
		strcat(tmpFileName, ruleFileName[q]);
		read_rules(tmpFileName, &datasets);

		int numMessage = datasets.size * 2;
		unsigned int sip, dip, sm, dm;
		int fromRuleNo[numMessage];
		message msgList[numMessage], tempMsg;
		rule *r = datasets.list;
		for (int i = 0; i < numMessage; i++) {
			if (i == datasets.size)
				r = datasets.list;

			fromRuleNo[i] = i % datasets.size;
			memcpy(&sip, r->source_ip, 4);
			memcpy(&dip, r->destination_ip, 4);
			sm = 32 - (unsigned int) r->source_mask; // 自由位数
			dm = 32 - (unsigned int) r->destination_mask;

			if (sm > 0) {
				if (sm == 32) // all 自由位数
					sip = rand() % 2 == 1 ? rand() | (1 << 31) : rand();
				else sip = (sip >> sm << sm) + ((unsigned int) rand() << (32 - sm) >> (32 - sm));
			}
			if (dm > 0) {
				if (dm == 32) // all 自由位数
					dip = rand() % 2 == 1 ? rand() | (1 << 31) : rand();
				else dip = (dip >> dm << dm) + ((unsigned int) rand() << (32 - dm) >> (32 - dm));
			}

			memcpy(msgList[i].source_ip, &sip, 4);
			memcpy(msgList[i].destination_ip, &dip, 4);

			if ((unsigned int) r->protocol[0] == 255)msgList[i].protocol = (unsigned int) r->protocol[1];
			else msgList[i].protocol = rand() % 14;

			if (r->source_port[0] == r->source_port[1])msgList[i].source_port = r->source_port[0];
			else msgList[i].source_port = r->source_port[0] + rand() % (r->source_port[1] - r->source_port[0]);

			if (r->destination_port[0] == r->destination_port[1])msgList[i].destination_port = r->destination_port[0];
			else
				msgList[i].destination_port = r->destination_port[0] + \
                (unsigned short) rand() % (r->destination_port[1] - r->destination_port[0]);

			r++;
		}

		for (int i = 0, to, tempInt; i < numMessage; i++) {
			to = rand() % numMessage;
			tempMsg = msgList[i];
			msgList[i] = msgList[to];
			msgList[to] = tempMsg;
			tempInt = fromRuleNo[i];
			fromRuleNo[i] = to;
			fromRuleNo[to] = tempInt;
		}

		FILE *fp = NULL;
		strcpy(tmpFileName, "classbench_256k_s/");
		strcat(tmpFileName, headFileName[q]);
		fp = fopen(tmpFileName, "w");
		for (int i = 0; i < numMessage; i++) {
			memcpy(&sip, msgList[i].source_ip, 4);
			memcpy(&dip, msgList[i].destination_ip, 4);
			fprintf(fp, "%u\t%u\t%u\t%u\t%u\t%u\t%d\n", sip, dip, msgList[i].source_port, msgList[i].destination_port,
					msgList[i].protocol, 0, fromRuleNo[i]);
//			printf("%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n", sip,dip,msgList[i].source_ip, msgList[i].destination_ip, msgList[i].source_port,
//				   msgList[i].destination_port, msgList[i].protocol,fromRuleNo[i]);
//			printf("%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\n", sip,dip,msgList[i].source_ip, msgList[i].destination_ip, msgList[i].source_port,
//				   msgList[i].destination_port, msgList[i].protocol,fromRuleNo[i]);
		}
		fclose(fp);
	}
}

void add_rule(ACL_rules *rules, rule *r) {
	if (rules->size < rules->capacity) {
		memcpy(rules->list + rules->size, r, sizeof(rule));
		rules->size++;
	} else {
		rules->capacity += 8;
		rule *p = (rule *) realloc(rules->list, rules->capacity * sizeof(rule));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		} else {
			rules->list = p;
			memcpy(rules->list + rules->size, r, sizeof(rule));
			rules->size++;
		}
	}
}

void add_data(Cell *c, data *d) {
	if (c->size < c->capacity) {
		memcpy(c->list + c->size, d, sizeof(data));
		c->size++;
	} else {
		c->capacity += 4;
		data *p = (data *) realloc(c->list, c->capacity * sizeof(data));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		} else {
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

void add_data_ordered2(Cell2 *c, data2 *d) {
	if (c->size >= c->capacity) {
		c->capacity += 4;
		data2 *p = (data *) realloc(c->list, c->capacity * sizeof(data2));
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
	memmove(c->list + i + 1, c->list + i, sizeof(data2) * (c->size - i));
	memcpy(c->list + i, d, sizeof(data2));
	c->size++;
}

void add_message(ACL_messages *messages, message *m) {
	if (messages->size < messages->capacity) {
		memcpy(messages->list + messages->size, m, sizeof(message));
		messages->size++;
	} else {
		messages->capacity += 8;
		message *p = (message *) realloc(messages->list, messages->capacity * sizeof(message));
		if (p == NULL) {
			fprintf(stderr, "Error - unable to allocate required memory\n");
		} else {
			messages->list = p;
			memcpy(messages->list + messages->size, m, sizeof(message));
			messages->size++;
		}
	}
}

uint64_t inline

GetCPUCycle() {
#ifdef __x86_64__
	unsigned int lo, hi;
	// __asm__ 指示编译器在此插入汇编语句, 声明一个内联汇编表达式
	// __volatile__ 严禁将此处的汇编语句与其它的语句重组合优化
	// memory gcc编译器假设RAM所有内存单元均被汇编指令修改，这样cpu中的registers和cache中已缓存的内存单元中的数据将作废, cpu将不得不在需要的时候重新读取内存中的数据
	// lfence 在lfence指令前的读操作当必须在lfence指令后的读操作前完成
	__asm__ __volatile__("lfence" : : : "memory"); // 内存屏障
	// rdtsc: ReaD TimeStamp Counter, it reads a 64-bit counter that counts up at every clock cycle of your processor.
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
#elif __aarch64__
	uint64_t v = 0;
	//  Instruction Synchronization Barrier (ISB) flushes the pipeline in the processor, so that all instructions following the ISB are fetched from cache or memory, after the ISB has been completed.
	asm volatile("isb" : : : "memory");
	asm volatile("mrs %0, cntvct_el0" : "=r"(v)); // 虚拟计时器计数寄存器 CNTPCT_EL0物理计时器计数寄存器
	// asm volatile("mrs %0, pmccntr_el0" : "=r" (v)); // Performance Monitors Cycle Count Register
	return v;
#else
	printf("unknown arch\n");
	return 0;
#endif
}