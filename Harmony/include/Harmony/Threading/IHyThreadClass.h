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
	std::thread					m_Thread;

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
	std::mutex					stateMutex;
	std::condition_variable		stateEvent;

public:
	IHyThreadClass(uint32 uiUpdateThrottleMs = 0);	// In milliseconds
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

private:
	static void ThreadFunc(IHyThreadClass *pThis);
};

#endif /* IHyThreadClass_h__ */
