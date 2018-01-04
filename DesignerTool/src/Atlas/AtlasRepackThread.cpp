/**************************************************************************
 *	AtlasRepackThread.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasRepackThread.h"

AtlasRepackThread::AtlasRepackThread()
{
}

/*virtual*/ AtlasRepackThread::~AtlasRepackThread()
{
}


/*virtual*/ void AtlasRepackThread::run() /*override*/
{
//    while(m_pCurrentRenderer && m_pCurrentRenderer->IsLoading())
//    { }

	Q_EMIT RepackIsFinished();
}
