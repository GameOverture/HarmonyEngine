/**************************************************************************
*	EntityItemMimeData.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityItemMimeData.h"
#include "EntityTreeModel.h"
#include "Project.h"

EntityItemMimeData::EntityItemMimeData(ProjectItemData &entityRef, QList<EntityTreeItemData *> &itemListRef) :
	IMimeData(MIMETYPE_EntityItems)
{
	QJsonObject rootEntityObj;
	rootEntityObj.insert("project", entityRef.GetProject().GetAbsPath().toLower());
	rootEntityObj.insert("entityUuid", entityRef.GetUuid().toString(QUuid::WithoutBraces));

	QJsonArray itemListArray; // An array of EntityTreeItemData
	for(int i = 0; i < itemListRef.size(); ++i)
	{
		QJsonObject itemObj;
		itemListRef[i]->InsertJsonInfo(itemObj);
		
		itemListArray.append(itemObj);
	}
	rootEntityObj.insert("itemList", itemListArray);

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootEntityObj));
	setData(HyGlobal::MimeTypeString(m_eMIME_TYPE), m_Data);
}

/*virtual*/ EntityItemMimeData::~EntityItemMimeData()
{ }
