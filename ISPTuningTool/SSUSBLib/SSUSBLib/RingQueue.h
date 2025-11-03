#pragma once
#ifndef _WIN32
#include <mutex>
#endif
class CRingQueue
{
public:
	CRingQueue();
	CRingQueue(int ItemSize, int InitItemCount, int AutoIncrease);
	~CRingQueue();
private:
	int m_nFront;
	int m_nRear;
	int m_nCount;
	int m_nItemSize;
	int m_nAutoIncrease;
	unsigned char* m_pbyItemArray;
	int m_nItemArraySize;
#ifdef _WIN32
	CRITICAL_SECTION m_LockThis;
#else
	std::mutex m_mutexLock;
#endif
public:
	int GetCapacity();
	int IncreaseCapacity();
	int GetItemCount();
	bool IsEmpty();
	bool IsFull();
	int Clear();
	int PushBack(void* pItem);
	int PopFront(void *pItem);
	int GetFront(void *pItem);
};
#pragma once
