#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

//我们此处的Hash表希望存储的数据是键值对这样的结构
#define HashMaxSize 1000
typedef int KeyType;
typedef int ValueType;
typedef size_t (*HashFunc)(KeyType key);

typedef enum
{
    Empty,//空状态
    Valid,//有效状态
    Delete,//被删改的状态
}Stat;

//这个结构体表示Hash表中的一个元素，这个元素中同时包含了键值对
typedef struct HashElem
{
    KeyType key;
    ValueType value;
    Stat stat;
}HashElem;

//[0,size)这个区间就不能表示Hash表中有效元素的区间
typedef struct HashTable
{
    HashElem data[HashMaxSize];
    size_t size;
    HashFunc func;//这是一个函数指针，指向一个Hash函数
}HashTable;

//初始化
void HashInit(HashTable* hashtable,HashFunc hash_func);

//销毁
void HashDestroy(HashTable* hashtable);

//插入
void HashInsert(HashTable* hashtable,KeyType key,ValueType value);

//查找元素
int HashFind(HashTable* hashtable,KeyType key,ValueType* value);

//删除
void HashRemove(HashTable* hashtable,KeyType key);

