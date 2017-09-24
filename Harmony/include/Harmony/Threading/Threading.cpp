
#include "PlatformInterop.h"
#include "Threading.h"

//*****************************************************************************
//** ThreadInfo implementation
//*****************************************************************************

ThreadInfo::ThreadInfo() : m_RefCounter(1)
{
	// attach to current thread
#if defined(HY_PLATFORM_GUI_WIN)
	m_ThreadId = InteropGetCurrentThreadId();

	// try to open thread handle with different access rights, does not work in some restricted processes in Vista
	m_hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_ThreadId);
	if(!m_hThread)
		m_hThread = OpenThread(SYNCHRONIZE | THREAD_TERMINATE, FALSE, m_ThreadId);
	if(!m_hThread)
		m_hThread = OpenThread(SYNCHRONIZE, FALSE, m_ThreadId);

	// create synchronization object if we are able to open a thread handle
	if(m_hThread)
		m_hThreadEvent = HY_NEW BasicEvent(m_hThread, false);
	
#elif defined(HY_PLATFORM_UNIX)
	m_ThreadId = InteropGetCurrentThreadId();
	m_hThread = (ThreadHandle)m_ThreadId;
	m_hThreadEvent = HY_NEW BasicEvent(true);
#endif

#if defined(HY_PLATFORM_GUI_WIN) || defined(HY_PLATFORM_UNIX) || defined(HY_PLATFORM_OSX)
	SetThreadName(_T("Attached Thread"));
#endif

	m_bAttached = true;
	m_pThreadProc = NULL;
	m_pThreadParam = NULL;
	m_TriggerTime = InteropGetTickCount();
	m_bIgnoreTrigger = false;
}

ThreadInfo::ThreadInfo(PCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam) : m_RefCounter(1)
{
	m_ThreadId = 0;
	m_hThread = NULL;
	m_hThreadEvent = NULL;
	m_bAttached = false;
	m_ThreadName = p_ThreadName;
	m_pThreadProc = p_pThreadProc;
	m_pThreadParam = p_pParam;
	m_TriggerTime = InteropGetTickCount();
	m_bIgnoreTrigger = false;
}

ThreadInfo::~ThreadInfo()
{
	if(m_hThreadEvent != NULL)
		delete(m_hThreadEvent);

#if defined(HY_PLATFORM_GUI_WIN)
	if(m_hThread)
		CloseHandle(m_hThread);
#endif
}


uint32 ThreadInfo::GetThreadId()
{
	return(m_ThreadId);
}

ThreadHandle ThreadInfo::GetThreadHandle()
{
	CHECK_EXPR(m_hThread);
	return(m_hThread);
}

BasicEvent& ThreadInfo::GetThreadEvent()
{
	CHECK_EXPR(m_hThreadEvent);
	return(*m_hThreadEvent);
}

void ThreadInfo::SetThreadName(PCTSTR p_Name)
{
	ASSERT_EXPR(IsCurrentThread());
	if(!IsCurrentThread())
		return;

	// set thread name
	m_ThreadName = p_Name;

	// set debugger thread name
#if defined(UNICODE)
	Utils::SetDebugThreadName(GetThreadId(), Utils::WStringToString(m_ThreadName).c_str());
#else
	Utils::SetDebugThreadName(GetThreadId(), m_ThreadName.c_str());
#endif
}

tstring ThreadInfo::GetThreadName()
{
	return(m_ThreadName);
}


bool ThreadInfo::IsAlive()
{
#if defined(HY_PLATFORM_GUI_WIN)
	CHECK_EXPR(m_hThread);
	return(WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT);
#elif defined(HY_PLATFORM_UNIX)
	return(pthread_kill(m_hThread, 0) == 0);
#endif
}

bool ThreadInfo::IsAttached()
{
	return(m_bAttached);
}

bool ThreadInfo::IsCurrentThread()
{
	return(GetThreadId() == InteropGetCurrentThreadId());
}


void ThreadInfo::Abort()
{
#if defined(HY_PLATFORM_GUI_WIN)
	CHECK_EXPR(m_hThread);
	TerminateThread(m_hThread, 0);
#elif defined(HY_PLATFORM_UNIX)
	// INFO: SIGTERM signal will be sent to the thread, the signal handler of
	// this thread will be called and handles the thread termination
	pthread_kill(m_hThread, SIGTERM);
#endif
}

void ThreadInfo::WaitForThreadStop()
{
	CHECK_EXPR(!IsCurrentThread());
#if defined(HY_PLATFORM_GUI_WIN)
	CHECK_EXPR(m_hThread);
	WaitForSingleObject(m_hThread, INFINITE);
#elif defined(HY_PLATFORM_UNIX)
	GetThreadEvent().Wait();
#endif
}

bool ThreadInfo::WaitForThreadStop(uint32 p_TimeoutMs)
{
	CHECK_EXPR(!IsCurrentThread());
#if defined(HY_PLATFORM_GUI_WIN)
	CHECK_EXPR(m_hThread);
	return(WaitForSingleObject(m_hThread, p_TimeoutMs) == WAIT_OBJECT_0);
#elif defined(HY_PLATFORM_UNIX)
	return(GetThreadEvent().Wait(p_TimeoutMs));
#endif
}

void ThreadInfo::DetachThread()
{
	CHECK_EXPR(IsCurrentThread());
	ASSERT_EXPR(!m_bAttached);
	m_bAttached = true;
}

void ThreadInfo::StartThread()
{
	ASSERT_EXPR(!m_bAttached && m_ThreadId == 0 && m_hThread == NULL);

	// set one reference for this thread
	AddRef();

#if defined(HY_PLATFORM_GUI_WIN)
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, (LPDWORD)&m_ThreadId);
	if(!m_hThread)
	{
		// don't forget to release thread reference in case of an error!
		Release();
		THROW_ERROR_LASTERROR();
	}
	else
	{
		m_hThreadEvent = HY_NEW BasicEvent(m_hThread, false);
		if(::ResumeThread(m_hThread) == (DWORD)-1)
		{
			// don't forget to release thread reference in case of an error!
			Release();
			THROW_ERROR_LASTERROR();
		}
	}

#elif defined(HY_PLATFORM_UNIX)

	// create thread event
	m_hThreadEvent = HY_NEW BasicEvent(true);

	// thread attribute
	pthread_attr_t threadAttr;

	// initialize the thread attribute
	pthread_attr_init(&threadAttr);

	// set thread to detached state. no need for pthread_join, automatic cleanup at thread stop
	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	// create the thread
	m_ThreadId = 0;
	int rc = pthread_create(&m_hThread, &threadAttr, ThreadProc, this);
	if(rc != 0)
	{
		pthread_attr_destroy(&threadAttr);
		// don't forget to release thread reference in case of an error!
		Release();
		THROW_UNIX_ERROR(rc);
	}

	// destroy and cleanup the thread attributes
	pthread_attr_destroy(&threadAttr);

#endif
}

void ThreadInfo::ThreadStartup()
{
	// initialize thread instance
	ASSERT_EXPR(!m_bAttached);

#if defined(HY_PLATFORM_UNIX)
	// INFO: get thread id from thread instance, starting thread will do the same (race condition)
	m_ThreadId = InteropGetCurrentThreadId();
#endif

	SetThreadName(m_ThreadName.c_str());

	// thread started
	//LOG(GetThreadName(), _T("Run"));

	try
	{
		// start user thread function
		m_pThreadProc(m_pThreadParam);
	}
	catch(...)
	{
		// TODO: Add your error handling code here ...
		//TRACE(_T("User thread '") << GetThreadName() << _T("' terminated abnormal with an unhandled exception.\n"));
	}

	// check for any pending wait object operations
	if(ThreadManager::Get()->HasThreadWaitObjectInfo() || m_WaitObjectInfo.GetSize() > 0)
	{
		//LOGWARNING(_T("Thread has pending wait object operations!\n") << Global::GetThreadManager()->DumpThreadWaitObjectInfo());
#ifdef _DEBUG
		ASSERT_FAILED(_T("Thread has pending wait object operations!"));
		Utils::BeepErrorWaitObject();
#endif
	}

	// thread stopped
	//LOG(GetThreadName(), _T("Stop"));

	// unregister thread
	ThreadManager::Get()->RemoveThreadInfo(GetThreadId());

#if defined(HY_PLATFORM_UNIX)
	// trigger stop event
	if(m_hThreadEvent)
		m_hThreadEvent->Set();
#endif

	// release reference for this thread
	Release();
}


void ThreadInfo::TriggerThread()
{
	m_TriggerTime = InteropGetTickCount();
}

uint32 ThreadInfo::GetTriggerTime()
{
	return(m_TriggerTime);
}

uint32 ThreadInfo::GetTriggerTimeDiff()
{
	return (InteropGetTickCount() - GetTriggerTime());
}

bool ThreadInfo::IsTriggerTimeout(uint32 p_Timeout)
{
	return(!IgnoreTriggerTimeout() && GetTriggerTimeDiff() > p_Timeout);
}

bool ThreadInfo::IgnoreTriggerTimeout()
{
	return(m_bIgnoreTrigger);
}

void ThreadInfo::SetIgnoreTrigger(bool p_bValue)
{
	m_bIgnoreTrigger = p_bValue;
}

//*****************************************************************************
//** ThreadManager implementation
//*****************************************************************************

ThreadManager::ThreadManager()
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

ThreadManager::~ThreadManager()
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

ThreadInfoPtr ThreadManager::BeginThread(LPCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam)
{
	SLock Lock(m_SyncObj);
	ThreadInfo* tmp = HY_NEW ThreadInfo(p_ThreadName, p_pThreadProc, p_pParam);
	m_ThreadInfo.push_back(tmp);
	tmp->StartThread();
	return ThreadInfoPtrAddRef(tmp);
}

void ThreadManager::RemoveThreadInfo(uint32 p_ThreadId)
{
	SLock Lock(m_SyncObj);
	for(ThreadInfoVector::iterator i = m_ThreadInfo.begin(); i != m_ThreadInfo.end(); ++i)
	{
		if((*i)->GetThreadId() == p_ThreadId)
		{
			CHECK_EXPR((*i)->IsCurrentThread() || (*i)->IsAlive());
			(*i)->Release();
			m_ThreadInfo.erase(i);
			m_TLSEntry.Set(NULL); // Remove thread info from TLS
			break;
		}
	}
}

void ThreadManager::CleanupThreadInfo()
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

ThreadInfo* ThreadManager::GetCurrentThreadInfo()
{
	ThreadInfo* ptr = (ThreadInfo*)m_TLSEntry.Get();
	if(ptr)
		return(ptr);

	// get thread info from central manager and store in thread local storage
	ThreadInfoPtr handler = GetThreadInfo(InteropGetCurrentThreadId());
	m_TLSEntry.Set(handler.get()); 

	return handler.get();
}

void ThreadManager::GetAllThreadInfos(std::vector<ThreadInfoPtr>& p_ThreadInfos)
{
	SLock Lock(m_SyncObj);
	p_ThreadInfos.reserve(m_ThreadInfo.size());
	for(size_t i = 0; i < m_ThreadInfo.size(); ++i)
	{
		p_ThreadInfos.push_back(ThreadInfoPtrAddRef(m_ThreadInfo[i]));
	}
}


uint32 ThreadManager::CreateUniqueWaitObjectId()
{
	return((uint32)InteropInterlockedIncrement(m_UniqueWaitObjectId));
}

void ThreadManager::AddWaitObject(WaitObject* p_pWaitObject)
{
	CHECK_EXPR(p_pWaitObject);
	SLock Lock(m_SyncObj);
	m_WaitObjects.push_back(p_pWaitObject);
}

void ThreadManager::RemoveWaitObject(WaitObject* p_pWaitObject)
{
	CHECK_EXPR(p_pWaitObject);
	SLock Lock(m_SyncObj);
	for(WaitObjectBaseArray::iterator i = m_WaitObjects.begin(); i != m_WaitObjects.end(); ++i)
	{
		if((*i)->GetId() == p_pWaitObject->GetId())
		{
			m_WaitObjects.erase(i);
			return;
		}
	}
	ASSERT_FAILED(_T("Invalid WaitObject!"));
}

bool ThreadManager::HasThreadWaitObjectInfo()
{
	return (!GetCurrentThreadInfo()->GetWaitObjectInfo().IsEmpty());
}

tstring ThreadManager::DumpThreadWaitObjectInfo()
{
	SLock Lock(m_SyncObj);
	return DumpThreadWaitObjectInfo(GetCurrentThreadInfo(), _T(""), InteropGetTickCount());
}

tstring ThreadManager::DumpProcessWaitObjectInfo(bool p_bSkipEmptyThreads)
{
	SLock Lock(m_SyncObj);
	tostringstream DumpStream;
	DumpStream << _T("Thread WaitObject Information:\n");

	// generate wait object info for all threads
	uint32 CurrentTimestamp = InteropGetTickCount();
	for(uintx i = 0; i < m_ThreadInfo.size(); ++i)
	{
		if(p_bSkipEmptyThreads && m_ThreadInfo[i]->GetWaitObjectInfo().IsEmpty())
			continue;
		DumpStream << DumpThreadWaitObjectInfo(m_ThreadInfo[i], _T(" "), CurrentTimestamp);
	}

	return DumpStream.str();
}


ThreadInfoPtr ThreadManager::GetThreadInfo(uint32 p_ThreadId)
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
	if(InteropGetCurrentThreadId() != p_ThreadId)
		THROW_ERROR_INVALIDARGUMENT();
	ThreadInfo* tmp = HY_NEW ThreadInfo;
	m_ThreadInfo.push_back(tmp);
	return ThreadInfoPtrAddRef(tmp);
}

WaitObject* ThreadManager::GetWaitObject(uint32 p_WaitObjectId)
{
	for(uintx i = 0; i < m_WaitObjects.size(); ++i)
		if(m_WaitObjects[i]->GetId() == p_WaitObjectId)
			return(m_WaitObjects[i]);
	return(NULL);
}

tstring ThreadManager::DumpThreadWaitObjectInfo(ThreadInfo* p_pThreadInfo, PCTSTR p_LinePrefix, uint32 p_DumpTimestamp)
{
	tostringstream DumpStream;
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

