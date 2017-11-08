/**************************************************************************
 *	AtlasRepackThread.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASREPACKTHREAD_H
#define ATLASREPACKTHREAD_H

#include <QThread>

class AtlasRepackThread : public QThread
{
    Q_OBJECT

public:
    AtlasRepackThread();
    virtual ~AtlasRepackThread();

    virtual void run() override;

Q_SIGNALS:
    void RepackIsFinished();
};

#endif // ATLASREPACKTHREAD_H
