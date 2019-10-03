#include "coroutine_cpp.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <queue>

using namespace std;

struct QUERY_RES
{
    int co_id;
    int result;
};

struct args
{
    int n;
};

// 模拟消息队列
queue<QUERY_RES> query_res;

int get_sleep_time()
{
    return rand() % 10;
}

int db_req(int co_id)
{
    printf("send req, from %d \n", co_id);
    // 模拟同步发送请求,阻塞等待结果
    int sleep_time = get_sleep_time();
    sleep(sleep_time);

    // 模拟将结果写入消息队列，作为查询的结果
    QUERY_RES res;
    res.co_id = co_id;
    res.result = sleep_time;
    query_res.push(res);
    return 0;
}

void co_work(struct schedule *S, void *args)
{
    // 发送请求
    int co_id = coroutine_running(S);
    db_req(co_id);

    // yield 让出控制权
    coroutine_yield(S);

    // 被唤醒继续执行
    // 从队列拿出数据，打印结果，模拟对查询数据的处理
    QUERY_RES res = query_res.front();
    query_res.pop();
    printf("wake up co %d, get result %d \n", res.co_id, res.result);
}

int proc(schedule *S)
{
    static int sleep_flag = 0;

    // 模拟检查消息队列，如果消息队列里面有消息，那么唤醒对应的协程
    while (!query_res.empty())
    {
        QUERY_RES res = query_res.front();

        // 唤醒对应的协程
        int co_id = res.co_id;
        coroutine_resume(S, co_id);

        sleep_flag++;
    }

    // 避免死循环
    if (sleep_flag == 10)
    {
        sleep(1);
        sleep_flag = 0;
    }

    return 0;
}

int main()
{
    struct schedule *S = coroutine_open();

    // 这个没有用到
    struct args arg1 = {0};
    struct args arg2 = {100};

    int co1 = coroutine_new(S, co_work, &arg1);
    int co2 = coroutine_new(S, co_work, &arg2);
    printf("main start\n");

    coroutine_resume(S, co1);
    coroutine_resume(S, co2);

    proc(S);

    coroutine_close(S);
    return 0;
}