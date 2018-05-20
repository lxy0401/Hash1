#include "hash.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t HashFuncDefault(KeyType key)
{
    return key%HashMaxSize;
}

void HashInit(HashTable* hashtable,HashFunc hash_func)
{
    hashtable->size = 0;
    hashtable->func = hash_func;
    size_t i = 0;
    for(;i < HashMaxSize; ++i)
    {
        hashtable->data[i].Stat = Empty;
    }
    return ;
}

void HashDestroy(HashTable* hashtable)
{
    hashtable->size = 0;
    hashtable->func = NULL;
    size_t i = 0;
    for(;i < HashMaxSize; ++i)
    {
        hashtable->data[i].Stat = Empty;
    }
    return ;
}

void HashInsert(HashTable* hashtable,KeyType key,ValueType value)
{
    if(hashtable == NULL)
    {
        //非法操作
        return ;
    }
    //1.判定hash表是否能继续插入（根据负载因子判定）
    //此处只是把负载因子定义为0.8
    if(hashtable >= 0.8* HashMaxSize)
    {
        //发现当前的hash表已达到负载因子的上限，此时直接插入失败
        return ;
    }
    //2.根据key来计算offset
    size_t offset = hashtable->func(key);
    //3.从offset位置开始线性的往后查找，找到第一个状态为empty的元素进行插入
    while(1)
    {
        if(hashtable->data[offset].stat == Empty)
        {
            //此时找不到合适的位置放置要插入的元素
            hashtable->data[offset].stat = Valid;
            hashtable->data[offset].key = key;
            hashtable->data[offset].value = value;
            //5.++size
            ++hashtable->size;
            return ;
        }
        else if(hashtable->data[offset].stat == Valid && hashtable->data[offset].key == key)
        {
            //4.若发现key相同的元素，此时认为插入失败
            //hash表中存在了一个key相同的元素
            //认为插入失败
            //若要修改value的值就放开下面这行代码
            //hashtable->data[offset].value = value;
            return ;
        }
        else
        {
            ++offset;
            if(offset >= HashMaxSize)
            {
                offset = 0;
            }
        }
    }
    return ;
}
/*****
 *
 *以下为测试代码
 *
 *
 * *****/
#if 1
#include <stdio.h>
#define TEST_HEADER printf("\n========%s=========\n",__FUNCTION__)
void TestInit()
{
    TEST_HEADER;
    HashTable hashtable;
    HashInit(&hashtable,HashFuncDefault);
    printf("size expected 0,actual %lu\n",hashtable.size);
    printf("func expected %p,actual %p\n",HashFuncDefault,hashtable.func);
    return;
}

void TestDestroy()
{
    TEST_HEADER;
    HashTable hashtable;
    HashInit(&hashtable,HashFuncDefault);
    HashDestroy(&hashtable);
    printf("size expected 0,actual %lu\n",hashtable.size);
    printf("func expected NULL,actual %p\n",hashtable.func);
    return;

}
int main()
{
    TestInit();
    TestDestroy();
    return 0;
}
#endif
