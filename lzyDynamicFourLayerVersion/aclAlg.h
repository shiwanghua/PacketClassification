
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    typedef struct AclRule_t {
        struct {
            uint32_t ip, len;
        } sip, dip;
        struct {
            uint16_t from, to;
        } sport, dport;
        uint8_t proto; /* 为0表示不关注 */
        uint32_t index; /* 规则index，亦代表优先级，值越小优先级越高，0表示无效 */
    } AclRule;

    typedef struct AclTuple_t {
        uint32_t sip, dip;
        uint16_t sport, dport;
        uint8_t proto;
    } AclTuple;

    /* 如果需要使用堆内存资源，需使用aclMemMalloc和aclMemFree进行申请和释放
     * !!! 禁止直接使用系统malloc和free !!! */
    void* aclMemMalloc(size_t size);
    void* aclMemCalloc(int32_t n, size_t size);
    void  aclMemFree(void *ptr);
    #define malloc forbidden_malloc()
    #define free forbidden_free()
    #define calloc forbidden_calloc()

    typedef struct AclTable_t AclTable;

    /* 创建一个ACL Table对象，具体的AclTable数据结构由用户自行决定
     * 如果用户不需要创建Table对象，直接返回NULL即可 */
    AclTable* aclCreate();

    /* 往ACL Table中添加 num 条ACL规则，成功返回0，否则返回非0 */
    uint32_t aclInsert(AclTable *tbl, AclRule *rules, uint32_t num);

    /* 查询一条ACL五元组，如果命中所添加的规则，则返回该规则对应的index，否则返回0 */
    uint32_t aclSearch(AclTable *tbl, AclTuple *tuple);

#ifdef __cplusplus
}
#endif
