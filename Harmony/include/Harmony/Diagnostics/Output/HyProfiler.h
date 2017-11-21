/**************************************************************************
*	HyProfiler.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyProfiler_h__
#define HyProfiler_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"

#include <time.h>

class HyProfiler : public HyEntity2d
{
	bool							m_bIsSetup;
	bool							m_bOpen;

	struct ProfileState
	{
		const char *				szName;
		clock_t						time;
		HyPrimitive2d				barSlice;

		std::vector<ProfileState>	childrenStates;

		ProfileState() : szName(nullptr), time(0)
		{ }
	};

	int32							m_iCurProfileStackIndex;	// Value of -1 indicates Start/End of one profile frame
	std::vector<ProfileState>		m_TopLevelProfileStateList;
	clock_t							m_TotalClockTicks;

	HyPrimitive2d					m_ProfileOutline;

public:
	HyProfiler();
	virtual ~HyProfiler();

	void ProfileBegin(const char *szName);
	void ProfileEnd();
};

#endif /* HyProfiler_h__ */
