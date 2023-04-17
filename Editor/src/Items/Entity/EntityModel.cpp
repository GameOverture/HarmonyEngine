/**************************************************************************
*	EntityModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityModel.h"
#include "Project.h"
#include "ExplorerModel.h"
#include "MainWindow.h"
#include "EntityDraw.h"
#include "EntityWidget.h"
#include "EntityItemMimeData.h"
#include "IAssetItemData.h"

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData)
{
}
/*virtual*/ EntityStateData::~EntityStateData()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_EntityTypeMapper(this),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta["UUID"].toString(), this),
	m_bVertexEditMode(false)
{
	InitStates<EntityStateData>(itemFileDataRef);

	// Insert all the items into the nodeTree
	QJsonArray childListArray = itemFileDataRef.m_Meta["childList"].toArray();
	for(int i = 0; i < childListArray.size(); ++i)
	{
		if(childListArray[i].isObject())
		{
			QJsonObject childObj = childListArray[i].toObject();
			Cmd_AddNewItem(childObj, false, i);
		}
		else if(childListArray[i].isArray())
		{
			QJsonArray childArray = childListArray[i].toArray();
			for(int j = 0; j < childArray.size(); ++j)
				Cmd_AddNewItem(childArray[j].toObject(), true, j == 0 ? i : j);
		}
		else
			HyGuiLog("EntityModel::EntityModel invalid childlist", LOGTYPE_Error);
	}

	QJsonArray shapeListArray = itemFileDataRef.m_Meta["shapeList"].toArray();
	for(int i = 0; i < shapeListArray.size(); ++i)
	{
		if(shapeListArray[i].isObject())
		{
			QJsonObject shapeObj = shapeListArray[i].toObject();
			Cmd_AddNewItem(shapeObj, false, i);
		}
		else if(shapeListArray[i].isArray())
		{
			QJsonArray shapeArray = shapeListArray[i].toArray();
			for(int j = 0; j < shapeArray.size(); ++j)
				Cmd_AddNewItem(shapeArray[j].toObject(), true, j == 0 ? i : j);
		}
		else
			HyGuiLog("EntityModel::EntityModel invalid shapeList", LOGTYPE_Error);
	}
}

/*virtual*/ EntityModel::~EntityModel()
{
}

void EntityModel::RegisterWidgets(QComboBox &cmbEntityTypeRef)
{
	m_EntityTypeMapper.AddComboBoxMapping(&cmbEntityTypeRef);
}

EntityTreeModel &EntityModel::GetTreeModel()
{
	return m_TreeModel;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_InsertNewChild(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);

		registerList.push_back(pItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
	
	return treeNodeList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewAssets(QList<AssetItemData *> assetItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pAssetItem : assetItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_InsertNewChild(pAssetItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_AddNewChildren could not insert an asset child: " % pAssetItem->GetName(), LOGTYPE_Error);

		registerList.push_back(pAssetItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);

	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddNewItem(QJsonObject initObj, bool bIsArrayItem, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewItem(initObj, bIsArrayItem, iRow);

	QUuid uuidToRegister(initObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_InsertNewShape(eShape, sData, bIsPrimitive, "m_", iRow);
	
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << pTreeItemData->GetThisUuid());

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ActivateVemOnNextJsonMeta();

	return pTreeItemData;
}

// It is assumed that the items within 'itemDataList' have been removed/popped prior
QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewArray(QList<EntityTreeItemData *> itemDataList, QString sArrayName, int iArrayFolderRow)
{
	sArrayName = GenerateCodeName(sArrayName);

	// Create temporary EntityItemMimeData to generate JSON object that contains an "itemList" JSON array of that represents 'itemDataList'
	EntityItemMimeData *pMimeData = new EntityItemMimeData(m_ItemRef, itemDataList);
	QByteArray jsonData = pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityItems));
	delete pMimeData;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	QJsonObject mimeObject = jsonDocument.object();
	QJsonArray duplicateItemArray = mimeObject["itemList"].toArray();

	// Add all the duplicates in the array
	QList<EntityTreeItemData *> arrayItemList;
	for(int i = 0; i < duplicateItemArray.size(); ++i)
	{
		QJsonObject arrayItemObj = duplicateItemArray[i].toObject();

		QJsonObject commonObj = arrayItemObj["Common"].toObject();
		commonObj.insert("UUID", QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));

		arrayItemObj.insert("Common", commonObj); // Reinsert "Common" with new UUID
		arrayItemObj.insert("codeName", sArrayName);
		arrayItemObj.insert("isSelected", false);

		EntityTreeItemData *pDuplicateItem = Cmd_AddNewItem(arrayItemObj, true, i == 0 ? iArrayFolderRow : -1);
		arrayItemList.push_back(pDuplicateItem);
	}

	return arrayItemList;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	for(EntityTreeItemData *pTreeItem : selectedList)
		pTreeItem->SetSelected(true);
	for(EntityTreeItemData *pTreeItem : deselectedList)
		pTreeItem->SetSelected(false);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ApplyJsonData();
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	int32 iRow = m_TreeModel.Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, QList<QUuid>() << pItem->GetItemUuid());

	ClearShapeEdit();

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_TreeModel.Cmd_ReaddChild(pNodeItem, iRow) == false)
		return false;

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << pNodeItem->GetItemUuid());

	return true;
}

void EntityModel::Cmd_RenameItem(EntityTreeItemData *pItemData, QString sNewName)
{
	if(pItemData->GetEntType() == ENTTYPE_ArrayFolder)
	{
		pItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_TreeModel.GetItemsRecursively(m_TreeModel.FindIndex<EntityTreeItemData *>(pItemData, 0));
		for(TreeModelItemData *pItemData : arrayChildrenList)
			pItemData->SetText(sNewName);
	}
	else if(pItemData->GetEntType() == ENTTYPE_ArrayItem)
	{
		QModelIndex arrayFolderIndex = m_TreeModel.parent(m_TreeModel.FindIndex<EntityTreeItemData *>(pItemData, 0));
		EntityTreeItemData *pArrayFolderItemData = m_TreeModel.data(arrayFolderIndex, Qt::UserRole).value<EntityTreeItemData *>();

		pArrayFolderItemData->SetText(sNewName);

		QList<TreeModelItemData *> arrayChildrenList = m_TreeModel.GetItemsRecursively(arrayFolderIndex);
		for(TreeModelItemData *pItemData : arrayChildrenList)
			pItemData->SetText(sNewName);
	}
	else
		pItemData->SetText(sNewName);
}

void EntityModel::SetShapeEditDrag(EditorShape eShapeType, bool bAsPrimitive)
{
	QString sStatusMsg("Drawing new ");
	sStatusMsg += bAsPrimitive ? "primitive " : "";
	sStatusMsg += HyGlobal::ShapeName(eShapeType);
	MainWindow::SetStatus(sStatusMsg, 0);

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->CheckShapeAddBtn(eShapeType, bAsPrimitive);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetShapeEditDrag(eShapeType, bAsPrimitive);
}

void EntityModel::SetShapeEditVemMode(bool bEnable)
{
	m_bVertexEditMode = bEnable;

	if(m_bVertexEditMode)
		MainWindow::SetStatus("Vertex Edit Mode", 0);
	else
		MainWindow::ClearStatus();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->CheckVertexEditMode(m_bVertexEditMode);

	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntityDraw)
	{
		if(m_bVertexEditMode)
			pEntityDraw->SetShapeEditVertex();
		else
			pEntityDraw->ClearShapeEdit();
	}
}

void EntityModel::ClearShapeEdit()
{
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->UncheckAll();

	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntityDraw)
		pEntityDraw->ClearShapeEdit();
}

QString EntityModel::GenerateCodeName(QString sDesiredName) const
{
	return m_TreeModel.GenerateCodeName(sDesiredName);
}

/*virtual*/ void EntityModel::OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) /*override*/
{
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(propertiesModelRef.GetSubstate().toLongLong());
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	// Save generated C++ class in a .h
	//m_ItemRef.GetProject().GetSourceAbsPath()

	return true;
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_TreeModel.GetRootTreeItemData()->GetCodeName());
	itemSpecificFileDataOut.m_Meta.insert("entityType", m_EntityTypeMapper.GetCurrentItem());
	
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_TreeModel.GetTreeItemData(childList, shapeList);

	QJsonArray childArray;
	QString sCurrentArrayCodeName = "";
	for(int i = 0; i < childList.size(); )
	{
		if(childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) != 0)
		{
			sCurrentArrayCodeName = childList[i]->GetCodeName();
			QJsonArray packedArray;
			do
			{
				QJsonObject arrayItemObj;
				childList[i]->InsertJsonInfo(arrayItemObj);
				packedArray.append(arrayItemObj);
				++i;

			} while(i < childList.size() && childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) == 0);

			childArray.append(packedArray);
		}
		else
		{
			QJsonObject childObj;
			childList[i]->InsertJsonInfo(childObj);
			childArray.append(childObj);

			++i;
		}
	}
	itemSpecificFileDataOut.m_Meta.insert("childList", childArray);

	QJsonArray shapeArray;
	sCurrentArrayCodeName = "";
	for(int i = 0; i < shapeList.size(); )
	{
		if(shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) != 0)
		{
			sCurrentArrayCodeName = shapeList[i]->GetCodeName();
			QJsonArray packedArray;
			do
			{
				QJsonObject arrayItemObj;
				shapeList[i]->InsertJsonInfo(arrayItemObj);
				packedArray.append(arrayItemObj);
				++i;

			} while(i < shapeList.size() && shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) == 0);

			shapeArray.append(packedArray);
		}
		else
		{
			QJsonObject shapeObj;
			shapeList[i]->InsertJsonInfo(shapeObj);
			shapeArray.append(shapeObj);

			++i;
		}
	}	
	itemSpecificFileDataOut.m_Meta.insert("shapeList", shapeArray);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
}

/*virtual*/ QList<AssetItemData *> EntityModel::GetAssets(AssetType eAssetType) const /*override*/
{
	// TODO: EntityModel::GetAssets
	return QList<AssetItemData *>();
}
