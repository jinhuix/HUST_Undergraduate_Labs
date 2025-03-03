//
// csim.c-使用C编写一个Cache模拟器，它可以处理来自Valgrind的跟踪和输出统计信息，
// 如命中、未命中和逐出的次数。更换政策是LRU。
// 设计和假设:
// 1. 每个加载存储最多可导致一个缓存未命中。（最大请求是8个字节。）
// 2. 忽略指令负载（I），因为我们有兴趣评估trace.c内容中数据存储性能。
// 3. 数据修改（M）被视为加载，然后存储到同一地址。因此，M操作可能导致两次缓存命中，或者一次未命中和一次命中，外加一次可能的逐出。
// 使用函数printSummary() 打印输出，输出hits, misses and evictions 的数，这对结果评估很重要
// 

#include <assert.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"


// added for part A
typedef struct{
    int valid;  // 有效位
    int tag;    // 标签
    int time;   // 时间戳，记录上一次访问这条缓存的时间
} CacheLine, *CacheSet, **Cache;

// added for part A
int hit, miss, evict;  // 表示访问结果
int s, S, E, b;         // 缓存接收的个参数：组索引的位数、组数、行数、块大小中的b
char filePath[100];     // trace文件的路径
Cache cache;            // 整个缓存


// 动态分配缓存空间
void mallocCache(){
    // 先给整个cache分配空间
    cache = (Cache)malloc(S * sizeof(CacheSet));
    assert(cache);

    // 再给每一行的CacheLine分配空间并初始化
    for(int i=0; i<S; i++){
        cache[i] = (CacheSet)malloc(E * sizeof(CacheLine));
        assert(cache[i]);
        memset(cache[i],0,sizeof(CacheLine)*E);
    }
}


// 释放缓存空间
void freeCache(){
    // 先释放每一个CacheLine
    for(int i=0; i<S; i++)
        free(cache[i]);
    
    // 再释放整个cache
    free(cache);
}


// 模拟访问缓存
void accessCache(uint64_t address){
    int tag = address>>(b+s);   // 获取tag
    uint64_t mask = ((1ULL << 63) - 1) >> (63 - s);
    CacheSet cacheSet = cache[(address >> b) & mask];

    // 缓存击中
    for(int i=0; i<E; i++){
        // 有效位为1且tag位一致则为击中
        if(cacheSet[i].valid && cacheSet[i].tag==tag){
            hit++;
            cacheSet[i].time = 0;
            return;
        }
    }

    // 缓存未击中
    miss++;

    // 有空位，直接写入
    for(int i=0; i<E; i++){
        if(!cacheSet[i].valid){
            cacheSet[i].valid = 1;
            cacheSet[i].tag = tag;
            cacheSet[i].time = 0;
            return;
        }
    }

    // 没有空位，使用LRU算法进行替换
    evict++;
    int idx=0, maxTime=0;
    for(int i=0; i<E; i++){
        if(cacheSet[i].time>maxTime){
            maxTime = cacheSet[i].time;
            idx = i;
        }
    }
    cacheSet[idx].tag = tag;
    cacheSet[idx].time = 0;
}


// 更新访问时间
void updateTime(){
    for(int i=0; i<S; i++){
        for(int j=0; j<E; j++){
            if(cache[i][j].valid)
                cache[i][j].time++;
        }
    }
}


// 模拟缓存的读写操作
void simulate(){
    // 打开文件
    FILE* file = fopen(filePath, "r");
    assert(file);
    char opt;           // 操作
    uint64_t address;   // 指定一个32位的十六进制存储器地址
    int size;           // 指定操作访问的字节数

    // 模拟访问操作，I和S需要访问1次，M需要访问2次
    while(fscanf(file, " %c %lx,%d", &opt, &address, &size) > 0){
        switch (opt){
            case 'M':
                accessCache(address);
            case 'L':
            case 'S':
                accessCache(address);
                break;
        }
        updateTime();   // 更新时间戳
    }

    // 关闭文件
    fclose(file);
}


int main(int argc, char* argv[])
{
    int opt;
    // getopt在循环中调用来检索参数，它的返回值存储在一个局部变量中
    while ((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch (opt) {
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(filePath, optarg);
                break;
        }
    }

    mallocCache();  // 动态分配缓存空间
    simulate();
    freeCache();    // 释放缓存空间
    printSummary(hit, miss, evict);
    return 0;
}