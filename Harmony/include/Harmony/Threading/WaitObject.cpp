
#include "Threading.h"
#include "PlatformInterop.h"
#include "WaitObject.h"


//*****************************************************************************
//** WaitObject implementation
//*****************************************************************************

WaitObject::WaitObject(PCTSTR p_Name, bool p_bLockMT) : m_RefCounter(1), m_bLockMT(p_bLockMT)
{
	m_Id = ThreadManager::Get()->CreateUniqueWaitObjectId();
	m_Name = p_Name;
	m_LockCount = 0;
	m_LockedByThreadId = 0;
	ThreadManager::Get()->AddWaitObject(this);
}

WaitObject::~WaitObject()
{
	ThreadManager::Get()->RemoveWaitObject(this);
}

bool WaitObject::IsLocked()
{
	return (m_LockCount > 0);
}

// returns the active lock count on this object
uint32 WaitObject::GetLockCount()
{
	return m_LockCount;
}

// thread id of locking thread or zero
uint32 WaitObject::GetLockedByThreadId()
{
	return m_LockedByThreadId;
}

uint32 WaitObject::GetId()
{
	return m_Id;
}

tstring WaitObject::GetName()
{
	return m_Name;
}

tstring WaitObject::GetNameState()
{
	tostringstream stream;
	stream << m_Name << _T(":") << m_Id << _T("(") << GetStateInfo() << _T(")");
	return stream.str();
}

tstring WaitObject::GetStateInfo()
{
	tostringstream stream;
	if(IsLocked())
		stream << _T("locked: ") << m_LockCount << _T(" by ") << m_LockedByThreadId;
	else
		stream << _T("locked: ") << m_LockCount;
	return stream.str();
}

void WaitObject::SetStateWait(uint32 p_Timeout)
{
	// Set wait state, for details see description of ThreadWaitState and CWaitInfoArray
	ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();
	if(WaitObjectInfo.GetSize() + (intx)STATIC_ELEMENT_COUNT + 1 <= WaitObjectInfo.GetMaxSize())
	{
		intx Pos = WaitObjectInfo.GetSize();
		WaitObjectInfo.SetAtGrow(Pos, (uint32)STATE_NOTHING);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, InteropGetTickCount());
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMEOUT, p_Timeout);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECT_COUNT, 1);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0, m_Id);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_STATE_SIZE, STATIC_ELEMENT_COUNT + 1);
		// Set real state at the end to enable better multi threading behavior
		WaitObjectInfo[Pos] = STATE_WAIT;
	}
	//else
	//	TRACE(_T("Wait object info too small! Could not store new information"));
}

void WaitObject::RemoveStateWait()
{
	ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

	// Search our wait state
	intx iWaitPos = WaitObjectInfo.GetSize() - 1;
	uint32 WaitEntrySize = 0;
	while(iWaitPos >= 0)
	{
		WaitEntrySize = WaitObjectInfo[iWaitPos];
		iWaitPos = (iWaitPos - WaitEntrySize) + 1;
		if(iWaitPos >= 0 && WaitObjectInfo[iWaitPos] == STATE_WAIT && WaitObjectInfo[iWaitPos + OFFSET_OBJECT_COUNT] == 1 && WaitObjectInfo[iWaitPos + OFFSET_OBJECTID_0] == m_Id)
			break;
		iWaitPos--;
	}

	// Check if we have a pending wait state, in that case simply overwrite it
	if(iWaitPos >= 0)
	{
		if(iWaitPos + (intx)WaitEntrySize == WaitObjectInfo.GetSize())
		{
			// we are on top of the array
			WaitObjectInfo.SetAtArray(iWaitPos, WaitEntrySize, 0);
			WaitObjectInfo.SetSize(iWaitPos);
		}
		else
		{
			// we are in between the array
			WaitObjectInfo.RemoveAt(iWaitPos, WaitEntrySize);
			for(intx i = 0; i < (intx)WaitEntrySize; ++i)
				WaitObjectInfo.GetData()[WaitObjectInfo.GetSize() + i] = 0;
		}
	}
	//else
	//	TRACE(_T("Wait object info is invalid!"));
}

void WaitObject::SetStateLock(bool p_bUpdateWait)
{
	if(!m_bLockMT)
	{
		ASSERT_EXPR(m_LockCount == 0 || m_LockedByThreadId == InteropGetCurrentThreadId());
		m_LockedByThreadId = InteropGetCurrentThreadId();
		m_LockCount++;
	}
	else
	{
		InteropInterlockedIncrement(m_LockCount);
	}

	if(!m_bLockMT)
	{
		// Set wait state, for details see description of ThreadWaitState and CWaitInfoArray
		ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

		if(p_bUpdateWait)
		{
			// Search our wait state
			intx iWaitPos = WaitObjectInfo.GetSize() - 1;
			uint32 WaitEntrySize = 0;
			while(iWaitPos >= 0)
			{
				WaitEntrySize = WaitObjectInfo[iWaitPos];
				iWaitPos = (iWaitPos - WaitEntrySize) + 1;
				if(iWaitPos >= 0 && WaitObjectInfo[iWaitPos] == STATE_WAIT && WaitObjectInfo[iWaitPos + OFFSET_OBJECT_COUNT] == 1 && WaitObjectInfo[iWaitPos + OFFSET_OBJECTID_0] == m_Id)
					break;
				iWaitPos--;
			}

			// Check if we have a pending wait state, in that case simply overwrite it
			if(iWaitPos >= 0)
			{
				intx Pos = iWaitPos;
				WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, InteropGetTickCount());
				// Set real state at the end to enable better multi threading behavior
				WaitObjectInfo[Pos] = STATE_LOCK;
			}
			//else
			//	TRACE(_T("Wait object info is invalid!"));
		}
		else if(WaitObjectInfo.GetSize() + (intx)STATIC_ELEMENT_COUNT + 1 <= WaitObjectInfo.GetMaxSize())
		{
			intx Pos = WaitObjectInfo.GetSize();
			WaitObjectInfo.SetAtGrow(Pos, (uint32)STATE_NOTHING);
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, InteropGetTickCount());
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMEOUT, 0);
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECT_COUNT, 1);
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0, m_Id);
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_STATE_SIZE, STATIC_ELEMENT_COUNT + 1);
			// Set real state at the end to enable better multi threading behavior
			WaitObjectInfo[Pos] = STATE_LOCK;
		}
		//else
		//	TRACE(_T("Wait object info too small! Could not store new information"));
	}
	else if(p_bUpdateWait)
		RemoveStateWait();
}

void WaitObject::RemoveStateLock()
{
	if(!m_bLockMT)
	{
		ASSERT_EXPR(m_LockCount > 0 && m_LockedByThreadId == InteropGetCurrentThreadId());
		m_LockCount--;
		if(m_LockCount == 0)
			m_LockedByThreadId = 0;
	}
	else
	{
		InteropInterlockedDecrement(m_LockCount);
	}

	if(!m_bLockMT)
	{
		ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

		// Search our state
		intx iWaitPos = WaitObjectInfo.GetSize() - 1;
		uint32 WaitEntrySize = 0;
		while(iWaitPos >= 0)
		{
			WaitEntrySize = WaitObjectInfo[iWaitPos];
			iWaitPos = (iWaitPos - WaitEntrySize) + 1;
			if(iWaitPos >= 0 && WaitObjectInfo[iWaitPos] == STATE_LOCK && WaitObjectInfo[iWaitPos + OFFSET_OBJECT_COUNT] == 1 && WaitObjectInfo[iWaitPos + OFFSET_OBJECTID_0] == m_Id)
				break;
			iWaitPos--;
		}

		// Remove the state
		if(iWaitPos >= 0)
		{
			if(iWaitPos + (intx)WaitEntrySize == WaitObjectInfo.GetSize())
			{
				// we are on top of the array
				WaitObjectInfo.SetAtArray(iWaitPos, WaitEntrySize, 0);
				WaitObjectInfo.SetSize(iWaitPos);
			}
			else
			{
				// we are in between the array
				WaitObjectInfo.RemoveAt(iWaitPos, WaitEntrySize);
				for(intx i = 0; i < (intx)WaitEntrySize; ++i)
					WaitObjectInfo.GetData()[WaitObjectInfo.GetSize() + i] = 0;
			}
		}
		//else
		//	TRACE(_T("Wait object info is invalid!"));
	}
}


//*****************************************************************************
//** WaitList implementation
//*****************************************************************************

WaitList::WaitList()
{
}

WaitList::WaitList(WaitObject* p_WaitHandle1)
{
	AddHandle(p_WaitHandle1);
}

WaitList::WaitList(WaitObject&p_WaitHandle1)
{
	AddHandle(p_WaitHandle1);
}

WaitList::WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
}

WaitList::WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
}

WaitList::WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
}

WaitList::WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
}

WaitList::WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3, WaitObject* p_WaitHandle4)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
	AddHandle(p_WaitHandle4);
}

WaitList::WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3, WaitObject& p_WaitHandle4)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
	AddHandle(p_WaitHandle4);
}

WaitList::WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3, WaitObject* p_WaitHandle4, WaitObject* p_WaitHandle5)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
	AddHandle(p_WaitHandle4);
	AddHandle(p_WaitHandle5);
}

WaitList::WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3, WaitObject& p_WaitHandle4, WaitObject& p_WaitHandle5)
{
	AddHandle(p_WaitHandle1);
	AddHandle(p_WaitHandle2);
	AddHandle(p_WaitHandle3);
	AddHandle(p_WaitHandle4);
	AddHandle(p_WaitHandle5);
}

WaitList::~WaitList()
{
	// release reference to wait handles
	for(WaitObjectPtrArray::iterator i = m_WaitHandles.begin(); i != m_WaitHandles.end(); ++i)
	{
		if((*i))
			(*i)->Release();
	}
}

void WaitList::AddHandle(WaitObject* p_WaitHandle)
{
	ASSERT_EXPR(static_cast<uint32>(m_WaitHandles.size()) <= MAX_WAIT_OBJECTS);
	m_WaitHandles.push_back(p_WaitHandle);
#if defined(HY_PLATFORM_WINDOWS)
	m_RawHandles.push_back(p_WaitHandle->GetHandle());
#endif
	p_WaitHandle->AddRef();
}

void WaitList::AddHandle(WaitObject& p_WaitHandle)
{
	ASSERT_EXPR(static_cast<uint32>(m_WaitHandles.size()) <= MAX_WAIT_OBJECTS);
	m_WaitHandles.push_back(&p_WaitHandle);
#if defined(HY_PLATFORM_WINDOWS)
	m_RawHandles.push_back(p_WaitHandle.GetHandle());
#endif
	p_WaitHandle.AddRef();
}

void WaitList::AddHandle(SyncHandle p_WaitHandle)
{
#if defined(HY_PLATFORM_UNIX)
	THROW_ERROR_NOTSUPPORTED();
#else
	ASSERT_EXPR(m_WaitHandles.size() <= MAX_WAIT_OBJECTS);
	m_WaitHandles.push_back(NULL);
	m_RawHandles.push_back(p_WaitHandle);
#endif
}

void WaitList::RemoveHandle(intx p_Idx)
{
	CHECK_EXPR((size_t)p_Idx < m_WaitHandles.size());
	if(m_WaitHandles[p_Idx])
		m_WaitHandles[p_Idx]->Release();
	m_WaitHandles.erase(m_WaitHandles.begin() + p_Idx);
#if defined(HY_PLATFORM_WINDOWS)
	m_RawHandles.erase(m_RawHandles.begin() + p_Idx);
#endif
}

void WaitList::RemoveHandle(WaitObject* p_WaitHandle)
{
	CHECK_EXPR(p_WaitHandle != NULL);
	WaitObjectPtrArray::iterator idx = std::find(m_WaitHandles.begin(), m_WaitHandles.end(), p_WaitHandle);
	if(idx != m_WaitHandles.end())
		RemoveHandle(std::distance(m_WaitHandles.begin(), idx));
}

WaitObject* WaitList::GetWaitHandle(int p_Idx)
{
	CHECK_EXPR((size_t)p_Idx < m_WaitHandles.size());
	return m_WaitHandles[p_Idx];
}

SyncHandle WaitList::GetSyncHandle(int p_Idx)
{
#if defined(HY_PLATFORM_WINDOWS)
	CHECK_EXPR((size_t)p_Idx < m_RawHandles.size());
	return m_RawHandles[p_Idx];
#else
	THROW_ERROR_NOTSUPPORTED();
#endif
}


int WaitList::WaitForMultipleObjects()
{
	return WaitForMultipleObjects(TIMEOUT_INFINITE);
}

int WaitList::WaitForMultipleObjects(uint32 p_Timeout)
{
	SetStateWait(p_Timeout, -1);
#if defined(HY_PLATFORM_WINDOWS)
	int Res = (int)IGNORE_ABANDONED(::WaitForMultipleObjects((DWORD)m_RawHandles.size(), m_RawHandles.data(), FALSE, p_Timeout));
#elif defined(HY_PLATFORM_UNIX)
	int Res = WAIT_TIMEOUT_VALUE;

	// only try to lock non blocking at any timeout
	for(WaitObjectPtrArray::size_type i = 0; i < m_WaitHandles.size(); ++i)
	{
		ASSERT_EXPR(m_WaitHandles[i]);
		if(m_WaitHandles[i]->Lock(0))
		{
			Res = WAIT_OBJECT_0_VALUE + i;
			// now we need to remove the locking state of the locked wait object to keep the locking stack information up to date (cleanup wait info)
			m_WaitHandles[i]->UnlockedExtern();
			break;
		}
	}

	// if not object is locked, wait for it
	if(Res == WAIT_TIMEOUT_VALUE && p_Timeout > 0)
	{
		// lock global timeout mutex
		int rc = pthread_mutex_lock(GetTimeoutMutex());
		if(rc != 0)
		{
			RemoveStateWait(-1);
			THROW_UNIX_ERROR(rc);
		}

		// only try to lock non blocking
		for(WaitObjectPtrArray::size_type i = 0; i < m_WaitHandles.size(); ++i)
		{
			ASSERT_EXPR(m_WaitHandles[i]);
			if(m_WaitHandles[i]->Lock(0))
			{
				Res = WAIT_OBJECT_0_VALUE + i;
				rc = pthread_mutex_unlock(GetTimeoutMutex());
				if(rc != 0)
				{
					m_WaitHandles[i]->Unlock();
					RemoveStateWait(-1);
					THROW_UNIX_ERROR(rc);
				}
				// now we need to remove the locking state of the locked wait object to keep the locking stack information up to date (cleanup wait info)
				m_WaitHandles[i]->UnlockedExtern();
				break;
			}
		}

		if(Res == WAIT_TIMEOUT_VALUE && p_Timeout == INFINITE)
		{
			// wait for timeout or change broadcast
			while(1)
			{
				// mutex will be unlocked, waiting for condition
				rc = pthread_cond_wait(GetTimeoutCond(), GetTimeoutMutex());
				if(rc == 0)
				{
					// condition occured and mutex locked!
					// only try to lock non blocking
					for(WaitObjectPtrArray::size_type i = 0; i < m_WaitHandles.size(); ++i)
					{
						ASSERT_EXPR(m_WaitHandles[i]);
						if(m_WaitHandles[i]->Lock(0))
						{
							Res = WAIT_OBJECT_0_VALUE + i;
							rc = pthread_mutex_unlock(GetTimeoutMutex());
							if(rc != 0)
							{
								m_WaitHandles[i]->Unlock();
								RemoveStateWait(-1);
								THROW_UNIX_ERROR(rc);
							}
							// now we need to remove the locking state of the locked wait object to keep the locking stack information up to date (cleanup wait info)
							m_WaitHandles[i]->UnlockedExtern();
							break;
						}
					}
					if(Res != WAIT_TIMEOUT_VALUE)
					{
						// object locked, break wait
						break;
					}
					// object can not be locked, goto wait state
				}
				else
				{
					pthread_mutex_unlock(GetTimeoutMutex());
					RemoveStateWait(-1);
					THROW_UNIX_ERROR(rc);
				}
			}
		}
		else if(Res == WAIT_TIMEOUT_VALUE) // try to lock with specified timeout (when not already locked)
		{
			// get current time, add timeout and correct overflow
			timeval tp;
			timespec ts;
			if(gettimeofday(&tp, 0) != 0)
			{
				uint32 LastError = InteropGetLastError();
				pthread_mutex_unlock(GetTimeoutMutex());
				RemoveStateWait(-1);
				THROW_UNIX_ERROR(LastError);
			}
			ts.tv_sec = tp.tv_sec + (p_Timeout / 1000);
			ts.tv_nsec = (tp.tv_usec + ((p_Timeout % 1000) * 1000)) * 1000;
			if(ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000;
			}

			// wait for timeout or change broadcast
			while(1)
			{
				// mutex will be unlocked, waiting for condition
				int rc = pthread_cond_timedwait(GetTimeoutCond(), GetTimeoutMutex(), &ts);
				if(rc == 0 || rc == ETIMEDOUT)
				{
					// check for timeout
					bool bTimeout = (rc == ETIMEDOUT);

					// condition occured and mutex locked!
					// only try to lock non blocking
					for(WaitObjectPtrArray::size_type i = 0; i < m_WaitHandles.size(); ++i)
					{
						ASSERT_EXPR(m_WaitHandles[i]);
						if(m_WaitHandles[i]->Lock(0))
						{
							Res = WAIT_OBJECT_0_VALUE + i;
							rc = pthread_mutex_unlock(GetTimeoutMutex());
							if(rc != 0)
							{
								m_WaitHandles[i]->Unlock();
								RemoveStateWait(-1);
								THROW_UNIX_ERROR(rc);
							}
							// now we need to remove the locking state of the locked wait object to keep the locking stack information up to date (cleanup wait info)
							m_WaitHandles[i]->UnlockedExtern();
							break;
						}
					}
					if(Res == WAIT_TIMEOUT_VALUE && bTimeout)
					{
						// timeout occured, break
						rc = pthread_mutex_unlock(GetTimeoutMutex());
						if(rc != 0)
						{
							RemoveStateWait(-1);
							THROW_UNIX_ERROR(rc);
						}
						break;
					}
					else if(Res != WAIT_TIMEOUT_VALUE)
					{
						// object locked, break wait
						break;
					}
					// object can not be locked, goto wait state
				}
				else
				{
					pthread_mutex_unlock(GetTimeoutMutex());
					RemoveStateWait(-1);
					THROW_UNIX_ERROR(rc);
				}
			}
		}
	}
#endif
	// cleanup wait state
	RemoveStateWait(-1);
	// set currently locked object state information
	if(Res >= WAIT_OBJECT_0_VALUE && Res < (WAIT_OBJECT_0_VALUE + (int)m_WaitHandles.size()) && m_WaitHandles[Res] != NULL)
		m_WaitHandles[Res]->LockedExtern();
	return(Res);
}

bool WaitList::WaitForSingleObject(int p_WaitObject)
{
	return(WaitForSingleObject(p_WaitObject, TIMEOUT_INFINITE));
}

bool WaitList::WaitForSingleObject(int p_WaitObject, uint32 p_Timeout)
{
	CHECK_EXPR((size_t)p_WaitObject < m_WaitHandles.size());
	if(m_WaitHandles[p_WaitObject] != NULL)
		return(m_WaitHandles[p_WaitObject]->Lock());
	else
	{
#if defined(HY_PLATFORM_UNIX)
		// waiting for raw handles is not supported on Unix
		THROW_ERROR_NOTSUPPORTED();
		return false;

#else
		CHECK_EXPR(m_RawHandles[p_WaitObject] != NULL);
		SetStateWait(p_Timeout, p_WaitObject);
#if defined(HY_PLATFORM_WINDOWS)
		bool Res = IGNORE_ABANDONED(::WaitForSingleObject(m_RawHandles[p_WaitObject], p_Timeout)) == (DWORD)WAIT_OBJECT_0_VALUE;
#endif
		RemoveStateWait(p_WaitObject);
		return(Res);
#endif	// HY_PLATFORM_UNIX
	}
}

void WaitList::SetStateWait(uint32 p_Timeout, int p_WaitObject)
{
	int NumWaitObjects = 1;
	if(p_WaitObject == -1)
		NumWaitObjects = (int)m_WaitHandles.size();

	// set wait state, for details see description of ThreadWaitState and CWaitInfoArray
	ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();
	if((int)WaitObjectInfo.GetSize() + (int)STATIC_ELEMENT_COUNT + NumWaitObjects <= (int)WaitObjectInfo.GetMaxSize())
	{
		intx Pos = WaitObjectInfo.GetSize();
		WaitObjectInfo.SetAtGrow(Pos, (uint32)STATE_NOTHING);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, InteropGetTickCount());
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMEOUT, p_Timeout);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECT_COUNT, NumWaitObjects);
		if(p_WaitObject == -1)
		{
			for(size_t i = 0; i < m_WaitHandles.size(); ++i)
			{
				if(m_WaitHandles[i] != NULL)
					WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0 + i, m_WaitHandles[i]->GetId());
				else
					WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0 + i, 0);
			}
		}
		else
		{
			if(m_WaitHandles[p_WaitObject] != NULL)
				WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0, m_WaitHandles[p_WaitObject]->GetId());
			else
				WaitObjectInfo.SetAtGrow(Pos + OFFSET_OBJECTID_0, 0);
		}
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_STATE_SIZE + (NumWaitObjects - 1), STATIC_ELEMENT_COUNT + NumWaitObjects);
		// Set real state at the end to enable better multi threading behavior
		WaitObjectInfo[Pos] = STATE_WAIT;
	}
	//else
	//	TRACE(_T("Wait object info too small! Could not store new information"));
}

void WaitList::RemoveStateWait(int p_WaitObject)
{
	int NumWaitObjects = 1;
	if(p_WaitObject == -1)
		NumWaitObjects = (int)m_WaitHandles.size();

	ThreadInfo::WaitInfoArray& WaitObjectInfo = ThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();
	int InfoSize = (int)WaitObjectInfo.GetSize();
	int WaitEntrySize = STATIC_ELEMENT_COUNT + NumWaitObjects;
	int iWaitPos = InfoSize - WaitEntrySize;

	// Verify wait structure only in debug build
	ASSERT_EXPR(InfoSize > 0);
	ASSERT_EXPR(WaitObjectInfo[InfoSize - 1] == (uint32)WaitEntrySize);
	ASSERT_EXPR(WaitObjectInfo[iWaitPos] == STATE_WAIT);
	ASSERT_EXPR(WaitObjectInfo[iWaitPos + OFFSET_OBJECT_COUNT] == (uint32)NumWaitObjects);

	// Zero memory (if wait array is valid)
	if(iWaitPos >= 0 && WaitObjectInfo[InfoSize - 1] == (uint32)WaitEntrySize && WaitObjectInfo[iWaitPos] == STATE_WAIT && WaitObjectInfo[iWaitPos + OFFSET_OBJECT_COUNT] == (uint32)NumWaitObjects)
	{
		WaitObjectInfo.SetAtArray(iWaitPos, WaitEntrySize, 0);
		WaitObjectInfo.SetSize(iWaitPos);
	}
	//else
	//	TRACE(_T("Wait object info is invalid!"));
}


//*****************************************************************************
//** WaitMutex implementation
//*****************************************************************************

WaitMutex::WaitMutex(PCTSTR p_Name) : WaitObject(p_Name, false)
{
}

WaitMutex::WaitMutex(PCTSTR p_Name, bool p_bInitialOwner, bool p_bNamed) : WaitObject(p_Name, false),
	m_hObject(p_bInitialOwner, (p_bNamed ? p_Name : NULL))
{
}

WaitMutex::WaitMutex(PCTSTR p_Name, SyncHandleMutex p_hMutex, bool p_bCloseHandle) : WaitObject(p_Name, false),
	m_hObject(p_hMutex, p_bCloseHandle)
{
}

WaitMutex::~WaitMutex()
{
}

SyncHandle WaitMutex::GetHandle()
{
#if defined(HY_PLATFORM_UNIX)
	THROW_ERROR_NOTSUPPORTED();
	return 0;
#else
	return((SyncHandle)m_hObject.GetHandle());
#endif
}

bool WaitMutex::Lock()
{
	SetStateWait(INFINITE);
	if(m_hObject.Lock())
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

bool WaitMutex::Lock(uint32 p_Timeout)
{
	SetStateWait(p_Timeout);
	if(m_hObject.Lock(p_Timeout))
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

void WaitMutex::Unlock()
{
	RemoveStateLock();
	m_hObject.Unlock();
}

void WaitMutex::LockedExtern()
{
	SetStateLock(false);
}

void WaitMutex::UnlockedExtern()
{
	RemoveStateLock();
}



//*****************************************************************************
//** WaitSemaphore implementation
//*****************************************************************************

WaitSemaphore::WaitSemaphore(PCTSTR p_Name, uint32 p_InitialCount) : WaitObject(p_Name, true), m_hObject(p_InitialCount)
{
}

WaitSemaphore::WaitSemaphore(PCTSTR p_Name, uint32 p_InitialCount, bool p_bNamed) : WaitObject(p_Name, true),
	m_hObject(p_InitialCount, (p_bNamed ? p_Name : NULL))
{
}

WaitSemaphore::WaitSemaphore(PCTSTR p_Name, SyncHandleSemaphore p_hMutex, bool p_bCloseHandle) : WaitObject(p_Name, true),
	m_hObject(p_hMutex, p_bCloseHandle)
{
}

WaitSemaphore::~WaitSemaphore()
{
}

SyncHandle WaitSemaphore::GetHandle()
{
#if defined(HY_PLATFORM_UNIX)
	THROW_ERROR_NOTSUPPORTED();
	return 0;
#else
	return((SyncHandle)m_hObject.GetHandle());
#endif
}

bool WaitSemaphore::Lock()
{
	SetStateWait(INFINITE);
	if(m_hObject.Lock())
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

bool WaitSemaphore::Lock(uint32 p_Timeout)
{
	SetStateWait(p_Timeout);
	if(m_hObject.Lock(p_Timeout))
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

void WaitSemaphore::Unlock()
{
	RemoveStateLock();
	m_hObject.Unlock();
}

void WaitSemaphore::LockedExtern()
{
	SetStateLock(false);
}

void WaitSemaphore::UnlockedExtern()
{
	RemoveStateLock();
}



//*****************************************************************************
//** WaitEvent implementation
//*****************************************************************************

WaitEvent::WaitEvent(PCTSTR p_Name, bool p_bManualReset) : WaitObject(p_Name, false),
	m_hObject(p_bManualReset)
{
}

WaitEvent::WaitEvent(PCTSTR p_Name, bool p_bManualReset, bool p_bInitialOwner, bool p_bNamed) : WaitObject(p_Name, false),
	m_hObject(p_bManualReset, p_bInitialOwner, (p_bNamed ? p_Name : NULL))
{
}

WaitEvent::WaitEvent(PCTSTR p_Name, SyncHandleEvent p_hEvent, bool p_bCloseHandle) : WaitObject(p_Name, false),
	m_hObject(p_hEvent, p_bCloseHandle)
{
}

WaitEvent::~WaitEvent()
{
}

bool WaitEvent::Wait()
{
	SetStateWait(INFINITE);
	bool bRes = m_hObject.Wait();
	RemoveStateWait();
	return(bRes);
}

bool WaitEvent::Wait(uint32 p_Timeout)
{
	SetStateWait(p_Timeout);
	bool bRes = m_hObject.Wait(p_Timeout);
	RemoveStateWait();
	return(bRes);
}

void WaitEvent::Set()
{
	m_hObject.Set();
}

void WaitEvent::Reset()
{
	m_hObject.Reset();
}

SyncHandle WaitEvent::GetHandle()
{
#if defined(HY_PLATFORM_UNIX)
	THROW_ERROR_NOTSUPPORTED();
	return 0;
#else
	return((SyncHandle)m_hObject.GetHandle());
#endif
}

bool WaitEvent::Lock()
{
	return(Wait());
}

bool WaitEvent::Lock(uint32 p_Timeout)
{
	return(Wait(p_Timeout));
}

void WaitEvent::Unlock()
{
	THROW_ERROR_NOTSUPPORTED();
}

void WaitEvent::LockedExtern()
{
}

void WaitEvent::UnlockedExtern()
{
}



//*****************************************************************************
//** WaitSection implementation
//*****************************************************************************

WaitSection::WaitSection(PCTSTR p_Name) : WaitObject(p_Name, false)
{
}

WaitSection::~WaitSection()
{
}

SyncHandle WaitSection::GetHandle()
{
	THROW_ERROR_NOTSUPPORTED();
#if defined(HY_PLATFORM_UNIX)
	return(NULL);
#endif
}

bool WaitSection::Lock()
{
	SetStateWait(INFINITE);
	if(m_hObject.Lock())
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

bool WaitSection::Lock(uint32 p_Timeout)
{
	CHECK_EXPR(p_Timeout == INFINITE || p_Timeout == 0);
	SetStateWait(p_Timeout);
	if(m_hObject.Lock(p_Timeout))
	{
		SetStateLock(true);
		return(true);
	}
	RemoveStateWait();
	return(false);
}

void WaitSection::Unlock()
{
	RemoveStateLock();
	m_hObject.Unlock();
}

void WaitSection::LockedExtern()
{
	THROW_ERROR_NOTSUPPORTED();
}

void WaitSection::UnlockedExtern()
{
	THROW_ERROR_NOTSUPPORTED();
}

