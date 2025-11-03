#include "stdafx.h"
#include "BufferPool.h"
#include <stdlib.h>


CBufferPool::CBufferPool()
{
	m_pPoolAddr = NULL;
	m_nSelfAlloc = 0;
	m_nPoolSize = 0;
	m_nFrontOffset = 0;
	m_nRearOffset = 0;
	memset(&m_CurFreeItem, 0, sizeof(ItemInfo_t));
	memset(&m_CurDataItem, 0, sizeof(ItemInfo_t));
	m_pItemsCQHandle = NULL;
	m_nLockThis = 0;
	m_bInitSuccess = FALSE;
#ifdef _WIN32
	m_hSemThis = NULL;
#endif
}

CBufferPool::CBufferPool(void* pPoolAddr, int PoolSize, int count)
{
	int ret;

	ret = -1;

	m_pPoolAddr = NULL;
	m_nSelfAlloc = 0;
	m_nPoolSize = 0;
	m_nFrontOffset = 0;
	m_nRearOffset = 0;
	memset(&m_CurFreeItem, 0, sizeof(ItemInfo_t));
	memset(&m_CurDataItem, 0, sizeof(ItemInfo_t));
	m_pItemsCQHandle = NULL;
	m_nLockThis = 0;
	m_bInitSuccess = FALSE;
#ifdef _WIN32
	m_hSemThis = NULL;
	InitializeCriticalSection(&m_ncsLockFrontOff);
#endif
	do
	{
		if (pPoolAddr)
		{
			//if input pointer is not null, do not allocate memory
			m_nSelfAlloc = 0;
			m_pPoolAddr = (unsigned char *)pPoolAddr;
		}
		else
		{
			//if input pointer is null, allocate memory
			m_nSelfAlloc = 1;
			m_pPoolAddr = (unsigned char*)malloc(PoolSize);
			if (!m_pPoolAddr)
			{
				break;
			}
		}

		//initialize frame state
		m_nPoolSize = PoolSize;
		m_CurFreeItem.Offset = -1;
		m_CurDataItem.Offset = -1;

		//if frame maxim count is 0, create a ring queue with maxim count value of 30
		if (0 == count)
		{
			//create ring queue
			m_pItemsCQHandle = new CRingQueue(sizeof(ItemInfo_t), 30, 0);
		}
		else
		{
			//create ring queue
			m_pItemsCQHandle = new CRingQueue(sizeof(ItemInfo_t), count, 0);
		}

		//failed to create ring queue, break
		if (m_pItemsCQHandle == NULL)
		{
			break;
		}

#ifdef _WIN32
		//create semaphore
		m_hSemThis = ::CreateSemaphore(NULL, 0, count, NULL);
#endif
		ret = 0;
	} while (0);

	if (ret != 0)
	{
		if (m_nSelfAlloc && m_pPoolAddr)
		{
			free(m_pPoolAddr);
			m_pPoolAddr = NULL;
		}

		if (m_pItemsCQHandle)
		{
			delete m_pItemsCQHandle;
			m_pItemsCQHandle = NULL;
		}
		m_bInitSuccess = FALSE;
	}
	else
	{
		m_bInitSuccess = TRUE;
	}
}

CBufferPool::~CBufferPool()
{
 	Clear();

	if (m_nSelfAlloc && m_pPoolAddr)
	{
		delete[] m_pPoolAddr;
		m_pPoolAddr = NULL;
	}

	if (m_pItemsCQHandle)
	{
		delete m_pItemsCQHandle;
		m_pItemsCQHandle = NULL;
	}
#ifdef _WIN32
	if (m_hSemThis)
	{
		CloseHandle(m_hSemThis);
		m_hSemThis = NULL;
	}
#endif
}


int CBufferPool::GetItemCount()
{
	int ret = 0;

	if (!m_pItemsCQHandle || m_nLockThis)
	{
		return 0;
	}

	ret = m_pItemsCQHandle->GetItemCount();

	return ret;
}

int CBufferPool::Clear()
{
	int ret = 0;

	if (m_pItemsCQHandle == NULL)
	{
		return -1;
	}

	m_nLockThis = 1;

	m_nFrontOffset = 0;
	m_nRearOffset = 0;
	ret = m_pItemsCQHandle->Clear();

	m_nLockThis = 0;

	return ret;
}

int CBufferPool::PushItemToBack(void * pFrameAddr, int FrameSize)
{
	int ret = 0;
	void * pTempAddr;

	if (pFrameAddr == NULL || FrameSize <= 0)
	{
		return -1;
	}

	ret = LockBackFreeItem(&pTempAddr, FrameSize);
	if (ret == 0)
	{
		memcpy(pTempAddr, pFrameAddr, FrameSize);
		ret = UnlockBackFreeItemAndSave(FrameSize);
	}

	return ret;
}

int CBufferPool::FetchFrontItem(void * pBuffAddr, int * pBuffSize, int Timeout)
{
	int ret = 0;
	void * pTempAddr;
	int TempSize;

	if (pBuffAddr == NULL || pBuffSize == NULL)
	{
		return -1;
	}

	ret = LockFrontItem(&pTempAddr, &TempSize, Timeout);
	if (ret == 0)
	{
		if (*pBuffSize >= TempSize)
		{
			memcpy(pBuffAddr, pTempAddr, TempSize);
			*pBuffSize = TempSize;
		}
		else
		{
			ret = -2;
			*pBuffSize = 0;
		}
		UnlockFrontItemAndDel();
	}

	return ret;
}

int CBufferPool::LockBackFreeItem(void ** ppBuffAddr, int FrameSize)
{
	if (ppBuffAddr == NULL || m_pItemsCQHandle == NULL || m_nLockThis)
	{
		ppBuffAddr = NULL;
		return -1;
	}

	m_CurFreeItem.Offset = -1;

#ifdef _WIN32
	EnterCriticalSection(&m_ncsLockFrontOff);
#else
	std::unique_lock<std::mutex> lockCs(m_MutexCriticalSection);
#endif
 	if ((m_nRearOffset > m_nFrontOffset)
 		|| (m_nRearOffset == m_nFrontOffset && m_pItemsCQHandle->IsEmpty()))
 	{
 		if (m_nPoolSize - m_nRearOffset >= FrameSize)
 		{
 			m_CurFreeItem.Offset = m_nRearOffset;
 		}
 		else if (m_nFrontOffset >= FrameSize)
 		{
 			m_CurFreeItem.Offset = 0;
 		}
 	}
 	else if (m_nRearOffset < m_nFrontOffset)
 	{
 		if (m_nFrontOffset - m_nRearOffset >= FrameSize)
 		{
 			m_CurFreeItem.Offset = m_nRearOffset;
 		}
 	}
#ifdef _WIN32
	LeaveCriticalSection(&m_ncsLockFrontOff);
#else
	lockCs.unlock();
	lockCs.release();
#endif

	//no free back space valid
	if (m_CurFreeItem.Offset < 0)
	{
		ppBuffAddr = NULL;
		return -2;
	}

	m_CurFreeItem.Size = FrameSize;
	*ppBuffAddr = m_pPoolAddr + m_CurFreeItem.Offset;

	return 0;
}

int CBufferPool::UnlockBackFreeItemAndSave(int FrameSize)
{
	int ret = 0;

	if (m_pItemsCQHandle == NULL || m_nLockThis)
	{
		return -1;
	}

	//no free space valid
	if (m_CurFreeItem.Offset < 0 || m_CurFreeItem.Size < FrameSize)
	{
		return -2;
	}

#ifndef _WIN32
	m_ConditionSem.notify_one();
#endif
	ret = m_pItemsCQHandle->PushBack(&m_CurFreeItem);
	if (ret != 0)
	{
		return -3;
	}

 	m_nRearOffset = m_CurFreeItem.Offset + FrameSize;

	m_CurFreeItem.Offset = -1;
	m_CurFreeItem.Size = 0;

#ifdef _WIN32
	::ReleaseSemaphore(m_hSemThis, 1, NULL);
#endif
	return 0;
}

int CBufferPool::UnlockBackFreeItem()
{
	int ret = 0;

	if (m_nLockThis)
	{
		return -1;
	}

	if (m_CurFreeItem.Offset < 0)
	{
		return -2;
	}

	m_CurFreeItem.Offset = -1;
	m_CurFreeItem.Size = 0;

	return ret;
}

int CBufferPool::LockFrontItem(void ** ppFrameAddr, int * pFrameSize, int Timeout)
{
	int ret = 0;

	if (ppFrameAddr == NULL || pFrameSize == NULL || m_pItemsCQHandle == NULL || m_nLockThis)
	{
		return -1;
	}

	m_CurDataItem.Offset = -1;

#ifdef _WIN32
	if (WaitForSingleObject(m_hSemThis, Timeout) != WAIT_OBJECT_0)
	{
		return -2;
	}
 	::ReleaseSemaphore(m_hSemThis, 1, NULL);
#else
	if (m_pItemsCQHandle->IsEmpty())
	{
		std::unique_lock<std::mutex> lockSem(m_MutexSem);
		m_ConditionSem.wait_for(lockSem, std::chrono::milliseconds(Timeout));
		m_ConditionSem.notify_one();
	}
#endif
	if (m_pItemsCQHandle->IsEmpty())
	{
		return -2;
	}

	ret = m_pItemsCQHandle->GetFront(&m_CurDataItem);
	if (ret != 0)
	{
		m_CurDataItem.Offset = -1;
		return -3;
	}

	*ppFrameAddr = m_pPoolAddr + m_CurDataItem.Offset;
	*pFrameSize = m_CurDataItem.Size;

	return 0;
}

int CBufferPool::UnlockFrontItemAndDel()
{
	int ret = 0;

	if (!m_pItemsCQHandle || m_nLockThis)
	{
		return -1;
	}

	if (m_CurDataItem.Offset < 0)
	{
		return -2;
	}

	if (m_pItemsCQHandle->IsEmpty())
	{
		return -3;
	}

	ret = m_pItemsCQHandle->PopFront(&m_CurDataItem);
	if (ret != 0)
	{
		return -4;
	}

#ifdef _WIN32
	EnterCriticalSection(&m_ncsLockFrontOff);
	m_nFrontOffset = m_CurDataItem.Offset + m_CurDataItem.Size;
	LeaveCriticalSection(&m_ncsLockFrontOff);

	WaitForSingleObject(m_hSemThis, 0);
#else
	std::unique_lock<std::mutex> lockCs(m_MutexCriticalSection);
	m_nFrontOffset = m_CurDataItem.Offset + m_CurDataItem.Size;
	lockCs.unlock();
	lockCs.release();
	std::unique_lock<std::mutex> lockSem(m_MutexSem);
	m_ConditionSem.wait(lockSem);
#endif
	m_CurDataItem.Offset = -1;
	m_CurDataItem.Size = 0;

	return 0;
}

//unlock front data frame
int CBufferPool::UnlockFontItem()
{
	int ret = 0;

	if (m_nLockThis)
	{
		return -1;
	}

	if (m_CurDataItem.Offset < 0)
	{
		return -2;
	}

	m_CurDataItem.Offset = -1;
	m_CurDataItem.Size = 0;

	return ret;
}

