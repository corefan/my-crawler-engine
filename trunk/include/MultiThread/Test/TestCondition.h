#pragma once

#include "../Lock.hpp"


volatile int g_iTestOnlyOneThreadPickTask_TotakCount = 0;
const int TESTONLYONETHREADPICKTASK_THREADCOUNT = 10;
async::thread::AutoCriticalSection	g_Mutex;

template<class CONDITION> inline UINT __stdcall TestOnlyOneThreadPickTask_Run(void* pParam)
{
    CONDITION *pEventCondition = reinterpret_cast<CONDITION *>(pParam);
    async::thread::AutoLock<async::thread::AutoCriticalSection>	AutoLock(g_Mutex);
    pEventCondition->Wait(g_Mutex, INFINITE);
    for (UINT i = 0; i < TESTONLYONETHREADPICKTASK_THREADCOUNT; ++i)
    {
        printf("<%d>\r\n", i);
        ++g_iTestOnlyOneThreadPickTask_TotakCount;
        Sleep(100);
    }
    return 0;
}

template<class CONDITION> inline void TestOnlyOneThreadPickTask()
{
    printf("============任务唤醒测试开始============\r\n");
    CONDITION clEventCondition;
    const int C_THREAD_NUM = 2;
    HANDLE ahThread[C_THREAD_NUM];
    for (int i = 0; i < C_THREAD_NUM; ++i)
    {
        ahThread[i] = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, TestOnlyOneThreadPickTask_Run<CONDITION>, &clEventCondition, 0, NULL));
    }
    Sleep(200);
    g_iTestOnlyOneThreadPickTask_TotakCount = 0;
    clEventCondition.Signal();
    printf("激活一个线程\r\n");
    WaitForMultipleObjects(C_THREAD_NUM, ahThread, FALSE, INFINITE);
    printf("线程完成\r\n");
    int iCountA = g_iTestOnlyOneThreadPickTask_TotakCount;
    clEventCondition.Signal();
    printf("激活一个线程\r\n");
    WaitForMultipleObjects(C_THREAD_NUM, ahThread, TRUE, INFINITE);
    int iCountB = g_iTestOnlyOneThreadPickTask_TotakCount;
    printf("线程完成\r\n");
    for (int i = 0; i < C_THREAD_NUM; ++i)
    {
        CloseHandle(ahThread[i]);
    }
    assert(iCountA == TESTONLYONETHREADPICKTASK_THREADCOUNT && iCountB == TESTONLYONETHREADPICKTASK_THREADCOUNT * 2);
    printf("============任务唤醒测试完成============\r\n");
}

inline void TestCondition()
{
   // TestOnlyOneThreadPickTask<async::thread::SemaphoreCondition>();
    TestOnlyOneThreadPickTask<async::thread::EventCondition>();
}

