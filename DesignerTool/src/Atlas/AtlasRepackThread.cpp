/**************************************************************************
 *	AtlasRepackThread.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
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
