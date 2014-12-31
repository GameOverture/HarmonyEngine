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
