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
#include "EntityModel.h"
#include "Project.h"

EntityItemMimeData::EntityItemMimeData(ProjectItemData &entityRef, QList<EntityTreeItemData *> &itemListRef) :
	IMimeData(MIMETYPE_EntityItems)
{
	QJsonObject rootEntityObj;
	rootEntityObj.insert("project", entityRef.GetProject().GetAbsPath().toLower());
	rootEntityObj.insert("entityUuid", entityRef.GetUuid().toString(QUuid::WithoutBraces));

	QJsonArray itemArray; // An array of objects, that each contain an descObj and propArray. Each index in 'itemArray' corresponds to the item in 'itemListRef'
	for(int i = 0; i < itemListRef.size(); ++i)
	{
		QJsonObject itemObj;

		QJsonObject descObj;
		itemListRef[i]->InsertJsonInfo_Desc(descObj);
		itemObj.insert("descObj", descObj);

		QJsonArray propArray;
		for(int iStateIndex = 0; iStateIndex < entityRef.GetModel()->GetNumStates(); ++iStateIndex)
		{
			EntityStateData *pStateData = static_cast<EntityStateData *>(entityRef.GetModel()->GetStateData(iStateIndex));
			QJsonObject propObj = pStateData->GetPropertiesTreeModel(itemListRef[i])->SerializeJson();

			propArray.append(propObj);
		}
		itemObj.insert("propArray", propArray);
		
		itemArray.append(itemObj);
	}
	rootEntityObj.insert("itemArray", itemArray);

	// Serialize the item info into json source
	m_Data = JsonValueToSrc(QJsonValue(rootEntityObj));
	setData(HyGlobal::MimeTypeString(m_eMIME_TYPE), m_Data);
}

/*virtual*/ EntityItemMimeData::~EntityItemMimeData()
{ }
