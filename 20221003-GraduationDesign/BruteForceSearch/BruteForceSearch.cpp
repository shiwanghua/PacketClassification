//
// Created by swh on 22-10-5.
//

#include "BruteForceSearch.h"

unsigned int BruteForceSearch::brute_force_search(const message* msg, const ACL_rules* rules)
{
	for (unsigned int rno = 0; rno < rules->size; rno++)
	{
//		rule _r;
//		memcpy(&_r, rules->list + rno, sizeof(rule));

//		unsigned int _ip;
//		memcpy(&_ip, rp->destination_ip, 4);

		rule* rp = rules->list + rno;
		unsigned int free_bit = 32 - (unsigned int)rp->destination_mask;
		if (*(unsigned int*)(msg->destination_ip) >> free_bit != *(unsigned int*)rp->destination_ip >> free_bit
			&& free_bit != 32)
		{
//			printf("freebit=%u, dip-%u, rp-%u\n", free_bit,
//				*(unsigned int*)(msg->destination_ip) >> free_bit, *(unsigned int*)rp->destination_ip >> free_bit);
			continue;  // if destination ip not match, check next
		}

		free_bit = 32 - (unsigned int)rp->source_mask;
		if (*(unsigned int*)(msg->source_ip) >> free_bit != *(unsigned int*)rp->source_ip >> free_bit && free_bit != 32)
			continue;

		if ((rp->source_port[0] <= msg->source_port) && (msg->source_port <= rp->source_port[1])
			&& (rp->destination_port[0] <= msg->destination_port)
			&& (msg->destination_port <= rp->destination_port[1]))
		{
			if ((rp->protocol[1] == msg->protocol) || (rp->protocol[0] == 0))
			{
				return rno;
			}
		}
	}
//	printf("Error: found no match.\n");
//	exit(0);
	return -1;
}
