#pragma once
#include "RingQueue.h"
#ifndef _WIN32
#include <mutex>
#include <condition_variable>
#endif

typedef struct ItemInfo
{
	int         Offset;
	int         Size;
}ItemInfo_t;

class CBufferPool
{
public:
	CBufferPool();
	CBufferPool(void* pPoolAddr, int PoolSize, int count);
	~CBufferPool();
private:
	unsigned char* m_pPoolAddr;
	int m_nSelfAlloc;
	int m_nPoolSize;
	int m_nFrontOffset;
	int m_nRearOffset;
	ItemInfo_t m_CurFreeItem;
	ItemInfo_t m_CurDataItem;
	CRingQueue *m_pItemsCQHandle;
#ifdef _WIN32
	HANDLE  m_hSemThis;
#else
	std::condition_variable m_ConditionSem;
	std::mutex m_MutexSem;
#endif
	BOOL m_bInitSuccess;
	int m_nLockThis;
public:
	int GetItemCount();
	int Clear();
	int PushItemToBack(void * pFrameAddr, int FrameSize);
	int FetchFrontItem(void * pBuffAddr, int * pBuffSize, int Timeout);

	int LockBackFreeItem(void ** ppBuffAddr, int FrameSize);
	int UnlockBackFreeItemAndSave(int FrameSize);
	int UnlockBackFreeItem();

	int LockFrontItem(void ** ppFrameAddr, int * pFrameSize, int Timeout);
	int UnlockFrontItemAndDel();
	int UnlockFontItem();

#ifdef _WIN32
	CRITICAL_SECTION m_ncsLockFrontOff;
#else
	std::mutex m_MutexCriticalSection;
#endif
};

