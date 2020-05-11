/**************************************************************************
*	IHyThreadClass.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyThreadClass_h__
#define IHyThreadClass_h__

#include "Afx/HyStdAfx.h"

class IHyThreadClass
{
#ifdef HY_CONFIG_SINGLETHREAD
public:
	static std::vector<IHyThreadClass *>	sm_SingleThreadUpdater;
	static void SingleThreadUpdate() {
		for(auto fauxThread : IHyThreadClass::sm_SingleThreadUpdater)
			fauxThread->OnThreadUpdate();
	}
protected:
	struct HyNullMutex {
		void lock() { }
		void unlock() { }
		bool try_lock() { return true; }
	};
	HyNullMutex					m_Mutex;
#else
protected:
	std::thread					m_Thread;
	std::mutex					m_Mutex;

private:
	std::mutex					stateMutex;
	std::condition_variable		stateEvent;
#endif

private:
	HyThreadPriority			m_ePriority;
	enum ThreadState
	{
		THREADSTATE_Inactive = 0,
		THREADSTATE_Running,
		THREADSTATE_ShouldExit,
		THREADSTATE_HasExited
	};
	std::atomic<ThreadState>	m_eThreadState;

	const uint32				m_uiTHROTTLE_MS;
	bool						m_bWaitEnabled;
	bool						m_bWaitComplete;
	bool						m_bAutoResetWaiting;

public:
	IHyThreadClass(HyThreadPriority ePriority = HYTHREAD_Normal, uint32 uiUpdateThrottleMs = 0);
	virtual ~IHyThreadClass();

	bool ThreadStart();
	void ThreadWait();
	void ThreadContinue(bool bOnlyOneUpdate);
	void ThreadStop();

	bool IsThreadFinished();
	void ThreadJoin();

protected:
	virtual void OnThreadInit() = 0;
	virtual void OnThreadUpdate() = 0;
	virtual void OnThreadShutdown() = 0;

#ifndef HY_CONFIG_SINGLETHREAD
private:
	static void ThreadFunc(IHyThreadClass *pThis);
#endif
};

#endif /* IHyThreadClass_h__ */
