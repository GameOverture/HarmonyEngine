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
	std::atomic<HyThreadState>	m_eThreadState;
	std::condition_variable		m_cv;
	std::mutex					m_cvMutex;

public:
	IHyThreadClass();
	virtual ~IHyThreadClass();

	bool ThreadStart();
	bool ThreadStop();

	bool IsThreadFinished();
	void ThreadJoin();

	virtual void OnThreadInit() = 0;
	virtual void OnThreadUpdate() = 0;
	virtual void OnThreadShutdown() = 0;

private:
	void ThreadFunc();
};

#endif /* IHyThreadClass_h__ */
