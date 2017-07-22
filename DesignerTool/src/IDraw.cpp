/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IDraw.h"
#include "ProjectItem.h"
#include "IModel.h"

#include <QJsonDocument>
#include <QJsonArray>

IDraw::IDraw(ProjectItem *pProjItem, IHyApplication &hyApp) :   m_pProjItem(pProjItem),
                                                                m_HyAppRef(hyApp),
                                                                m_pCamera(nullptr)
{
    m_pCamera = m_HyAppRef.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);
}

/*virtual*/ IDraw::~IDraw()
{
    m_HyAppRef.Window().RemoveCamera(m_pCamera);
}

void IDraw::ApplyJsonData(bool bReloadInAssetManager)
{
    if(m_pProjItem == nullptr)
        return;

    QJsonValue valueData = m_pProjItem->GetModel()->GetSaveInfo(false);
    QByteArray src;
    if(valueData.isArray())
    {
        QJsonDocument tmpDoc(valueData.toArray());
        src = tmpDoc.toJson();
    }
    else
    {
        QJsonDocument tmpDoc(valueData.toObject());
        src = tmpDoc.toJson();
    }

    jsonxx::Value newValue;
    newValue.parse(src.toStdString());

    OnApplyJsonData(newValue, bReloadInAssetManager);
}

void IDraw::Show()
{
    m_pCamera->SetEnabled(true);
    OnShow(m_HyAppRef);
}

void IDraw::Hide()
{
    m_pCamera->SetEnabled(false);
    OnHide(m_HyAppRef);
}
