/**************************************************************************
 *	HyGlobal.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGlobal.h"

/*static*/ QString HyGlobal::sm_sItemNames[NUMITEM];
/*static*/ QString HyGlobal::sm_sItemExt[NUMITEM];
/*static*/ QIcon HyGlobal::sm_ItemIcons[NUMITEM];
/*static*/ QIcon HyGlobal::sm_AtlasIcons[NUMATLAS];

/*static*/ QRegExpValidator *HyGlobal::sm_pFileNameValidator = NULL;
/*static*/ QRegExpValidator *HyGlobal::sm_pFilePathValidator = NULL;

QAction *FindAction(QList<QAction *> list, QString sName)
{
    foreach(QAction *pAction, list)
    {
        if(pAction->objectName() == sName)
            return pAction;
    }
    
    return NULL;
}

char *QStringToCharPtr(QString sString)
{
    QByteArray ba;
    ba.append(sString);
    return ba.data();
}

QString PointToQString(QPointF ptPoint)
{
    return QString::number(ptPoint.x(), 'g', 2) % ", " % QString::number(ptPoint.y(), 'g', 2);
}
