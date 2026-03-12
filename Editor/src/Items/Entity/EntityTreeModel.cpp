/**************************************************************************
*	EntityTreeModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityTreeModel.h"
#include "EntityModel.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "MainWindow.h"
#include "EntityUndoCmds.h"
#include "GfxPrimitiveModel.h"
#include "GfxShapeModel.h"
#include "GfxChainModel.h"
#include "ContainerEditModel.h"

#include <QVariant>
#include <QStack>

EntityTreeModel::EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QJsonObject fileMetaObj, QObject *pParent /*= nullptr*/) :
	ITreeModel(NUMCOLUMNS, QStringList(), pParent),
	m_ModelRef(modelRef),
	m_GuiLayout(fileMetaObj["guiLayout"].toObject())
{
	// Insert self as root node
	if(insertRow(0, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	m_pRootTreeItemData = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sEntityCodeName, ITEM_Entity, ENTTYPE_Root, QUuid(fileMetaObj["UUID"].toString()), QUuid(fileMetaObj["UUID"].toString()));
	QVariant rootData;
	rootData.setValue(m_pRootTreeItemData);
	QModelIndex entRootIndex = index(0, 0, QModelIndex());
	if(setData(entRootIndex, rootData, Qt::UserRole) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - setData on root failed", LOGTYPE_Error);
		return;
	}

	// Insert 'folder' to hold bounding volumes (shapes)
	if(insertRow(1, QModelIndex()) == false)
	{
		HyGuiLog("EntityTreeModel::EntityTreeModel() - insertRow failed", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pFixtureFolderItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, "Fixtures", ITEM_None, ENTTYPE_FixtureFolder, QUuid(), QUuid());
	QVariant fixtureData;
	fixtureData.setValue(pFixtureFolderItem);
	if(setData(index(1, 0, QModelIndex()), fixtureData, Qt::UserRole) == false)
		HyGuiLog("EntityTreeModel::EntityTreeModel() - setData failed", LOGTYPE_Error);

	// Then use "fusedItemList" to init each base class's fused tree item data
	QJsonArray fusedItemArray = fileMetaObj["fusedItemList"].toArray();
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
	{
		switch(i)
		{
		case ENTBASECLASS_HyEntity2d:
			m_FusedTreeItemData[i] = nullptr;
			break;
	
		case ENTBASECLASS_HyActor2d:
			if(fusedItemArray.empty() || i <= fusedItemArray.size())
				m_FusedTreeItemData[i] = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, "m_ActorFixture", ITEM_ShapeFixture, ENTTYPE_FusedItem, QUuid(), QUuid::createUuid());
			else
				m_FusedTreeItemData[i] = new EntityTreeItemData(m_ModelRef, fusedItemArray[i].toObject(), false, true);
			break;

		case ENTBASECLASS_HyGui:
			if(fusedItemArray.empty() || i <= fusedItemArray.size())
				m_FusedTreeItemData[i] = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, "m_RootLayout", ITEM_UiLayout, ENTTYPE_FusedItem, QUuid(), QUuid::createUuid());
			else
				m_FusedTreeItemData[i] = new EntityTreeItemData(m_ModelRef, fusedItemArray[i].toObject(), false, true);
			break;
		}
	}

	if(m_GuiLayout.IsValid() == false)
		m_GuiLayout.m_Uuid = m_FusedTreeItemData[ENTBASECLASS_HyGui]->GetThisUuid();

	// Insert all the 'children' and 'shape' items
	std::function<void(const QJsonArray &)> fpPopulateNodeTreeItems = [&](const QJsonArray &itemListArray)
	{
		for(int i = 0; i < itemListArray.size(); ++i)
		{
			if(itemListArray[i].isObject())
				m_ModelRef.Cmd_AddExistingItem(itemListArray[i].toObject(), false, false, i);
			else if(itemListArray[i].isArray())
			{
				QJsonArray subItemArray = itemListArray[i].toArray();
				for(int j = 0; j < subItemArray.size(); ++j)
					m_ModelRef.Cmd_AddExistingItem(subItemArray[j].toObject(), true, false, j == 0 ? i : j); // TODO: Confirm why [0] uses index i
			}
			else
				HyGuiLog("EntityTreeModel::EntityTreeModel invalid JSON type", LOGTYPE_Error);
		}
	};
	// Insert all the 'child' items into the nodeTree
	fpPopulateNodeTreeItems(fileMetaObj["descChildList"].toArray());
	fpPopulateNodeTreeItems(fileMetaObj["descShapeList"].toArray());
}

/*virtual*/ EntityTreeModel::~EntityTreeModel()
{
}

TreeModelItem *EntityTreeModel::GetRootTreeItem() const
{
	if(m_pRootItem->GetNumChildren() == 0)
		return nullptr;

	return m_pRootItem->GetChild(0);
}

EntityTreeItemData *EntityTreeModel::GetRootTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() == 0)
	{
		HyGuiLog("EntityTreeModel::GetRootTreeItemData - root data not created", LOGTYPE_Error);
		return nullptr;
	}

	return m_pRootTreeItemData;
}

TreeModelItem *EntityTreeModel::GetFixtureFolderTreeItem() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1);
}

EntityTreeItemData *EntityTreeModel::GetFixtureFolderTreeItemData() const
{
	if(m_pRootItem->GetNumChildren() < 2)
		return nullptr;

	return m_pRootItem->GetChild(1)->data(0).value<EntityTreeItemData *>();
}

QList<EntityTreeItemData *> EntityTreeModel::GetAllFusedItemData() const
{
	QList<EntityTreeItemData *> fusedItemList;
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
		fusedItemList.push_back(m_FusedTreeItemData[i]);
	
	return fusedItemList;
}

TreeModelItem *EntityTreeModel::GetArrayFolderTreeItem(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItem was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = pArrayItem->IsFixtureItem() ? GetFixtureFolderTreeItem() : GetRootTreeItem();
	for(int i = 0; i < pParentFolderItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentFolderItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pArrayItem->GetCodeName() == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
			return pParentFolderItem->GetChild(i);
	}

	HyGuiLog("EntityTreeModel::GetArrayFolderTreeItem array folder was not found", LOGTYPE_Error);
	return nullptr;
}

EntityTreeItemData *EntityTreeModel::GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const
{
	if(pArrayItem->GetEntType() != ENTTYPE_ArrayItem)
	{
		HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData was passed a non-array item", LOGTYPE_Error);
		return nullptr;
	}

	TreeModelItem *pParentFolderItem = pArrayItem->IsFixtureItem() ? GetFixtureFolderTreeItem() : GetRootTreeItem();
	for(int i = 0; i < pParentFolderItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentFolderItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pArrayItem->GetCodeName() == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
			return pSubItem;
	}

	HyGuiLog("EntityTreeModel::GetArrayFolderTreeItemData array folder was not found", LOGTYPE_Error);
	return nullptr;
}

void EntityTreeModel::InsertGuiItem(QUuid uuidParent, QJsonObject guiItemObj)
{
	if(uuidParent.isNull())
	{
		HyGuiLog("EntityModel::InsertGuiItem - Attempting to insert a widget/layout item into the GUI heirarchy with a null parent", LOGTYPE_Error);
		return;
	}

	// Find the parent item in the GUI heirarchy (m_GuiLayout), and insert this item as a child to the appropriate GuiItem within m_GuiLayout.
	std::function<void(EntityTreeModel::GuiItem &)> fpInsertFunc =
		[&](EntityTreeModel::GuiItem &curItem)
		{
			if(curItem.m_Uuid == uuidParent)
			{
				curItem.m_ChildList.removeOne(QUuid(guiItemObj["uuid"].toString()));
				curItem.m_ChildList.push_back(EntityTreeModel::GuiItem(guiItemObj));
				return;
			}
			for(GuiItem &child : curItem.m_ChildList)
				fpInsertFunc(child);
		};
	fpInsertFunc(m_GuiLayout);
}

void EntityTreeModel::PopGuiItem(EntityTreeItemData *pItem)
{
	if(m_GuiLayout.m_Uuid == pItem->GetThisUuid())
	{
		HyGuiLog("EntityModel::PopGuiItem - Attempting to pop the root GUI item, which is not allowed", LOGTYPE_Error);
		return;
	}

	QJsonObject poppedObj;
	std::function<void(EntityTreeModel::GuiItem &)> fpPopFunc =
		[&](EntityTreeModel::GuiItem &curItem)
		{
			for(int i = 0; i < curItem.m_ChildList.size(); ++i)
			{
				if(curItem.m_ChildList[i].m_Uuid == pItem->GetThisUuid())
				{
					poppedObj = curItem.m_ChildList[i].Serialize();
					curItem.m_ChildList.removeAt(i);
					return;
				}
				fpPopFunc(curItem.m_ChildList[i]);
			}
		};
	fpPopFunc(m_GuiLayout);

	m_PoppedGuiItemsMap.insert(pItem, poppedObj);
}

QUuid EntityTreeModel::FindGuiLayoutFromItemUuid(QUuid itemUuid) const
{
	// Search through m_GuiLayout for the GuiItem that equals 'itemUuid' and set it to pCurItem
	const GuiItem *pCurItem = nullptr;
	std::function<void(const EntityTreeModel::GuiItem &)> fpFindFunc =
		[&](const EntityTreeModel::GuiItem &curItem)
		{
			if(curItem == itemUuid)
				pCurItem = &curItem;
			for(const GuiItem &child : curItem.m_ChildList)
				fpFindFunc(child);
		};
	fpFindFunc(m_GuiLayout);

	if(pCurItem == nullptr)
		return QUuid();

	// Now search upward from pCurItem
	while(pCurItem)
	{
		EntityTreeItemData *pTreeItem = FindTreeItemData(pCurItem->m_Uuid);
		if(pTreeItem->GetType() == ITEM_UiLayout)
			return pTreeItem->GetThisUuid(); // Found the layout!

		const GuiItem *pParentItem = nullptr;
		std::function<void(const EntityTreeModel::GuiItem &)> fpFindParentFunc =
			[&](const EntityTreeModel::GuiItem &curItem)
			{
				for(const GuiItem &child : curItem.m_ChildList)
				{
					if(child == pCurItem->m_Uuid)
					{
						pParentItem = &curItem;
						return;
					}
					fpFindParentFunc(child);
				}
			};
		fpFindParentFunc(m_GuiLayout);

		pCurItem = pParentItem;
	}

	return QUuid();
}

void EntityTreeModel::AssembleGuiLayoutHeirarchy()
{
	EntityTreeItemData *pRootLayoutItem = GetAllFusedItemData()[ENTBASECLASS_HyGui];
	if(pRootLayoutItem == nullptr)
	{
		HyGuiLog("EntityTreeModel::AssembleGuiLayoutHeirarchy - Root layout item was not found, cannot assemble GUI heirarchy", LOGTYPE_Error);
		return;
	}

	// 'pRootLayoutItem' == m_GuiLayout. 
}

void EntityTreeModel::GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const
{
	TreeModelItem *pThisEntity = GetRootTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;

		if(pCurItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisEntity->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				childListOut.push_back(pArrayItem);
			}
		}
		else
			childListOut.push_back(pCurItem);
	}

	TreeModelItem *pThisFixturesFolder = GetFixtureFolderTreeItem();
	for(int i = 0; i < pThisFixturesFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurFixture = pThisFixturesFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurFixture == nullptr)
			continue;

		if(pCurFixture->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisFixturesFolder->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				fixtureListOut.push_back(pArrayItem);
			}
		}
		else
			fixtureListOut.push_back(pCurFixture);
	}
}

void EntityTreeModel::GetSelectedTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const
{
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> fixtureList;
	GetTreeItemData(childList, fixtureList);
	for(EntityTreeItemData *pChildItem : childList)
	{
		if(pChildItem->IsSelected())
			childListOut.push_back(pChildItem);
	}
	for(EntityTreeItemData *pFixtureItem : fixtureList)
	{
		if(pFixtureItem->IsSelected())
			fixtureListOut.push_back(pFixtureItem);
	}
}

EntityTreeItemData *EntityTreeModel::FindTreeItemData(QUuid uuid) const
{
	EntityTreeItemData *pRootItemData = GetRootTreeItemData();
	if(pRootItemData && pRootItemData->GetUuid() == uuid)
		return pRootItemData;

	TreeModelItem *pThisEntity = GetRootTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;

		if(pCurItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisEntity->GetChild(i);
			for(int i = 0; i < pArrayFolder->GetNumChildren(); ++i)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
				if(pArrayItem->GetThisUuid() == uuid)
					return pArrayItem;
			}
		}

		if(pCurItem->GetThisUuid() == uuid)
			return pCurItem;
	}

	TreeModelItem *pThisFixtureFolder = GetFixtureFolderTreeItem();
	for(int i = 0; i < pThisFixtureFolder->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurShape = pThisFixtureFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurShape == nullptr)
			continue;

		if(pCurShape->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisFixtureFolder->GetChild(i);
			for(int i = 0; i < pArrayFolder->GetNumChildren(); ++i)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(i)->data(0).value<EntityTreeItemData *>();
				if(pArrayItem->GetThisUuid() == uuid)
					return pArrayItem;
			}
		}

		if(pCurShape->GetThisUuid() == uuid)
			return pCurShape;
	}

	return nullptr;
}

int EntityTreeModel::GetPrimLayerIndex(EntityTreeItemData *pPrimLayer, EntityTreeItemData *&pPrimNodeOut) const
{
	if(pPrimLayer == nullptr || pPrimLayer->GetType() != ITEM_PrimLayer || pPrimLayer->GetReferencedItemUuid().isNull())
	{
		HyGuiLog("EntityTreeModel::GetPrimLayerIndex was passed an invalid PrimLayer item", LOGTYPE_Error);
		return -1;
	}

	// Find the PrimNode that is the parent of the passed in pPrimLayer
	QUuid primNodeUuid = pPrimLayer->GetReferencedItemUuid();
	pPrimNodeOut = nullptr;

	TreeModelItem *pThisEntity = GetRootTreeItem();
	for(int i = 0; i < pThisEntity->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pThisEntity->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;
		if(pCurItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			TreeModelItem *pArrayFolder = pThisEntity->GetChild(i);
			for(int j = 0; j < pArrayFolder->GetNumChildren(); ++j)
			{
				EntityTreeItemData *pArrayItem = pArrayFolder->GetChild(j)->data(0).value<EntityTreeItemData *>();
				if(pArrayItem->GetReferencedItemUuid() == primNodeUuid)
				{
					pPrimNodeOut = pArrayItem;
					break;
				}
			}

		}
		if(pCurItem->GetReferencedItemUuid() == primNodeUuid)
			pPrimNodeOut = pCurItem;

		if(pPrimNodeOut)
			break;
	}

	if(pPrimNodeOut == nullptr)
	{
		HyGuiLog("EntityTreeModel::GetPrimLayerIndex could not find parent PrimNode for the passed in PrimLayer", LOGTYPE_Error);
		return -1;
	}

	// Look through the PrimNode's children to find the index of the passed in PrimLayer
	TreeModelItem *pPrimNodeTreeItem = GetItem(FindIndex<EntityTreeItemData *>(pPrimNodeOut, 0));
	for(int i = 0; i < pPrimNodeTreeItem->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pCurItem = pPrimNodeTreeItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(pCurItem == nullptr)
			continue;
		if(pCurItem == pPrimLayer)
			return i;
	}

	return -1;
}

bool EntityTreeModel::IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const
{
	if(pItem == nullptr)
	{
		if(bShowDialogsOnFail)
			HyGuiLog("Entity tried to add a null item", LOGTYPE_Error);
		return false;
	}

	if(HyGlobal::IsItemType_Project(pItem->GetType()) == false && HyGlobal::IsItemType_Asset(pItem->GetType()) == false)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a valid child type: " % QString::number(pItem->GetType()), LOGTYPE_Warning);
		return false;
	}

	// Make sure the Project of 'pItem' is the same as this project
	if(m_ModelRef.GetItem().GetProject().FindItemData(pItem->GetUuid()) == nullptr)
	{
		if(bShowDialogsOnFail)
			HyGuiLog(pItem->GetText() % " is not a apart of this project", LOGTYPE_Warning);
		return false;
	}

	// Make sure pItem isn't or does not contain itself
	if(pItem->GetType() == ITEM_Entity)
	{
		ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pItem);

		FileDataPair entFileDataPair;
		pProjItem->GetLatestFileData(entFileDataPair);

		const QUuid &thisUuid = m_ModelRef.GetItem().GetUuid();
		if(thisUuid == QUuid(entFileDataPair.m_Meta["UUID"].toString()))
		{
			if(bShowDialogsOnFail)
				HyGuiLog(pItem->GetText() % " cannot insert a child node of itself", LOGTYPE_Warning);
			return false;
		}

		// Exhaustively look through all children (and children's children) to make sure this entity isn't a child of itself
		QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
		QStack<QJsonObject> childObjStack;
		for(int i = 0; i < descChildList.size(); ++i)
		{
			if(descChildList[i].isObject())
				childObjStack.push_back(descChildList[i].toObject());
			else // QJsonArray
			{
				QJsonArray childArray = descChildList[i].toArray();
				for(int j = 0; j < childArray.size(); ++j)
					childObjStack.push_back(childArray[j].toObject());
			}

			while(childObjStack.empty() == false)
			{
				QJsonObject childObj = childObjStack.pop();
				QUuid childUuid(childObj["itemUUID"].toString());
				if(thisUuid == childUuid)
				{
					if(bShowDialogsOnFail)
						HyGuiLog(pItem->GetText() % " cannot insert an entity that cointains a child node of this", LOGTYPE_Warning);
					return false;
				}

				if(childObj["itemType"].toString() == HyGlobal::ItemName(ITEM_Entity, false))
				{
					ProjectItemData *pEntTreeItem = static_cast<ProjectItemData *>(m_ModelRef.GetItem().GetProject().FindItemData(childUuid));
					pEntTreeItem->GetLatestFileData(entFileDataPair);
					QJsonArray nestedChildList = entFileDataPair.m_Meta["descChildList"].toArray();
					for(int j = 0; j < nestedChildList.size(); ++j)
					{
						childObjStack.push_back(nestedChildList[j].toObject());
					}
				}
			}
		}
	}

	return true;
}

QJsonObject EntityTreeModel::SerializeGuiLayout() const
{
	return m_GuiLayout.Serialize();
}

// (Re)inserts fuse items into the item tree model. It will also disassemble/assemble the GUI layout heirarchy
void EntityTreeModel::Cmd_ResetFusedItems()
{
	// Disassemble the GUI layout heirarchy if it exists. Move any widget items to be in the regular list of children
	QModelIndex layoutRootIndex = FindIndex<EntityTreeItemData *>(m_FusedTreeItemData[ENTBASECLASS_HyGui], 0);
	if(layoutRootIndex.isValid())
	{
		std::function<void(const QModelIndex &)> fpDestructGuiLayoutHeirarchy = 
			[&](const QModelIndex &parentIndex)
			{
				TreeModelItem *pParentItem = GetItem(parentIndex);
				for(int i = 0; i < pParentItem->GetNumChildren(); ++i)
				{
					QModelIndex childIndex = index(i, 0, parentIndex);
					EntityTreeItemData *pGuiItemData = pParentItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
					if(pGuiItemData == nullptr || pGuiItemData->GetType() == ITEM_UiSpacer)
						continue;
					if(pGuiItemData->IsWidgetItem())
						MoveTreeItem(pGuiItemData, GetRootTreeItemData(), 0);
					else // Is ITEM_UiLayout, recursively disassemble its heirarchy
						fpDestructGuiLayoutHeirarchy(childIndex);
				}
			};
		fpDestructGuiLayoutHeirarchy(layoutRootIndex);
	}

	// Remove/replace any fused items
	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
		RemoveTreeItem(m_FusedTreeItemData[i]);
	if(m_FusedTreeItemData[m_ModelRef.GetBaseClassType()])
	{
		TreeModelItem *pParentTreeItem = (m_FusedTreeItemData[m_ModelRef.GetBaseClassType()]->IsFixtureItem()) ? GetFixtureFolderTreeItem() : GetRootTreeItem();
		InsertTreeItem(m_ModelRef.GetItem().GetProject(), m_FusedTreeItemData[m_ModelRef.GetBaseClassType()], pParentTreeItem, 0);

		if(m_ModelRef.GetBaseClassType() == ENTBASECLASS_HyGui)
		{
			// Reassemble the GUI layout heirarchy. Move any widget items that are children of the GetRootTreeItem() to be children of the appropriate layout item based on 'm_GuiLayout'
			QModelIndex newLayoutRootIndex = FindIndex<EntityTreeItemData *>(m_FusedTreeItemData[ENTBASECLASS_HyGui], 0);
			if(newLayoutRootIndex.isValid() == false)
				HyGuiLog("EntityTreeModel::Cmd_ResetFusedItems - could not find root layout item after reinserting fused items", LOGTYPE_Error);

			TreeModelItem *pEntRootTreeItem = GetRootTreeItem();
			// Insert all layouts and spacers first to construct the proper heirarchy, then insert all widgets
			std::function<void(const GuiItem &, const QModelIndex &)> fpConstructGuiLayoutHeirarchy =
				[&](const GuiItem &curGuiItem, const QModelIndex &parentIndex)
				{
					for(const GuiItem &child : curGuiItem.m_ChildList)
					{
						EntityTreeItemData *pEntTreeItemData = FindTreeItemData(child.m_Uuid);
						if(pEntTreeItemData == nullptr)
						{
							HyGuiLog("EntityTreeModel::Cmd_ResetFusedItems - could not find tree item data for a GUI item in the layout heirarchy", LOGTYPE_Error);
							continue;
						}
						
						if(pEntTreeItemData->GetType() == ITEM_UiLayout || pEntTreeItemData->GetType() == ITEM_UiSpacer)
						{
							InsertTreeItem(m_ModelRef.GetItem().GetProject(), pEntTreeItemData, GetItem(parentIndex), 0);
							fpConstructGuiLayoutHeirarchy(child, FindIndex<EntityTreeItemData *>(pEntTreeItemData, 0));
						}
					}
				};
			fpConstructGuiLayoutHeirarchy(m_GuiLayout, newLayoutRootIndex);

			// Now insert the widgets
			for(int i = 0; i < pEntRootTreeItem->GetNumChildren(); ++i)
			{
				EntityTreeItemData *pGuiItemData = pEntRootTreeItem->GetChild(i)->data(0).value<EntityTreeItemData *>();
				if(pGuiItemData->IsWidgetItem())
				{
					EntityTreeItemData *pGuiLayoutParent = FindTreeItemData(FindGuiLayoutFromItemUuid(pGuiItemData->GetThisUuid()));
					if(pGuiLayoutParent)
						MoveTreeItem(pGuiItemData, pGuiLayoutParent, 0);
				}
			}
		}
	}
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocChildTreeItem(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pProjItem->GetName(false));

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, pProjItem->GetType(), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	//// If pProjItem is a sub-entity, also create tree items for all of its children
	//if(pProjItem->GetType() == ITEM_Entity)
	//{
	//	TreeModelItem *pCurParent = GetItem(FindIndex<EntityTreeItemData *>(pNewItem, 0));

	//	FileDataPair entFileDataPair;
	//	pProjItem->GetLatestFileData(entFileDataPair);
	//	QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
	//	for(int i = 0; i < descChildList.size(); ++i)
	//	{
	//		if(descChildList[i].isObject())
	//		{
	//			QJsonObject childObj = descChildList[i].toObject();
	//		}
	//		else
	//		{
	//			QJsonArray childArray = descChildList[i].toArray();
	//			for(int j = 0; j < childArray.size(); ++j)
	//			{
	//				QJsonObject childObj = childArray[j].toObject();
	//			}
	//		}

	//		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef,
	//															  HyGlobal::GetEntityDeclType(childObj["declarationType"].toString()),
	//															  childObj["codeName"].toString(),
	//															  HyGlobal::GetTypeFromString(childObj["itemType"].toString()), ENTTYPE_Item, pProjItem->GetUuid(), QUuid::createUuid());
	//		InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pCurParent, iRow);
	//	}

	//	QStack<ProjectItemData *> projItemStack;
	//	for(ProjectItemData *pProjItem : projItemList)
	//		projItemStack.push_back(pProjItem);

	//	while(projItemStack.empty() == false)
	//	{
	//		ProjectItemData *pProjItem = projItemStack.pop();

	//		if(pProjItem->GetType() == ITEM_Entity)
	//		{
	//			// Exhaustively add all sub-entity children (and children's children)
	//			pProjItem->GetLatestFileData(entFileDataPair);

	//			QJsonArray descChildList = entFileDataPair.m_Meta["descChildList"].toArray();
	//			for(int i = 0; i < descChildList.size(); ++i)
	//			{
	//				QUuid nestedChildUuid(descChildList[i].toObject()["itemUUID"].toString());
	//				projItemStack.push_back(static_cast<ProjectItemData *>(m_ItemRef.GetProject().FindItemData(nestedChildUuid)));
	//			}
	//		}
	//	}
	//}





	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocAssetTreeItem(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + pAssetItem->GetName());

	ItemType eItemType = ITEM_Unknown;
	switch(pAssetItem->GetAssetManagerType())
	{
	case ASSETMAN_Atlases: eItemType = ITEM_AtlasFrame; break;
	case ASSETMAN_Audio: eItemType = ITEM_SoundClip; break;
	default:
		HyGuiLog("EntityTreeModel::Cmd_InsertNewAsset - tried to add an unhandled asset manager type: " % QString::number(pAssetItem->GetAssetManagerType()), LOGTYPE_Error);
		break;
	}
	QUuid assetUuid = pAssetItem->GetUuid();
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eItemType, ENTTYPE_Item, assetUuid, QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocExistingTreeItem(QJsonObject descObj, bool bIsArrayItem, bool bIsFusedItem, int iRow)
{
	ItemType eGuiType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
	QString sCodeName = descObj["codeName"].toString();
	if(bIsArrayItem == false)
		sCodeName = GenerateCodeName(sCodeName);

	TreeModelItem *pParentTreeItem = nullptr;
	if(HyGlobal::IsItemType_Fixture(eGuiType) == false)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetFixtureFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(bIsArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, sCodeName, eGuiType, iRow);

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, descObj, bIsArrayItem, bIsFusedItem);
	iRow = (iRow < 0 || (bIsArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocGuiItemTreeItem(ItemType eWidgetType, QString sCodeNamePrefix, QUuid guiLayoutParentUuid, int iRow /*= -1*/)
{
	// Generate a unique code name for this new widget
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + HyGlobal::ItemName(eWidgetType, false));

	TreeModelItem *pParentTreeItem = GetRootTreeItem();
	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eWidgetType, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);

	if(pNewItem->IsLayoutItem() && guiLayoutParentUuid.isNull())
		HyGuiLog("EntityTreeModel::Cmd_AllocGuiItemTreeItem - Attempting to create a layout item in the GUI heirarchy without a valid layout parent", LOGTYPE_Error);
	
	if(guiLayoutParentUuid.isNull() == false)
	{
		InsertGuiItem(guiLayoutParentUuid, GuiItem(pNewItem->GetThisUuid()).Serialize());
		pNewItem->SetGuiParentUuid(guiLayoutParentUuid);
	}

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocPrimNodeTreeItem(QString sCodeNamePrefix, int iRow /*= -1*/)
{
	QString sCodeName = GenerateCodeName(sCodeNamePrefix + "Primitive");

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, ITEM_PrimNode, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetRootTreeItem(), iRow);

	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocPrimLayerTreeItem(EntityTreeItemData *pPrimNode, int iRow /*= -1*/)
{
	if(pPrimNode->GetType() != ITEM_PrimNode)
	{
		HyGuiLog("EntityTreeModel::Cmd_AllocPrimLayerTreeItem was passed a non-prim node item", LOGTYPE_Error);
		return nullptr;
	}

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, "Layer", ITEM_PrimLayer, ENTTYPE_SubItem, pPrimNode->GetThisUuid(), QUuid::createUuid());
	TreeModelItem *pParentTreeItem = GetItem(FindIndex<EntityTreeItemData *>(pPrimNode, 0));
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, pParentTreeItem, iRow);
	return pNewItem;
}

EntityTreeItemData *EntityTreeModel::Cmd_AllocFixtureTreeItem(bool bIsShape, QString sCodeNamePrefix, int iRow /*= -1*/)
{
	// Generate a unique code name for this new item
	QString sCodeName;
	if(bIsShape)
		sCodeName = GenerateCodeName(sCodeNamePrefix + "Shape");
	else
		sCodeName = GenerateCodeName(sCodeNamePrefix + "Chain");

	EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, bIsShape ? ITEM_ShapeFixture : ITEM_ChainFixture, ENTTYPE_Item, QUuid(), QUuid::createUuid());
	InsertTreeItem(m_ModelRef.GetItem().GetProject(), pNewItem, GetFixtureFolderTreeItem(), iRow);

	return pNewItem;
}

bool EntityTreeModel::Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow)
{
	// Generate a unique code name for this new item
	//QString sCodeName = GenerateCodeName(pItem->GetCodeName());

	TreeModelItem *pParentTreeItem = nullptr;
	if(pItem->IsFixtureItem() == false)
		pParentTreeItem = GetRootTreeItem();
	else
		pParentTreeItem = GetFixtureFolderTreeItem();

	bool bFoundArrayFolder = false;
	if(pItem->GetEntType() == ENTTYPE_ArrayItem)
		bFoundArrayFolder = FindOrCreateArrayFolder(pParentTreeItem, pItem->GetCodeName(), pItem->GetType(), iRow);

	// If apart of GUI layout, readd it
	InsertGuiItem(pItem->GetGuiParentUuid(), m_PoppedGuiItemsMap[pItem]);

	iRow = (iRow < 0 || (pItem->GetEntType() == ENTTYPE_ArrayItem && bFoundArrayFolder == false)) ? pParentTreeItem->GetNumChildren() : iRow;
	return InsertTreeItem(m_ModelRef.GetItem().GetProject(), pItem, pParentTreeItem, iRow);
}

int32 EntityTreeModel::Cmd_PopChild(EntityTreeItemData *pItem)
{
	QModelIndex itemIndex = FindIndex<EntityTreeItemData *>(pItem, 0);
	if(itemIndex.isValid() == false)
	{
		HyGuiLog("EntityTreeModel::Cmd_PopChild could not find index for pItem", LOGTYPE_Error);
		return -1;
	}

	TreeModelItem *pTreeItem = GetItem(itemIndex);
	TreeModelItem *pParentTreeItem = pTreeItem->GetParent();

	int32 iRow = pTreeItem->GetIndex();
	QModelIndex parentIndex = createIndex(pParentTreeItem->GetIndex(), 0, pParentTreeItem);
	if(removeRow(iRow, parentIndex) == false)
	{
		HyGuiLog("ExplorerModel::PopChild() - removeRow failed", LOGTYPE_Error);
		return -1;
	}

	// If item removed was apart of an array, check if its array folder is now empty. If so, remove the array folder too
	EntityTreeItemData *pParentTreeItemData = pParentTreeItem->data(0).value<EntityTreeItemData *>();
	if(pParentTreeItemData->GetEntType() == ENTTYPE_ArrayFolder && pParentTreeItem->GetNumChildren() == 0)
	{
		TreeModelItem *pArrayFolderParent = pParentTreeItem->GetParent();
		
		int iArrayFolderRow = pParentTreeItem->GetIndex();
		if(removeRow(iArrayFolderRow, createIndex(pArrayFolderParent->GetIndex(), 0, pArrayFolderParent)) == false)
			HyGuiLog("ExplorerModel::PopChild() - removeRow failed for array folder", LOGTYPE_Error);

		iRow = iArrayFolderRow; // Return the ArrayFolder row so if the item gets readded, it'll recreate the ArrayFolder on the same row
	}

	// If apart of GUI layout, remove it
	PopGuiItem(pItem);

	return iRow;
}

QVariant EntityTreeModel::data(const QModelIndex &indexRef, int iRole /*= Qt::DisplayRole*/) const
{
	TreeModelItem *pTreeItem = GetItem(indexRef);
	if(pTreeItem == m_pRootItem)
		return QVariant();

	if(iRole == Qt::UserRole)
		return ITreeModel::data(indexRef, iRole);

	EntityTreeItemData *pItem = pTreeItem->data(0).value<EntityTreeItemData *>();
	TreeModelItemData *pReferencedItemData = m_ModelRef.GetItem().GetProject().FindItemData(pItem->GetReferencedItemUuid());

	switch(iRole)
	{
	case Qt::DisplayRole:		// The key data to be rendered in the form of text. (QString)
	case Qt::EditRole:			// The data in a form suitable for editing in an editor. (QString)
	case Qt::StatusTipRole:		// The data displayed in the status bar. (QString)
		if(indexRef.column() == COLUMN_CodeName)
		{
			if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
				return QVariant(pItem->GetCodeName() % "[" % QString::number(pTreeItem->GetNumChildren()) % "]");
			else if(pItem->GetEntType() == ENTTYPE_ArrayItem)
				return QVariant("[" % QString::number(pTreeItem->GetIndex()) % "] - " % pItem->GetThisUuid().toString(QUuid::StringFormat::WithoutBraces).split('-')[0]);
			else
				return QVariant(pItem->GetCodeName());
		}
		else // COLUMN_ItemPath
		{
			if(pReferencedItemData)
			{
				if(pReferencedItemData->IsProjectItem())
					return static_cast<ProjectItemData *>(pReferencedItemData)->GetName(true);
				else if(pReferencedItemData->IsAssetItem())
					return static_cast<IAssetItemData *>(pReferencedItemData)->GetPropertyInfo();
			}
			
			return QVariant();
		}

	case Qt::DecorationRole:	// The data to be rendered as a decoration in the form of an icon. (QColor, QIcon or QPixmap)
		if(indexRef.column() != COLUMN_CodeName)
			return QVariant();

		if(pReferencedItemData)
		{
			if(pReferencedItemData->IsProjectItem())
			{
				ProjectItemData *pProjItem = static_cast<ProjectItemData *>(pReferencedItemData);
				if(pProjItem->IsExistencePendingSave())
					return QVariant(pItem->GetIcon(SUBICON_New));
				else if(pProjItem->IsSaveClean() == false)
					return QVariant(pItem->GetIcon(SUBICON_Dirty));
			}
			else
				return QVariant(pReferencedItemData->GetIcon(SUBICON_None));
		}

		if(pItem->GetEntType() == ENTTYPE_FixtureFolder)
			return QIcon(":/icons16x16/fixture-folder.png");
		else if(pItem->GetEntType() == ENTTYPE_ArrayFolder)
			return HyGlobal::ItemIcon(pItem->GetType(), SUBICON_Open);

		if(pItem->GetType() == ITEM_PrimLayer || pItem->GetType() == ITEM_ShapeFixture)
		{
			int iStateIndex = 0;
			if(m_ModelRef.GetItem().GetWidget())
				iStateIndex = m_ModelRef.GetItem().GetWidget()->GetCurStateIndex();
			const EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_ModelRef.GetStateData(iStateIndex))->GetDopeSheetScene();

			QIcon icon;
			QString sIconUrl = ":/icons16x16/shapes/" % QString(pItem->GetType() == ITEM_PrimLayer ? "primitive_" : "shapes_");

			QString sShapeType = dopeSheetSceneRef.BasicExtrapolateKeyFrameProperty(pItem, dopeSheetSceneRef.GetCurrentFrame(), pItem->GetType() == ITEM_PrimLayer ? "Primitive Layer" : "Shape", "Type").toString();
			switch(HyGlobal::GetShapeFromString(sShapeType))
			{
			case SHAPE_None:		sIconUrl += "icon.png"; break;
			case SHAPE_Box:			sIconUrl += "box.png"; break;
			case SHAPE_Circle:		sIconUrl += "circle.png"; break;
			case SHAPE_LineSegment:	sIconUrl += "lineSeg.png"; break;
			case SHAPE_Polygon:		sIconUrl += "polygon.png"; break;
			case SHAPE_Capsule:		sIconUrl += "capsule.png"; break;
			default: // Special case for primitives
				sIconUrl += "lineChain.png";
				break;
			}

			icon.addFile(sIconUrl);
			return QVariant(icon);
		}
			
		return QVariant(pItem->GetIcon(SUBICON_None));

	case Qt::ToolTipRole:		// The data displayed in the item's tooltip. (QString)
		return QVariant();// QVariant(pItem->GetThisUuid().toString());

	default:
		return QVariant();
	}

	return QVariant();
}

/*virtual*/ Qt::ItemFlags EntityTreeModel::flags(const QModelIndex &indexRef) const /*override*/
{
	EntityTreeItemData *pItem = GetItem(indexRef)->data(0).value<EntityTreeItemData *>();

	Qt::ItemFlags returnFlags = Qt::NoItemFlags;
	if(pItem->IsSelectable())
		returnFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	
	//if(pItem->GetEntType() == ENTTYPE_FixtureFolder)
	//	returnFlags |= Qt::ItemIsEnabled;
	
	return returnFlags;
}

/*virtual*/ void EntityTreeModel::OnTreeModelItemRemoved(TreeModelItem *pTreeItem) /*override*/
{
}

QString EntityTreeModel::GenerateCodeName(QString sDesiredName) const
{
	// Use 'HyGlobal::CodeNameValidator()' (QRegExpValidator) to make sure 'sDesiredName' is valid
	int pos = 0;
	if(HyGlobal::CodeNameValidator()->validate(sDesiredName, pos) != QValidator::Acceptable)
		sDesiredName.remove(QRegularExpression("[^a-zA-Z0-9_]"));

	if(sDesiredName.isEmpty())
		sDesiredName = "unnamed";

	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> fixtureList;
	GetTreeItemData(childList, fixtureList);
	childList += fixtureList; // Just combine the two since they all need to be unique

	uint uiConflictCount = 0;
	bool bIsUnique = false;
	do
	{
		QString sFullCodeName = sDesiredName;
		if(uiConflictCount > 0)
			sFullCodeName += QString::number(uiConflictCount + 1);

		int i = 0;
		for(; i < childList.size(); ++i)
		{
			if(childList[i] == nullptr)
				continue;

			if(sFullCodeName.compare(childList[i]->GetCodeName()) == 0)
			{
				uiConflictCount++;
				break;
			}
		}
		if(i == childList.size())
		{
			sDesiredName = sFullCodeName;
			bIsUnique = true;
		}

	} while(!bIsUnique);

	return sDesiredName;
}

bool EntityTreeModel::FindOrCreateArrayFolder(TreeModelItem *&pParentTreeItemOut, QString sCodeName, ItemType eItemType, int iRowToCreateAt)
{
	bool bFoundArrayFolder = false;

	// Find array tree item parent or create it
	for(int i = 0; i < pParentTreeItemOut->GetNumChildren(); ++i)
	{
		EntityTreeItemData *pSubItem = pParentTreeItemOut->GetChild(i)->data(0).value<EntityTreeItemData *>();
		if(sCodeName == pSubItem->GetCodeName() && pSubItem->GetEntType() == ENTTYPE_ArrayFolder)
		{
			pParentTreeItemOut = pParentTreeItemOut->GetChild(i);
			bFoundArrayFolder = true;
			break;
		}
	}

	if(bFoundArrayFolder == false)
	{
		QModelIndex parentIndex = FindIndex<EntityTreeItemData *>(pParentTreeItemOut->data(0).value<EntityTreeItemData *>(), 0);
		int iArrayFolderRow = (iRowToCreateAt == -1 ? pParentTreeItemOut->GetNumChildren() : iRowToCreateAt);

		//if(pParentTreeItemOut->GetNumChildren() > 0)
		//	iArrayFolderRow = HyMath::Min(iArrayFolderRow, pParentTreeItemOut->GetNumChildren() - 1);

		if(insertRow(iArrayFolderRow, parentIndex) == false)
		{
			HyGuiLog("EntityTreeModel::FindOrCreateArrayFolder - ArrayFolder insertRow failed", LOGTYPE_Error);
			return false;
		}
		// Allocate and store the new array folder item in the tree model
		EntityTreeItemData *pNewItem = new EntityTreeItemData(m_ModelRef, ENTDECLTYPE_Static, sCodeName, eItemType, ENTTYPE_ArrayFolder, QUuid(), QUuid());
		QVariant v;
		v.setValue(pNewItem);
		for(int iCol = 0; iCol < NUMCOLUMNS; ++iCol)
		{
			if(setData(index(iArrayFolderRow, iCol, parentIndex), v, Qt::UserRole) == false)
				HyGuiLog("ExplorerModel::FindOrCreateArrayFolder() - setData failed", LOGTYPE_Error);
		}

		pParentTreeItemOut = pParentTreeItemOut->GetChild(iArrayFolderRow);
	}

	return bFoundArrayFolder;
}
