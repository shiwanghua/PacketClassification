
#include <stdio.h>
#include "aclAlg.h"

typedef struct AclTable_t {
    int todo; /* 请自行设计的ACL算法所需的数据结构 */
} AclTable;

/* 创建一个ACL Table对象，具体的AclTable数据结构由用户自行决定
    * 如果用户不需要创建Table对象，直接返回NULL即可 */
AclTable* aclCreate()
{
    AclTable *tbl = (AclTable*)aclMemMalloc(sizeof(AclTable));
    /* 初始化你的算法数据结构 */

    return tbl;
}

    /* 往ACL Table中添加 num 条ACL规则，成功返回0，否则返回非0 */
uint32_t aclInsert(AclTable *tbl, AclRule *rules, uint32_t num)
{
    for (uint32_t i = 0; i < num; i++) {
        AclRule *r = &rules[i];
        /*printf("\nInsert rule sip 0x%08x/%u, dip 0x%08x/%u, sport [%u-%u], dport [%u-%u], proto %u, index %u.", 
            r->sip.ip, r->sip.len, r->dip.ip, r->dip.len,
            r->sport.from, r->sport.to,
            r->dport.from, r->dport.to,
            r->proto, r->index);*/
    }
    return 0;
}

/* 查询一条ACL五元组，如果命中所添加的规则，则返回该规则对应的index，否则返回0。
 * 如果命中多条规则，则返回优先级最高的规则对应的index。
 * 注意：代码中的printf等IO显示操作将会极大地影响算法性能，非必要时请尽量不要使用 */
uint32_t aclSearch(AclTable *tbl, AclTuple *tuple)
{
    /*printf("\nSearch sip 0x%08x, dip 0x%08x, sport %u, dport %u, proto %u.", 
		tuple->sip, tuple->dip, tuple->sport, tuple->dport, tuple->proto);*/
    return 0;
}