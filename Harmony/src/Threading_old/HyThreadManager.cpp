/**************************************************************************
 *	HyThreadManager.cpp
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#include "HyThreadManager.h"

#include "Misc/HyLock.h"

HyThreadManager::HyThreadManager()
{
	m_UniqueWaitObjectId = 1;

#if defined(HY_PLATFORM_UNIX)
	// init mutex
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int rc = pthread_mutex_init(&m_hTimeoutMutex, &attr);
	if(rc != 0)
	{
		pthread_mutexattr_destroy(&attr);
		THROW_UNIX_ERROR(rc);
	}
	rc = pthread_mutex_init(&m_hInterlockedMutex, &attr);
	if(rc != 0)
	{
		pthread_mutexattr_destroy(&attr);
		THROW_UNIX_ERROR(rc);
	}
	pthread_mutexattr_destroy(&attr);

	// init timeout condition
	rc = pthread_cond_init(&m_hTimeoutCond, 0);
	if(rc != 0)
		THROW_UNIX_ERROR(rc);
#endif
}

HyThreadManager::~HyThreadManager()
{
	// release remaining thread infos
	SLock Lock(m_SyncObj);
	for(ThreadInfoVector::iterator i = m_ThreadInfo.begin(); i != m_ThreadInfo.end(); ++i)
		(*i)->Release();
	Lock.Unlock();

#if defined(HY_PLATFORM_UNIX)
	pthread_mutex_destroy(&m_hTimeoutMutex);
	pthread_cond_destroy(&m_hTimeoutCond);
	pthread_mutex_destroy(&m_hInterlockedMutex);
#endif
}

ThreadInfoPtr HyThreadManager::BeginThread(LPCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam)
{
	SLock Lock(m_SyncObj);
	ThreadInfo* tmp = new ThreadInfo(p_ThreadName, p_pThreadProc, p_pParam);
	m_ThreadInfo.push_back(tmp);
	tmp->StartThread();
	return ThreadInfoPtrAddRef(tmp);
}

void HyThreadManager::RemoveThreadInfo(uint32 p_ThreadId)
{
	SLock Lock(m_SyncObj);
	for(ThreadInfoVector::iterator i = m_ThreadInfo.begin(); i != m_ThreadInfo.end(); ++i)
	{
		if((*i)->GetThreadId() == p_ThreadId)
		{
			HyAssert((*i)->IsCurrentThread() || (*i)->IsAlive(), "HyThreadManager::RemoveThreadInfo() failed");
			(*i)->Release();
			m_ThreadInfo.erase(i);
			m_TLSEntry.Set(NULL); // Remove thread info from TLS
			break;
		}
	}
}

void HyThreadManager::CleanupThreadInfo()
{
	SLock Lock(m_SyncObj);
	for(ThreadInfoVector::iterator i = m_ThreadInfo.begin(); i != m_ThreadInfo.end(); ++i)
	{
		if(!(*i)->IsAlive())
		{
			(*i)->Release();
			m_ThreadInfo.erase(i);
			--i;
		}
	}
}

ThreadInfo* HyThreadManager::GetCurrentThreadInfo()
{
	ThreadInfo* ptr = (ThreadInfo*)m_TLSEntry.Get();
	if(ptr)
		return(ptr);

	// get thread info from central manager and store in thread local storage
	ThreadInfoPtr handler = GetThreadInfo(InteropGetCurrentThreadId());
	m_TLSEntry.Set(handler.get());

	return handler.get();
}

void HyThreadManager::GetAllThreadInfos(std::vector<ThreadInfoPtr>& p_ThreadInfos)
{
	SLock Lock(m_SyncObj);
	p_ThreadInfos.reserve(m_ThreadInfo.size());
	for(size_t i = 0; i < m_ThreadInfo.size(); ++i)
	{
		p_ThreadInfos.push_back(ThreadInfoPtrAddRef(m_ThreadInfo[i]));
	}
}


uint32 HyThreadManager::CreateUniqueWaitObjectId()
{
	return((uint32)InteropInterlockedIncrement(m_UniqueWaitObjectId));
}

void HyThreadManager::AddWaitObject(WaitObject* p_pWaitObject)
{
	HyAssert(p_pWaitObject, "HyThreadManager::AddWaitObject() was passed invalid argument");
	SLock Lock(m_SyncObj);
	m_WaitObjects.push_back(p_pWaitObject);
}

void HyThreadManager::RemoveWaitObject(WaitObject* p_pWaitObject)
{
	HyAssert(p_pWaitObject, "HyThreadManager::RemoveWaitObject() was passed invalid argument");

	SLock Lock(m_SyncObj);
	for(WaitObjectBaseArray::iterator i = m_WaitObjects.begin(); i != m_WaitObjects.end(); ++i)
	{
		if((*i)->GetId() == p_pWaitObject->GetId())
		{
			m_WaitObjects.erase(i);
			return;
		}
	}
	HyAssert(false, _T("Invalid WaitObject!"));
}

bool HyThreadManager::HasThreadWaitObjectInfo()
{
	return (!GetCurrentThreadInfo()->GetWaitObjectInfo().IsEmpty());
}

HyString HyThreadManager::DumpThreadWaitObjectInfo()
{
	SLock Lock(m_SyncObj);
	return DumpThreadWaitObjectInfo(GetCurrentThreadInfo(), _T(""), HyGetTickCount());
}

HyString HyThreadManager::DumpProcessWaitObjectInfo(bool p_bSkipEmptyThreads)
{
	SLock Lock(m_SyncObj);
	HyStringStream DumpStream;
	DumpStream << _T("Thread WaitObject Information:\n");

	// generate wait object info for all threads
	uint32 CurrentTimestamp = HyGetTickCount();
	for(uintx i = 0; i < m_ThreadInfo.size(); ++i)
	{
		if(p_bSkipEmptyThreads && m_ThreadInfo[i]->GetWaitObjectInfo().IsEmpty())
			continue;
		DumpStream << DumpThreadWaitObjectInfo(m_ThreadInfo[i], _T(" "), CurrentTimestamp);
	}

	return DumpStream.str();
}


ThreadInfoPtr HyThreadManager::GetThreadInfo(uint32 p_ThreadId)
{
	SLock Lock(m_SyncObj);
	for(int i = 0; i < (int)m_ThreadInfo.size(); ++i)
	{
		if(m_ThreadInfo[i]->GetThreadId() == p_ThreadId)
		{
			return ThreadInfoPtrAddRef(m_ThreadInfo[i]);
		}
	}
	// thread info not found in list, create new one
	HyAssert(InteropGetCurrentThreadId() == p_ThreadId, "HyThreadManager::GetThreadInfo() Invalid argument");

	ThreadInfo* tmp = new ThreadInfo;
	m_ThreadInfo.push_back(tmp);
	return ThreadInfoPtrAddRef(tmp);
}

WaitObject* HyThreadManager::GetWaitObject(uint32 p_WaitObjectId)
{
	for(uintx i = 0; i < m_WaitObjects.size(); ++i)
		if(m_WaitObjects[i]->GetId() == p_WaitObjectId)
			return(m_WaitObjects[i]);
	return(NULL);
}

HyString HyThreadManager::DumpThreadWaitObjectInfo(ThreadInfo* p_pThreadInfo, PCTSTR p_LinePrefix, uint32 p_DumpTimestamp)
{
	HyStringStream DumpStream;
	DumpStream << p_LinePrefix << p_pThreadInfo->GetThreadId() <<  _T(" : ") << p_pThreadInfo->GetThreadName() << _T("\n");
	ThreadInfo::WaitInfoArray& WaitObjectInfo = p_pThreadInfo->GetWaitObjectInfo();
	intx InfoArraySize = WaitObjectInfo.GetSize();
	uint32* pInfoArray = WaitObjectInfo.GetData();
	int ThreadDumps = 0;
	intx iPos = 0;
	while(iPos < InfoArraySize)
	{
		if(pInfoArray[iPos] == STATE_LOCK)
		{
			// buffer overflow check
			if(iPos + (intx)STATIC_ELEMENT_COUNT + 1 > InfoArraySize)
				break;

			// dump locked information
			uint32 Timestamp = pInfoArray[iPos + OFFSET_TIMESTAMP];
			uint32 TimeDiff = p_DumpTimestamp - Timestamp;
			if(TimeDiff > 0x0FFFFFFF)
				TimeDiff = 0;
			uint32 WaitObjectId = pInfoArray[iPos + OFFSET_OBJECTID_0];
			WaitObject* pWaitObject = GetWaitObject(WaitObjectId);
			DumpStream << _T(" - Locked (") << TimeDiff << _T("ms) wait object ") << WaitObjectId << _T(" : ");
			if(pWaitObject != NULL)
				DumpStream << pWaitObject->GetName() << _T("\n");
			else
				DumpStream << _T("Unknown wait object\n");
			ThreadDumps++;
			iPos += STATIC_ELEMENT_COUNT + 1;
		}
		else if(pInfoArray[iPos] == STATE_WAIT)
		{
			// buffer overflow check
			if(iPos + (intx)STATIC_ELEMENT_COUNT > InfoArraySize)
				break;

			// dump waiting information
			uint32 Timestamp = pInfoArray[iPos + OFFSET_TIMESTAMP];
			uint32 TimeDiff = p_DumpTimestamp - Timestamp;
			if(TimeDiff > 0x0FFFFFFF)
				TimeDiff = 0;
			uint32 WaitTimeout = pInfoArray[iPos + OFFSET_TIMEOUT];
			uint32 NumWaitObjects = pInfoArray[iPos + OFFSET_OBJECT_COUNT];

			// buffer overflow check
			if(iPos + (intx)STATIC_ELEMENT_COUNT + (intx)NumWaitObjects > InfoArraySize)
				break;

			if(NumWaitObjects == 1)
			{
				// waiting for single object
				uint32 WaitObjectId = pInfoArray[iPos + OFFSET_OBJECTID_0];
				WaitObject* pWaitObject = GetWaitObject(WaitObjectId);

				DumpStream << p_LinePrefix << _T("- Waiting (") << TimeDiff << _T("ms/");
				if(WaitTimeout == INFINITE) DumpStream << _T("INFINITE)");
				else DumpStream << WaitTimeout << _T("ms)");
				DumpStream << _T(" for wait object ") << WaitObjectId << _T(" : ");

				if(pWaitObject != NULL) DumpStream << pWaitObject->GetName() << _T("\n");
				else DumpStream << _T("Unknown wait object\n");
			}
			else
			{
				// waiting for multiple objects
				DumpStream << p_LinePrefix << _T("- Waiting (") << TimeDiff << _T("ms/");
				if(WaitTimeout == INFINITE) DumpStream << _T("INFINITE)");
				else DumpStream << WaitTimeout << _T("ms)");
				DumpStream << _T(" for multiple wait objects (") << NumWaitObjects << _T(")\n");

				for(uint32 iWaitObject = 0; iWaitObject < NumWaitObjects; ++iWaitObject)
				{
					uint32 WaitObjectId = pInfoArray[iPos + OFFSET_OBJECTID_0 + iWaitObject];
					WaitObject* pWaitObject = GetWaitObject(WaitObjectId);
					DumpStream << p_LinePrefix << _T("  - ") << WaitObjectId << _T(" : ");
					if(pWaitObject != NULL) DumpStream << pWaitObject->GetName() << _T("\n");
					else DumpStream << _T("Unknown wait object\n");
				}
			}
			ThreadDumps++;
			iPos = iPos + (intx)STATIC_ELEMENT_COUNT + (intx)NumWaitObjects;
		}
		else
			break;
	}
	if(ThreadDumps == 0)
		DumpStream << _T(" - No Locks\n");
	return(DumpStream.str());
}
