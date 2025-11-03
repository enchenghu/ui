#include "stdafx.h"
#include "RingQueue.h"


CRingQueue::CRingQueue()
{
	m_nItemSize = 0;
	m_nAutoIncrease = 0;
	m_nFront = 0;
	m_nRear = 0;
	m_nCount = 0;
	m_nItemSize = 0;
	m_nAutoIncrease = 0;
	m_pbyItemArray = NULL;
	m_nItemArraySize = 0;
}

CRingQueue::CRingQueue(int ItemSize, int InitItemCount, int AutoIncrease)
{
	int ret;
	ret = -1;

	m_nItemSize = 0;
	m_nAutoIncrease = 0;
	m_nFront = 0;
	m_nRear = 0;
	m_nCount = 0;
	m_nItemSize = 0;
	m_nAutoIncrease = 0;
	m_pbyItemArray = NULL;
	m_nItemArraySize = 0;

	do
	{
		//set ring queue value
		m_nItemSize = ItemSize;
		m_nAutoIncrease = AutoIncrease;
		m_nItemArraySize = InitItemCount - 1;//safety if leave last buffer when ringqueue is full
		m_pbyItemArray = new unsigned char[m_nItemSize * m_nItemArraySize];
		if (m_pbyItemArray == NULL)
		{
			break;
		}
#ifdef _WIN32
		InitializeCriticalSection(&m_LockThis);
#endif
		ret = 0;
	} while (0);

	if (ret != 0)
	{
		if (m_pbyItemArray)
		{
			delete[] m_pbyItemArray;
			m_pbyItemArray = NULL;
		}
	}
}

CRingQueue::~CRingQueue()
{
	if (m_pbyItemArray)
	{
		delete[] m_pbyItemArray;
		m_pbyItemArray = NULL;
	}

#ifdef _WIN32
	DeleteCriticalSection(&m_LockThis);
#endif
}

int CRingQueue::GetCapacity()
{
#ifdef _WIN32
	int ret = 0;
	EnterCriticalSection(&m_LockThis);
	ret = m_nItemArraySize;
	LeaveCriticalSection(&m_LockThis);
	return ret;
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
	return m_nItemArraySize;
#endif
}

int CRingQueue::IncreaseCapacity()
{
	int ret;
	int NewSize;
	unsigned char * NewArray;

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	//calculate new queue size
	NewSize = m_nItemArraySize * 2;
	//allocate memory
	NewArray = new unsigned char[m_nItemSize * NewSize];
	if (NewArray != NULL)
	{
		ret = 0;

		if (m_nCount > 0)
		{
			if (m_nRear > m_nFront)
			{
				memcpy(NewArray, m_pbyItemArray + m_nItemSize * m_nFront, (m_nRear - m_nFront) * m_nItemSize);
			}
			else
			{
				memcpy(NewArray, m_pbyItemArray + m_nItemSize * m_nFront, (m_nItemArraySize - m_nFront) * m_nItemSize);
				if (m_nRear > 0)
				{
					memcpy(NewArray + m_nItemSize * (m_nItemArraySize - m_nFront), m_pbyItemArray, m_nRear * m_nItemSize);
				}
			}
		}
		//free old memory
		if (m_pbyItemArray != NULL)
		{
			delete[] m_pbyItemArray;
			m_pbyItemArray = NULL;
		}
		m_pbyItemArray = NewArray;
		m_nItemArraySize = NewSize;
		m_nFront = 0;
		m_nRear = m_nCount%m_nItemArraySize;
	}
	else
	{
		ret = -2;
	}

#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif
	return ret;
}

int CRingQueue::GetItemCount()
{
	int ret;

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	ret = m_nCount;
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return ret;
}

bool CRingQueue::IsEmpty()
{
	bool ret;

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	ret = (m_nCount == 0);
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif
	return ret;
}

bool CRingQueue::IsFull()
{
	bool ret;

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	ret = (m_nCount == m_nItemArraySize);
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return ret;
}

int CRingQueue::Clear()
{
#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif

	m_nCount = 0;
	m_nFront = 0;
	m_nRear = 0;
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return 0;
}

int CRingQueue::PushBack(void* pItem)
{
	int ret;

	if (!pItem)
	{
		return -1;
	}

	if (IsFull())
	{
		if (m_nAutoIncrease)
		{
			ret = IncreaseCapacity();
			if (ret != 0)
			{
				return ret;
			}
		}
		else
		{
			return -3;
		}
	}

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	//memcpy(m_pbyItemArray + m_nItemSize * m_nRear, pItem, m_nItemSize);
	memcpy_s(m_pbyItemArray + m_nItemSize * m_nRear, m_nItemSize, pItem, m_nItemSize);
	m_nRear = (m_nRear + 1) % m_nItemArraySize;
	m_nCount++;
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return 0;
}

int CRingQueue::PopFront(void *pItem)
{
	if (!pItem)
	{
		return -1;
	}

	if (IsEmpty())
	{
		return -4;
	}

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	//memcpy(pItem, m_pbyItemArray + m_nItemSize * m_nFront, m_nItemSize);
	memcpy_s(pItem, m_nItemSize, m_pbyItemArray + m_nItemSize * m_nFront, m_nItemSize);
	m_nFront = (m_nFront + 1) % m_nItemArraySize;
	m_nCount--;
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return 0;
}

int CRingQueue::GetFront(void *pItem)
{
	if (!pItem)
	{
		return -1;
	}

	if (IsEmpty())
	{
		return -4;
	}

#ifdef _WIN32
	EnterCriticalSection(&m_LockThis);
#else
	std::unique_lock<std::mutex> lock(m_mutexLock);
#endif
	//memcpy(pItem, m_pbyItemArray + m_nItemSize * m_nFront, m_nItemSize);
	memcpy_s(pItem, m_nItemSize, m_pbyItemArray + m_nItemSize * m_nFront, m_nItemSize);
#ifdef _WIN32
	LeaveCriticalSection(&m_LockThis);
#endif

	return 0;
}


