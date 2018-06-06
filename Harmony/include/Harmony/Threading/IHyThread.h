/**************************************************************************
*	IHyThread.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyThread_h__
#define IHyThread_h__

#include "Afx/HyStdAfx.h"

class IHyThread
{
	std::thread					m_Thread;
	std::atomic<HyThreadState>	m_eThreadState;
	std::condition_variable		m_cv;
	std::mutex					m_cvMutex;

public:
	IHyThread();
	virtual ~IHyThread();

	//void ThreadUpdate
};

#endif /* IHyThread_h__ */
