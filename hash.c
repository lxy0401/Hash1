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
        hashtable->data[i].stat = Empty;
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
        hashtable->data[i].stat = Empty;
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
    if(hashtable->size >= 0.8*HashMaxSize)
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

int HashFind(HashTable* hashtable,KeyType key,ValueType* value)
{
    if(hashtable == NULL || value == NULL)
    {
        return 0;
    }
    if(hashtable->size == 0)
    {
        //空hash表
        return 0;
    }
    //1.根据key计算出offset
    size_t offset = hashtable->func(key);
    //2.从offset开始往后查找，每次取到一个元素，使用key进行比较
    while(1)
    {
        if(hashtable->data[offset].key == key && hashtable->data[offset].stat == Valid)
        {
            //找到了
            //a)找到了key相同的元素，此时直接返回value，认为查找成功
            *value = hashtable->data[offset].value;
            return 1;
        }
        else if(hashtable->data[offset].stat == Empty)
        {
            //查找失败
            //b)发现当前key不相同，就继续往后查找
            return 0;
        }
        else
        {
            //c)若发现当前元素为NULL，认为查找失败
            ++offset;
            offset = offset>= HashMaxSize?0:offset;
        }
    }
    return 0;
}

void HashRemove(HashTable* hashtable,KeyType key)
{
    if(hashtable == NULL)
    {
        return ;
    }
    if(hashtable->size == 0)
    {
        return ;
    }
    //1.根据key计算offset
    size_t offset = hashtable->func(key);
    //2.从offset开始一次判断当前元素的key和要删除的key是不是相同
    while(1)
    {
        if(hashtable->data[offset].key == key && hashtable->data[offset].stat == Valid)
        {
            //a)若当前的key就是要删除的key，删除当前元素即可，删除元素要引入一个新的状态标记Delete
            //找到了要删除的元素，要标记成 Delete 
            hashtable->data[offset].stat = Delete;
            --hashtable->size;
            return ;
        }
        else if(hashtable->data[offset].stat == Empty)
        {
            //b)若当前元素为NULL，key在hash表查找删除失败
            //删除失败
            return ;
        }
        else
        {
            //c)剩下的情况++offset，线性的探测下一个元素
            ++offset;
            offset = offset >= HashMaxSize?0:offset;
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
void HashPrint(HashTable* hashtable,const char* msg)
{
    printf("[%s]\n",msg);
    size_t i = 0;
    for(;i < HashMaxSize ;++i)
    {
        if(hashtable->data[i].stat == Empty)
        {
            continue;
        }
        printf("[%d:%d]",hashtable->data[i].key,hashtable->data[i].value);
    }
    printf("\n");

}

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

void TestInsert()
{
    TEST_HEADER;
    HashTable hashtable;
    HashInit(&hashtable,HashFuncDefault);
    HashInsert(&hashtable,1,1);
    HashInsert(&hashtable,1,10);
    HashInsert(&hashtable,2,2);
    HashInsert(&hashtable,1001,11);
    HashInsert(&hashtable,1002,12);
    HashPrint(&hashtable,"插入若干个元素");

}

void TestFind()
{
    TEST_HEADER;
    HashTable hashtable;
    HashInit(&hashtable,HashFuncDefault);
    HashInsert(&hashtable,1,1);
    HashInsert(&hashtable,1,10);
    HashInsert(&hashtable,2,2);
    HashInsert(&hashtable,1001,11);
    HashInsert(&hashtable,1002,12);
    ValueType value;
    int ret = HashFind(&hashtable,1002,&value);
    printf("ret expected 1,actual %d\n",ret);
    printf("value expected 12,actual %d\n",value);
    return ;
}

void TestRemove()
{
    TEST_HEADER;
    HashTable hashtable;
    HashInit(&hashtable,HashFuncDefault);
    HashInsert(&hashtable,1,1);
    HashInsert(&hashtable,1,10);
    HashInsert(&hashtable,2,2);
    HashInsert(&hashtable,1001,11);
    HashInsert(&hashtable,1002,12);
    HashRemove(&hashtable,2);
    ValueType value;
    int ret1 = HashFind(&hashtable,1002,&value);
    printf("ret expected 1,actual %d\n",ret1);
    printf("value expected 12,actual %d\n",value);
    int ret2 = HashFind(&hashtable,2,&value);
    printf("ret expected 0,actual %d\n",ret2);
    
}
int main()
{
    TestInit();
    TestDestroy();
    TestInsert();
    TestFind();
    TestRemove();
    return 0;
}
#endif
