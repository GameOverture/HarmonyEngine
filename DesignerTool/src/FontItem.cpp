/**************************************************************************
 *	ItemFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontItem.h"

#include <QMenu>
#include <QAction>
#include <QUndoView>
#include <QJsonObject>
#include <QJsonDocument>

#include "MainWindow.h"
#include "FontWidget.h"
#include "AtlasWidget.h"

#include "Harmony/HyEngine.h"

//FontItem::FontItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) :  ProjectItem(pItemProj, ITEM_Font, sPrefix, sName)
//{
//}

///*virtual*/ FontItem::~FontItem()
//{
//    delete m_pWidget;
//}

///*virtual*/ void FontItem::OnGiveMenuActions(QMenu *pMenu)
//{
//    static_cast<FontWidget *>(m_pWidget)->OnGiveMenuActions(pMenu);
//}

/////*virtual*/ void FontItem::OnLink(AtlasFrame *pFrame)
////{
////}


/////*virtual*/ void FontItem::OnUnlink(AtlasFrame *pFrame)
////{
////}

///*virtual*/ QJsonValue FontItem::OnSave()
//{
//    FontWidget *pWidget = static_cast<FontWidget *>(m_pWidget);
//    pWidget->SaveFontFilesToMetaDir();
    
//    pWidget->GeneratePreview(true);
    
//    QJsonObject fontObj;
//    pWidget->GetSaveInfo(fontObj);

//    return fontObj;
//}
