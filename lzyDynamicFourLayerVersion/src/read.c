#include "read.h"
#include "read.h"

void read_rules(const char* file_name, ACL_rules* rules)
{
	printf("read_rules begin %s ...\n", file_name);

	FILE* fp = NULL;
	fp = fopen(file_name, "r");
	unsigned int sIp[5];
	unsigned int dIp[5];
	unsigned int sPort[2];
	unsigned int dPort[2];
	unsigned int protocol;
	unsigned int index;
	int i = 0;
	while (fscanf(fp, "sip=%u.%u.%u.%u/%u,dip=%u.%u.%u.%u/%u,sport=%u:%u,dport=%u:%u,proto=%u,index=%u\n", &sIp[0], &sIp[1], &sIp[2], &sIp[3], &sIp[4],
		&dIp[0], &dIp[1], &dIp[2], &dIp[3], &dIp[4],
		&sPort[0], &sPort[1], &dPort[0], &dPort[1], &protocol, &index) != EOF) {

		/*
		printf("sip=%u.%u.%u.%u/%u,dip=%u.%u.%u.%u/%u,sport=%u:%u,dport=%u:%u,proto=%u,index=%u\n", sIp[0], sIp[1], sIp[2], sIp[3], sIp[4],
		dIp[0], dIp[1], dIp[2], dIp[3], dIp[4], sPort[0], sPort[1], dPort[0], dPort[1], protocol, index);
		*/

		rule r;
		r.PRI = index;

		if (protocol == 0) {
			r.protocol[0] = 0;
			r.protocol[1] = 0;
		}
		else {
			r.protocol[0] = 1;
			r.protocol[1] = (unsigned char)protocol;
		}

		r.source_mask = (unsigned char)sIp[4];
		r.destination_mask = (unsigned char)dIp[4];
		int k = 4;
		for (int j = 0; j < 4; j++) {
			r.source_ip[j] = (unsigned char)sIp[--k];
			r.destination_ip[j] = (unsigned char)dIp[k];
		}
		r.source_port[0] = (unsigned short)sPort[0]; r.source_port[1] = (unsigned short)sPort[1];
		r.destination_port[0] = (unsigned short)dPort[0]; r.destination_port[1] = (unsigned short)dPort[1];
		add_rule(rules, &r);
		i++;
	}
	fclose(fp);
	printf("read_rules %d done %s\n", i, file_name);
}

void read_messages(const char* file_name, ACL_messages* messages)
{
	printf("read_messages begin %s ...\n", file_name);
	FILE* fp = NULL;
	fp = fopen(file_name, "r");
	message _m = { 0 };
	unsigned int sIp[4], dIp[4];
	while (fscanf(fp, "sip=%u.%u.%u.%u,dip=%u.%u.%u.%u,sport=%u,dport=%u,proto=%u,index=%u\n",
		&sIp[0], &sIp[1], &sIp[2], &sIp[3], &dIp[0], &dIp[1], &dIp[2], &dIp[3],
		&_m.source_port, &_m.destination_port, &_m.protocol, &_m.rule_id) != EOF) {

		for (unsigned i = 0; i < 4; i++) {
			_m.source_ip[i] = (unsigned char)sIp[3 - i];
			_m.destination_ip[i] = (unsigned char)dIp[3 - i];
		}

		//printf("%u\t%u\t%u\t%u\t%u\n", ip_src, ip_des, _m.source_port, _m.destination_port, _m.protocol);
		add_message(messages, &_m);
	}
	fclose(fp);
	printf("read_messages done %s\n", file_name);
}
