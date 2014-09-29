/**************************************************************************
 *	HyWaitBase.cpp
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#include "HyWaitBase.h"

#include "Threading/HyThreadManager.h"

WaitObject::WaitObject(PCTSTR p_Name, bool p_bLockMT) : m_RefCounter(1), m_bLockMT(p_bLockMT)
{
	m_Id = HyThreadManager::Get()->CreateUniqueWaitObjectId();
	m_Name = p_Name;
	m_LockCount = 0;
	m_LockedByThreadId = 0;
	HyThreadManager::Get()->AddWaitObject(this);
}

WaitObject::~WaitObject()
{
	HyThreadManager::Get()->RemoveWaitObject(this);
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

HyString WaitObject::GetName()
{
	return m_Name;
}

HyString WaitObject::GetNameState()
{
	HyStringStream stream;
	stream << m_Name << _T(":") << m_Id << _T("(") << GetStateInfo() << _T(")");
	return stream.str();
}

HyString WaitObject::GetStateInfo()
{
	HyStringStream stream;
	if(IsLocked())
		stream << _T("locked: ") << m_LockCount << _T(" by ") << m_LockedByThreadId;
	else
		stream << _T("locked: ") << m_LockCount;
	return stream.str();
}

void WaitObject::SetStateWait(uint32 p_Timeout)
{
	// Set wait state, for details see description of ThreadWaitState and CWaitInfoArray
	ThreadInfo::WaitInfoArray& WaitObjectInfo = HyThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();
	if(WaitObjectInfo.GetSize() + (intx)STATIC_ELEMENT_COUNT + 1 <= WaitObjectInfo.GetMaxSize())
	{
		intx Pos = WaitObjectInfo.GetSize();
		WaitObjectInfo.SetAtGrow(Pos, (uint32)STATE_NOTHING);
		WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, HyGetTickCount());
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
	ThreadInfo::WaitInfoArray& WaitObjectInfo = HyThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

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
		HyAssert(m_LockCount == 0 || m_LockedByThreadId == InteropGetCurrentThreadId(), "WaitObject::SetState() no locks remaining or Locked by current thread");
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
		ThreadInfo::WaitInfoArray& WaitObjectInfo = HyThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

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
				WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, HyGetTickCount());
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
			WaitObjectInfo.SetAtGrow(Pos + OFFSET_TIMESTAMP, HyGetTickCount());
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
		HyAssert(m_LockCount > 0 && m_LockedByThreadId == InteropGetCurrentThreadId(), "WaitObject::RemoveStateLock() Out of locks, or locked by current thread");
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
		ThreadInfo::WaitInfoArray& WaitObjectInfo = HyThreadManager::Get()->GetCurrentThreadInfo()->GetWaitObjectInfo();

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

