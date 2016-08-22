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
/*static*/ QRegExpValidator *HyGlobal::sm_pVector2dValidator = NULL;

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
    return "(" % QString::number(ptPoint.x(), 'g', 4) % "," % QString::number(ptPoint.y(), 'g', 4) % ")";
}

QPointF StringToPoint(QString sPoint)
{
    sPoint.replace(QString("("), QString(""));
    sPoint.replace(QString(")"), QString(""));
    QStringList sComponentList = sPoint.split(',');
    
    if(sComponentList.size() < 2)
        return QPointF(0.0f, 0.0f);
    else
        return QPointF(sComponentList[0].toFloat(), sComponentList[1].toFloat());
}
