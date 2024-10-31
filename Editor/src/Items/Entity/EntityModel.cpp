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
#include "SourceModel.h"
#include "EntityUndoCmds.h"

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_DopeSheetScene(this, stateFileData.m_Meta)
{
}

/*virtual*/ EntityStateData::~EntityStateData()
{
}

EntityDopeSheetScene &EntityStateData::GetDopeSheetScene()
{
	return m_DopeSheetScene;
}

const EntityDopeSheetScene &EntityStateData::GetDopeSheetScene() const
{
	return m_DopeSheetScene;
}

//void EntityStateData::InsertNewPropertiesModel(EntityTreeItemData *pItemData, QJsonObject propObj)
//{
//	if(m_PropertiesMap.contains(pItemData))
//	{
//		HyGuiLog("EntityStateData::InsertNewPropertiesModel - item already was added", LOGTYPE_Error);
//		return;
//	}
//
//	PropertiesTreeModel *pNewProperties = new PropertiesTreeModel(m_ModelRef.GetItem(), m_iINDEX, QVariant());
//	InitalizePropertyModel(pItemData, *pNewProperties);
//	pNewProperties->DeserializeJson(propObj);
//
//	m_PropertiesMap.insert(pItemData, pNewProperties);
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityModel::AuxWidgetsModel::AuxWidgetsModel(EntityModel &entityModelRef, int iFramesPerSecond, bool bAutoInitialize) :
	m_EntityModelRef(entityModelRef),
	m_iFramesPerSecond(iFramesPerSecond),
	m_bAutoInitialize(bAutoInitialize)
{
	if(m_iFramesPerSecond <= 0)
		m_iFramesPerSecond = 60;
	dataChanged(index(0, 0), index(0, NUM_AUXDOPEWIDGETSECTIONS - 1));
}

/*virtual*/ int EntityModel::AuxWidgetsModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return 1;
}

/*virtual*/ int EntityModel::AuxWidgetsModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return NUM_AUXDOPEWIDGETSECTIONS;
}

/*virtual*/ QVariant EntityModel::AuxWidgetsModel::data(const QModelIndex &modelIndex, int role /*= Qt::DisplayRole*/) const /*override*/
{
	if(role == Qt::UserRole || role == Qt::EditRole)
	{
		if(modelIndex.column() == AUXDOPEWIDGETSECTION_FramesPerSecond)
			return QVariant(m_iFramesPerSecond);
		else
			return QVariant(m_bAutoInitialize);
	}
	return QVariant();
}

/*virtual*/ bool EntityModel::AuxWidgetsModel::setData(const QModelIndex &modelIndex, const QVariant &value, int role /*= Qt::EditRole*/) /*override*/
{
	if(role == Qt::EditRole)
	{
		if(modelIndex.column() == AUXDOPEWIDGETSECTION_FramesPerSecond)
		{
			if(m_iFramesPerSecond == value.toInt())
				return false;

			EntityUndoCmd_FramesPerSecond *pCmd = new EntityUndoCmd_FramesPerSecond(m_EntityModelRef, value.toInt());
			m_EntityModelRef.GetItem().GetUndoStack()->push(pCmd);
		}
		else if(modelIndex.column() == AUXDOPEWIDGETSECTION_AutoInitialize)
		{
			if(m_bAutoInitialize == value.toBool())
				return false;

			EntityUndoCmd_AutoInitialize *pCmd = new EntityUndoCmd_AutoInitialize(m_EntityModelRef, value.toBool());
			m_EntityModelRef.GetItem().GetUndoStack()->push(pCmd);
		}

		return false;
	}
	if(role == Qt::UserRole) // This occurs from the above undo commands
	{
		if(modelIndex.column() == AUXDOPEWIDGETSECTION_FramesPerSecond)
			m_iFramesPerSecond = value.toInt();
		else if(modelIndex.column() == AUXDOPEWIDGETSECTION_AutoInitialize)
			m_bAutoInitialize = value.toBool();
		else
			return false;

		dataChanged(modelIndex, modelIndex);
		//m_DopeSheetSceneRef.RefreshAllGfxItems();
		return true;
	}
	return QAbstractTableModel::setData(modelIndex, value, role);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef),
	m_TreeModel(*this, m_ItemRef.GetName(false), itemFileDataRef.m_Meta, this),
	m_bShapeEditMode(false),
	m_AuxWidgetsModel(*this, itemFileDataRef.m_Meta["framesPerSecond"].toInt(60), itemFileDataRef.m_Meta["autoInitialize"].toBool(true))
{
	// The EntityTreeModel ('m_TreeModel') was initialized first so that all the EntityTreeItemData's exist.
	// InitStates will look them up using their UUID when initializing its Key Frames map within the state's DopeSheetScene
	InitStates<EntityStateData>(itemFileDataRef);

	//// Initialize the callbacks list by calling EntityDopeSheetScene::SetCallback() for all the callbacks found in "stateArray"
	//// NOTE: EntityDopeSheetScene::SetCallback() will update *this 'm_CallbacksList'
	//QJsonArray stateArray = itemFileDataRef.m_Meta["stateArray"].toArray();
	//for(int i = 0; i < stateArray.size(); ++i)
	//{
	//	QJsonObject stateObj = stateArray[i].toObject();
	//	QJsonArray callbacksArray = stateObj["callbacks"].toArray();
	//	for(int j = 0; j < callbacksArray.size(); ++j)
	//	{
	//		QJsonObject callbackObj = callbacksArray[j].toObject();
	//		
	//	}
	//}
}

/*virtual*/ EntityModel::~EntityModel()
{
}

EntityTreeModel &EntityModel::GetTreeModel()
{
	return m_TreeModel;
}

QAbstractItemModel *EntityModel::GetAuxWidgetsModel()
{
	return &m_AuxWidgetsModel;
}

QList<QString *> &EntityModel::GetCallbacksList()
{
	return m_CallbacksList;
}

int EntityModel::GetFinalFrameIndex(int iStateIndex) const
{
	return static_cast<const EntityStateData *>(GetStateData(iStateIndex))->GetDopeSheetScene().GetFinalFrame();
}

int EntityModel::GetFramesPerSecond() const
{
	return m_AuxWidgetsModel.data(m_AuxWidgetsModel.index(0, AUXDOPEWIDGETSECTION_FramesPerSecond), Qt::UserRole).toInt();
}

bool EntityModel::IsAutoInitialize() const
{
	return m_AuxWidgetsModel.data(m_AuxWidgetsModel.index(0, AUXDOPEWIDGETSECTION_AutoInitialize), Qt::UserRole).toBool();
}

QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewChildren(QList<ProjectItemData *> projItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pItem : projItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_AllocChildTreeItem(pItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_CreateNewChildren could not insert a child: " % pItem->GetName(true), LOGTYPE_Error);

		registerList.push_back(pItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);
	
	return treeNodeList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewAssets(QList<IAssetItemData *> assetItemList, int iRow)
{
	QList<EntityTreeItemData *> treeNodeList;
	QList<QUuid> registerList;
	for(auto *pAssetItem : assetItemList)
	{
		EntityTreeItemData *pAddedItem = m_TreeModel.Cmd_AllocAssetTreeItem(pAssetItem, "m_", iRow);
		if(pAddedItem)
			treeNodeList.push_back(pAddedItem);
		else
			HyGuiLog("EntityModel::Cmd_CreateNewAssets could not insert an asset child: " % pAssetItem->GetName(), LOGTYPE_Error);

		registerList.push_back(pAssetItem->GetUuid());
	}

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, registerList);

	return treeNodeList;
}

EntityTreeItemData *EntityModel::Cmd_AddExistingItem(QJsonObject descObj, bool bIsArrayItem, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_AllocExistingTreeItem(descObj, bIsArrayItem, iRow);

	QUuid uuidToRegister(descObj["itemUUID"].toString());
	if(uuidToRegister.isNull() == false)
		m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << uuidToRegister);

	return pTreeItemData;
}

EntityTreeItemData *EntityModel::Cmd_CreateNewShape(int iStateIndex, int iFrameIndex, EditorShape eShape, QString sData, bool bIsPrimitive, int iRow)
{
	EntityTreeItemData *pTreeItemData = m_TreeModel.Cmd_AllocShapeTreeItem(eShape, sData, bIsPrimitive, "m_", iRow);
	static_cast<EntityStateData *>(GetStateData(iStateIndex))->GetDopeSheetScene().SetKeyFrameProperty(pTreeItemData, iFrameIndex, "Shape", "Type", QJsonValue(HyGlobal::ShapeName(eShape)), false);
	static_cast<EntityStateData *>(GetStateData(iStateIndex))->GetDopeSheetScene().SetKeyFrameProperty(pTreeItemData, iFrameIndex, "Shape", "Data", QJsonValue(sData), true);
	
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	if(pWidget)
		pWidget->RequestSelectedItems(QList<QUuid>() << pTreeItemData->GetThisUuid());

	SetShapeEditMode(true);

	return pTreeItemData;
}

QList<EntityTreeItemData *> EntityModel::Cmd_AddNewPasteItems(QJsonObject mimeObject, EntityTreeItemData *pArrayFolder)
{
	QList<EntityTreeItemData *> pastedItemList;

	QJsonArray itemArray = mimeObject["itemArray"].toArray();
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();

		QJsonObject descObj = itemObj["descObj"].toObject();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("isSelected", false);

		bool bIsArrayItem = false;
		if(pArrayFolder)
		{
			ItemType eGuiType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
			if(eGuiType != pArrayFolder->GetType())
			{
				HyGuiLog("EntityUndoCmd_PasteItems::redo - pasted array item (" % descObj["codeName"].toString() % ") " % descObj["itemType"].toString() % " did match array type", LOGTYPE_Error);
				continue;
			}

			descObj["codeName"] = pArrayFolder->GetCodeName();
			bIsArrayItem = true;
		}

		// NOTE: Error checking the pasted item's states to match current entity should have already been done in EntityWidget::on_actionPasteEntityItems_triggered
		EntityTreeItemData *pPastedTreeItemData = Cmd_AddExistingItem(descObj, bIsArrayItem, -1);
		pastedItemList.push_back(pPastedTreeItemData);

		// Merge the pasted key frame data into the states for the new item
		QJsonArray stateKeyFramesArray = itemObj["stateKeyFramesArray"].toArray();
		for(int iStateIndex = 0; iStateIndex < GetNumStates(); ++iStateIndex)
		{
			if(stateKeyFramesArray.size() <= iStateIndex)
				break;

			EntityStateData *pStateData = static_cast<EntityStateData *>(GetStateData(iStateIndex));

			QJsonObject stateKeyFramesObj = stateKeyFramesArray[iStateIndex].toObject();
			QJsonArray keyFramesArray = stateKeyFramesObj["keyFrames"].toArray();

			for(int iKeyFrameIndex = 0; iKeyFrameIndex < keyFramesArray.size(); ++iKeyFrameIndex)
			{
				QJsonObject keyFrameObj = keyFramesArray[iKeyFrameIndex].toObject();
				pStateData->GetDopeSheetScene().SetKeyFrameProperties(pPastedTreeItemData, keyFrameObj["frame"].toInt(), keyFrameObj["props"].toObject());
			}
		}
	}

	return pastedItemList;
}

QList<EntityTreeItemData *> EntityModel::Cmd_CreateNewArray(QList<EntityTreeItemData *> itemDataList, QString sArrayName, int iArrayFolderRow)
{
	// Create temporary EntityItemMimeData to generate JSON object that contains an "itemList" JSON array of that represents 'itemDataList'
	EntityItemMimeData *pMimeData = new EntityItemMimeData(m_ItemRef, itemDataList);
	QByteArray jsonData = pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityItems));
	delete pMimeData;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	QJsonObject mimeObject = jsonDocument.object();
	QJsonArray itemArray = mimeObject["itemArray"].toArray();

	// Parse info from 'itemArray' and invoke Cmd_AddExistingItem on each item
	QList<EntityTreeItemData *> newItemDataList;
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();

		QJsonObject descObj = itemObj["descObj"].toObject();
		QJsonArray stateKeyFramesArray = itemObj["stateKeyFramesArray"].toArray();

		QUuid newUuid = QUuid::createUuid();
		descObj.insert("UUID", newUuid.toString(QUuid::StringFormat::WithoutBraces));
		descObj.insert("codeName", sArrayName);
		descObj.insert("isSelected", false);

		EntityTreeItemData *pDuplicateItem = Cmd_AddExistingItem(descObj, true, i == 0 ? iArrayFolderRow : -1);
		newItemDataList.push_back(pDuplicateItem);

		// Copy all its property/key frames into the newly created item
		for(int j = 0; j < stateKeyFramesArray.size(); ++j)
		{
			EntityStateData *pStateData = static_cast<EntityStateData *>(m_StateList[j]);

			QJsonObject stateKeyFramesObj = stateKeyFramesArray[j].toObject();
			if(pStateData->GetName() != stateKeyFramesObj["name"].toString() || GetFramesPerSecond() != stateKeyFramesObj["framesPerSecond"].toInt())
				HyGuiLog("EntityModel::Cmd_CreateNewArray - states mismatch", LOGTYPE_Error);
			
			QJsonArray keyFramesArray = stateKeyFramesObj["keyFrames"].toArray();
			for(int iKeyFrameIndex = 0; iKeyFrameIndex < keyFramesArray.size(); ++iKeyFrameIndex)
			{
				QJsonObject keyFrameObj = keyFramesArray[iKeyFrameIndex].toObject();
				pStateData->GetDopeSheetScene().SetKeyFrameProperties(pDuplicateItem, keyFrameObj["frame"].toInt(), keyFrameObj["props"].toObject());
			}
		}
	}

	return newItemDataList;
}

void EntityModel::Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	for(EntityTreeItemData *pTreeItem : selectedList)
		pTreeItem->SetSelected(true);
	for(EntityTreeItemData *pTreeItem : deselectedList)
		pTreeItem->SetSelected(false);

	static_cast<EntityStateData *>(m_StateList[static_cast<EntityWidget *>(m_ItemRef.GetWidget())->GetCurStateIndex()])->GetDopeSheetScene().RefreshAllGfxItems();

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ApplyJsonData();
}

int32 EntityModel::Cmd_RemoveTreeItem(EntityTreeItemData *pItem)
{
	if(pItem == nullptr)
		return -1;

	m_ItemRef.GetProject().DecrementDependencies(&m_ItemRef, QList<QUuid>() << pItem->GetReferencedItemUuid());

	int32 iRow = m_TreeModel.Cmd_PopChild(pItem);
	if(iRow < 0)
		return iRow;

	// Pop all the key frames associated with this item
	for(IStateData *pStateData : m_StateList)
		static_cast<EntityStateData *>(pStateData)->GetDopeSheetScene().PopAllKeyFrames(pItem, true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ClearHover(); // Flushes out potentially deleted IDrawEx::m_pCurHoverItem

	return iRow;
}

bool EntityModel::Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow)
{
	if(m_TreeModel.Cmd_ReaddChild(pNodeItem, iRow) == false)
		return false;

	m_ItemRef.GetProject().IncrementDependencies(&m_ItemRef, QList<QUuid>() << pNodeItem->GetReferencedItemUuid());

	// Re-add all the key frames associated with this item
	for(IStateData *pStateData : m_StateList)
		static_cast<EntityStateData *>(pStateData)->GetDopeSheetScene().PushAllKeyFrames(pNodeItem, true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ClearHover(); // Flushes out potentially deleted IDrawEx::m_pCurHoverItem

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

void EntityModel::ToggleShapeAdd(EditorShape eShapeType, bool bAsPrimitive)
{
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pWidget == nullptr || pEntDraw == nullptr)
		return;

	if(eShapeType != SHAPE_None && eShapeType != pEntDraw->GetShapeAddType() && pEntDraw->SetAsShapeAdd(eShapeType, bAsPrimitive))
	{
		pWidget->CheckShapeAddBtn(eShapeType, bAsPrimitive);

		QString sStatusMsg("Creating New ");
		sStatusMsg += bAsPrimitive ? "Primitive: " : "Shape: ";
		sStatusMsg += HyGlobal::ShapeName(eShapeType);
		MainWindow::SetStatus(sStatusMsg, 0);
	}
	else
	{
		pWidget->UncheckAll();
		pEntDraw->SetAsShapeEditMode(false);
		MainWindow::ClearStatus();
	}
}

bool EntityModel::IsShapeEditMode() const
{
	return m_bShapeEditMode;
}

void EntityModel::ToggleShapeEditMode()
{
	SetShapeEditMode(!m_bShapeEditMode);
}

void EntityModel::SetShapeEditMode(bool bEnable)
{
	m_bShapeEditMode = bEnable;
	if(m_bShapeEditMode)
		MainWindow::SetStatus("Shape Edit Mode", 0);
	else
		MainWindow::ClearStatus();

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_ItemRef.GetWidget());
	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pWidget == nullptr || pEntityDraw == nullptr)
	{
		HyGuiLog("EntityModel::SetShapeEditMode() - pWidget or pEntityDraw is nullptr", LOGTYPE_Error);
		return;
	}

	m_TreeModel.RefreshSelectedItems();

	pWidget->SetAsShapeEditMode(m_bShapeEditMode);
	pEntityDraw->SetAsShapeEditMode(m_bShapeEditMode);

	pWidget->UpdateActions();
	pEntityDraw->ApplyJsonData();
}

QString EntityModel::GenerateCodeName(QString sDesiredName) const
{
	return m_TreeModel.GenerateCodeName(sDesiredName);
}

QString EntityModel::GenerateSrc_FileIncludes() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);

	QStringList sIncludeList;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pItem->GetType() != ITEM_Entity)
			continue;

		QString sIncludeFileName;
		if(pItem->IsPromotedEntity() == false)
			sIncludeFileName = "hy_" + pItem->GetHyNodeTypeName(false) + ".h";
		else
			sIncludeFileName = pItem->GetHyNodeTypeName(false) + ".h";
		
		sIncludeList.append(sIncludeFileName);
	}
	sIncludeList.removeDuplicates();

	for(QString sInclude : sIncludeList)
		sSrc += "#include \"" + sInclude + "\"\n";

	return sSrc;
}

QString EntityModel::GenerateSrc_StateEnums() const
{
	// Just print states as comments instead of declaring an enum
	QString sSrc;
	for(int i = 0; i < m_StateList.size(); ++i)
		sSrc += "// State " + QString::number(i) + ((i < 10) ? ":  " : ": ") + m_StateList[i]->GetName() + "\n";

	//QString sSrc;
	//sSrc += "\tenum " + m_ItemRef.GetName(false) + "State\n\t{\n\t";
	//for(int i = 0; i < m_StateList.size(); ++i)
	//{
	//	sSrc += "\t\tSTATE_" + m_StateList[i]->GetName();
	//	if(i == 0)
	//		sSrc += " = 0";
	//	if(i < m_StateList.size() - 1)
	//		sSrc += ",\n";
	//}
	//sSrc += "\n\t};";

	return sSrc;
}

QString EntityModel::GenerateSrc_MemberVariables() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	EntityTreeItemData *pCurArray = nullptr;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pCurArray)
		{
			if(pCurArray->GetCodeName() == pItem->GetCodeName())
				continue;
			pCurArray = nullptr;
		}

		sSrc += "\t";
		QString sType = pItem->GetHyNodeTypeName(true);
		sSrc += sType;
		const int iNUM_TAB_WIDTH = 7;
		int iNumTabs = iNUM_TAB_WIDTH - (sType.length() / 4); // Do 'iNUM_TAB_WIDTH' tabs (minus sType's length / 4) to align the variable names
		if(iNumTabs > 0)
		{
			for(int i = 0; i < iNumTabs; ++i)
				sSrc += "\t";
		}
		else
			sSrc += " ";
		sSrc += pItem->GetCodeName();
		if(pItem->GetEntType() != ENTTYPE_ArrayItem)
			sSrc += ";\n";
		else
		{
			sSrc += "[" + QString::number(m_TreeModel.GetArrayFolderTreeItem(pItem)->GetNumChildren()) + "];\n";
			pCurArray = pItem;
		}
	}

	return sSrc;
}

QString EntityModel::GenerateSrc_AccessorDecl() const
{
	QString sSrc;
	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);

	QStringList arrayList;
	for(EntityTreeItemData *pItem : itemList)
	{
		QString sCodeName = pItem->GetCodeName();
		if(sCodeName.startsWith("m_"))
			sCodeName.remove(0, 2);

		if(pItem->GetEntType() == ENTTYPE_ArrayItem)
		{
			if(arrayList.contains(sCodeName) == false)
			{
				sSrc += "\n\t" + pItem->GetHyNodeTypeName(true) +
					(pItem->GetDeclarationType() == ENTDECLTYPE_Static ? " &" : " *") +
					"Get" + sCodeName + "(int iIndex);";
				
				arrayList.push_back(sCodeName);
			}
		}
		else
		{
			sSrc += "\n\t" + pItem->GetHyNodeTypeName(true) +
				(pItem->GetDeclarationType() == ENTDECLTYPE_Static ? " &" : " *") +
				"Get" + sCodeName + "();";
		}
	}

	return sSrc;
}

QString EntityModel::GenerateSrc_AccessorDefinition(QString sClassName) const
{
	QString sSrc;
	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);

	QStringList arrayList;
	for(EntityTreeItemData *pItem : itemList)
	{
		QString sCodeName = pItem->GetCodeName();
		if(sCodeName.startsWith("m_"))
			sCodeName.remove(0, 2);

		if(pItem->GetEntType() == ENTTYPE_ArrayItem)
		{
			if(arrayList.contains(sCodeName) == false)
			{
				sSrc += "\n" + pItem->GetHyNodeTypeName(true) +
					(pItem->GetDeclarationType() == ENTDECLTYPE_Static ? " &" : " *") +
					sClassName + "::Get" + sCodeName + "(int iIndex)\n{";
				sSrc += "\n\tHyAssert(iIndex >= 0 && iIndex < " + QString::number(pItem->GetNumArrayItems()) + ", \"" + sClassName + "::Get" + sCodeName + " tried to access an array element '\" << iIndex << \"' which is out of bounds.\");";
				sSrc += "\n\treturn " + pItem->GetCodeName() + "[iIndex];";
				sSrc += "\n}";

				arrayList.push_back(sCodeName);
			}
		}
		else
		{
			sSrc += "\n" + pItem->GetHyNodeTypeName(true) +
				(pItem->GetDeclarationType() == ENTDECLTYPE_Static ? " &" : " *") +
				sClassName + "::Get" + sCodeName + "() { return " + pItem->GetCodeName() + "; }";
		}
	}
	return sSrc;
}

QString EntityModel::GenerateSrc_CallbacksDecl() const
{
	QString sSrc;
	for(QString *pCallback : m_CallbacksList)
		sSrc += "\n\tvirtual void " + *pCallback + "() { }";

	return sSrc;
}

QString EntityModel::GenerateSrc_MemberInitializerList() const
{
	QString sSrc = " :\n\tHyEntity2d(pParent)";

	sSrc += ",\n\tm_fTIMELINE_FRAME_DURATION(" + QString::number(1.0 / GetFramesPerSecond(), 'f') + "f)";
	sSrc += ",\n\tm_fpTimelineUpdate(nullptr)";
	sSrc += ",\n\tm_fTimelineFrameTime(0.0f)";
	sSrc += ",\n\tm_uiTimelineFrame(0)";
	sSrc += ",\n\tm_bTimelinePaused(false)";
	sSrc += ",\n\tm_uiTimelineFinalFrame(0)";

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	EntityTreeItemData *pCurArray = nullptr;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pCurArray)
		{
			if(pCurArray->GetCodeName() != pItem->GetCodeName())
			{
				sSrc += "},\n\t" + pItem->GetCodeName();
				pCurArray = nullptr;
			}
		}
		else
			sSrc += ",\n\t" + pItem->GetCodeName();

		TreeModelItemData *pReferencedItemData = GetItem().GetProject().FindItemData(pItem->GetReferencedItemUuid());
		QString sInitialization;
		switch(pItem->GetType())
		{
		case ITEM_Primitive:
		case ITEM_BoundingVolume:
		case ITEM_Entity:
			sInitialization = "(this)";
			break;

		case ITEM_Audio:
		case ITEM_Text:
		case ITEM_Spine:
		case ITEM_Sprite:
		case ITEM_TileMap:
			if(pReferencedItemData == nullptr || pReferencedItemData->IsProjectItem() == false)
				HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Could not find referenced project item for: " % pItem->GetCodeName(), LOGTYPE_Error);
			else
			{
				QString sItemPath = static_cast<ProjectItemData *>(pReferencedItemData)->GetName(true);
				sInitialization = "(HyNodePath(\"" + sItemPath + "\"), this)";
			}
			break;

		case ITEM_AtlasFrame:
			if(pReferencedItemData == nullptr || pReferencedItemData->IsAssetItem() == false)
			{
				HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Could not find referenced asset item for: " % pItem->GetCodeName(), LOGTYPE_Error);
				break;
			}
			sInitialization = "(" + QString::number(static_cast<IAssetItemData *>(pReferencedItemData)->GetChecksum()) + ", " + QString::number(static_cast<IAssetItemData *>(pReferencedItemData)->GetBankId()) + ", this)";
			break;

		default:
			HyGuiLog("EntityModel::GenerateSrc_MemberInitializerList() - Unhandled item type: " % HyGlobal::ItemName(pItem->GetType(), false), LOGTYPE_Error);
			break;
		}
		
		if(pItem->GetEntType() != ENTTYPE_ArrayItem)
			sSrc += sInitialization;
		else
		{
			if(pCurArray == nullptr)
				sSrc += "{";
			else
				sSrc += ", ";

			sSrc += pItem->GetHyNodeTypeName(true) + sInitialization;
			pCurArray = pItem;
		}
	}

	if(pCurArray)
		sSrc += "}";

	return sSrc;
}

QString EntityModel::GenerateSrc_Ctor() const
{
	return "SetState(0);";
}

// NOTE: The listed 4 functions below share logic that process all item properties. Any updates should reflect to all of them
//             - EntityTreeItemData::InitalizePropertyModel
//             - EntityModel::GenerateSrc_SetStateImpl
//             - EntityDrawItem::ExtractPropertyData
//             - ExtrapolateProperties
QString EntityModel::GenerateSrc_SetStateImpl() const
{
	QString sSrc = "switch(m_uiState)\n\t{";

	for(int i = 0; i < GetNumStates(); ++i)
	{
		const EntityDopeSheetScene &entDopeSheetSceneRef = static_cast<const EntityStateData *>(GetStateData(i))->GetDopeSheetScene();

		sSrc += "\n\tcase " + QString::number(i) + ": // " + m_StateList[i]->GetName() + "\n\t\t";

		sSrc += "m_uiTimelineFinalFrame = " + QString::number(entDopeSheetSceneRef.GetFinalFrame()) + ";\n\t\t";
		sSrc += "m_fpTimelineUpdate = [this]()\n\t\t{\n\t\t\t";

		sSrc += "std::vector<glm::vec2> vertList;\n\t\t\t";
		sSrc += "switch(m_uiTimelineFrame)\n\t\t\t{\n\t\t\t";
		sSrc += "default:\n\t\t\t\tbreak;\n\n\t\t\t";

		QMap<int, QMap<EntityTreeItemData *, QJsonObject>> propertiesMapByFrame = entDopeSheetSceneRef.GetKeyFrameMapPropertiesByFrame();
		const QMap<int, QList<QString *>> &callbacksMap = entDopeSheetSceneRef.GetCallbacksMap();

		QList<int> callbacksFrameIndexList = callbacksMap.keys();
		QList<int> frameList = propertiesMapByFrame.keys();

		// Combine and remove duplicates
		QSet<int> frameSet = callbacksFrameIndexList.toSet();
		frameSet.unite(frameList.toSet());
		frameList = frameSet.toList();

		std::sort(frameList.begin(), frameList.end()); // Then sort by frame index
		for(int iFrameIndex : frameList)
		{
			sSrc += "case " + QString::number(iFrameIndex) + ":\n\t\t\t\t";

			// CALL IN THIS ORDER:
			// 1: Standard Properties
			if(propertiesMapByFrame.contains(iFrameIndex))
			{
				for(QMap<EntityTreeItemData *, QJsonObject>::const_iterator iter = propertiesMapByFrame[iFrameIndex].begin(); iter != propertiesMapByFrame[iFrameIndex].end(); ++iter)
					sSrc += GenerateSrc_SetProperties(iter.key(), iter.value(), "\n\t\t\t\t");
			}

			// 2: Callbacks
			if(callbacksFrameIndexList.contains(iFrameIndex))
			{
				QList<QString *> sCallbacks = callbacksMap[iFrameIndex];
				for(const QString *pCallback : sCallbacks)
					sSrc += *pCallback + "();\n\t\t\t\t";
			}

			// 3: Root Entity Timeline Events
			if(propertiesMapByFrame.contains(iFrameIndex) && propertiesMapByFrame[iFrameIndex].contains(m_TreeModel.GetRootTreeItemData()))
			{
				QJsonObject rootPropsObj = propertiesMapByFrame[iFrameIndex][m_TreeModel.GetRootTreeItemData()];
				for(QString sCategoryName : rootPropsObj.keys())
				{
					if(sCategoryName == "Timeline")
					{
						QJsonObject timelineObj = rootPropsObj["Timeline"].toObject();

						if(timelineObj.contains("Pause"))
							sSrc += "SetTimelinePause(" + QString(timelineObj["Pause"].toBool() ? "true" : "false") + ");" + "\n\t\t\t\t";

						if(timelineObj.contains("Frame"))
							sSrc += "SetTimelineFrame(" + QString::number(timelineObj["Frame"].toInt()) + ");" + "\n\t\t\t\t";

						if(timelineObj.contains("State"))
							sSrc += "SetState(" + QString::number(timelineObj["State"].toInt()) + ");" + "\n\t\t\t\t";
					}
				}
			}

			sSrc += "break;\n\n\t\t\t";
		}

		sSrc += "}\n\t\t"; // End switch(m_uiTimelineFrame)

		sSrc += "};\n\t\t"; // End m_fpTimelineUpdate
		sSrc += "break;\n"; // End case m_uiState
	}
	sSrc += "\t}"; // End switch(m_uiState)

	return sSrc;
}

QString EntityModel::GenerateSrc_SetProperties(EntityTreeItemData *pItemData, QJsonObject propObj, QString sNewLine) const
{
	QString sSrc;

	QString sCodeName;
	if(pItemData->GetEntType() != ENTTYPE_Root)
	{
		if(pItemData->GetEntType() == ENTTYPE_ArrayItem)
			sCodeName = pItemData->GetCodeName() % "[" % QString::number(pItemData->GetArrayIndex()) % "]";
		else
			sCodeName = pItemData->GetCodeName();

		if(pItemData->GetDeclarationType() == ENTDECLTYPE_Static)
			sCodeName += ".";
		else
			sCodeName += "->";
	}

	for(QString sCategoryName : propObj.keys())
	{
		// NOTE: Only do sub-entity "Timeline" properties. The root "Timeline" is handled last
		if(sCategoryName == "Timeline" && pItemData->GetEntType() != ENTTYPE_Root)
		{
			QJsonObject timelineObj = propObj["Timeline"].toObject();

			if(timelineObj.contains("Pause"))
				sSrc += sCodeName + "SetTimelinePause(" + (timelineObj["Pause"].toBool() ? "true" : "false") + ");" + sNewLine;

			if(timelineObj.contains("Frame"))
				sSrc += sCodeName + "SetTimelineFrame(" + QString::number(timelineObj["Frame"].toInt()) + ");" + sNewLine;

			if(timelineObj.contains("State"))
				sSrc += sCodeName + "SetState(" + QString::number(timelineObj["State"].toInt()) + ");" + sNewLine;
		}
		else if(sCategoryName == "Common")
		{
			QJsonObject commonObj = propObj["Common"].toObject();
			if(commonObj.contains("State"))
				sSrc += sCodeName + "SetState(" + QString::number(commonObj["State"].toInt()) + ");" + sNewLine;

			if(commonObj.contains("Update During Paused"))
				sSrc += sCodeName + "SetPauseUpdate(" + (commonObj["Update During Paused"].toBool() ? "true" : "false") + ");" + sNewLine;

			if(commonObj.contains("User Tag"))
			{
				sSrc += "#ifdef HY_ENABLE_USER_TAGS" + sNewLine;
				sSrc += sCodeName + "SetTag(" + QString::number(commonObj["User Tag"].toInt()) + ");" + sNewLine;
				sSrc += "#endif" + sNewLine;
			}
		}
		else if(sCategoryName == "Transformation")
		{
			QJsonObject transformObj = propObj["Transformation"].toObject();
			if(transformObj.contains("Position"))
			{
				QJsonArray posArray = transformObj["Position"].toArray();
				sSrc += sCodeName + "pos.Set(" + QString::number(posArray[0].toDouble(), 'f') + "f, " + QString::number(posArray[1].toDouble(), 'f') + "f);" + sNewLine;
			}
			if(transformObj.contains("Scale"))
			{
				QJsonArray scaleArray = transformObj["Scale"].toArray();
				sSrc += sCodeName + "scale.Set(" + QString::number(scaleArray[0].toDouble(), 'f') + "f, " + QString::number(scaleArray[1].toDouble(), 'f') + "f);" + sNewLine;
			}
			if(transformObj.contains("Rotation"))
				sSrc += sCodeName + "rot.Set(" + QString::number(transformObj["Rotation"].toDouble(), 'f') + "f);" + sNewLine;
		}
		else if(sCategoryName == "Body")
		{
			QJsonObject bodyObj = propObj["Body"].toObject();
			if(bodyObj.contains("Visible"))
				sSrc += sCodeName + "SetVisible(" + (bodyObj["Visible"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(bodyObj.contains("Color Tint"))
			{
				QJsonArray colorArray = bodyObj["Color Tint"].toArray();
				sSrc += sCodeName + "SetTint(HyColor(" + QString::number(colorArray[0].toInt()) + ", " + QString::number(colorArray[1].toInt()) + ", " + QString::number(colorArray[2].toInt()) + ", 255));" + sNewLine;
			}
			if(bodyObj.contains("Alpha"))
				sSrc += sCodeName + "alpha.Set(" + QString::number(bodyObj["Alpha"].toDouble(), 'f') + "f);" + sNewLine;
			if(bodyObj.contains("Override Display Order"))
				sSrc += sCodeName + "SetDisplayOrder(" + QString::number(bodyObj["Override Display Order"].toInt()) + ");" + sNewLine;
		}
		else if(sCategoryName == "Physics")
		{
			QJsonObject physicsObj = propObj["Physics"].toObject();
			if(physicsObj.contains("Type"))
			{
				sSrc += sCodeName + "physics.SetType(";
				switch(physicsObj["Type"].toInt())
				{
				case HYBODY_Static:		sSrc += "HYBODY_Static";	break;
				case HYBODY_Kinematic:	sSrc += "HYBODY_Kinematic";	break;
				case HYBODY_Dynamic:	sSrc += "HYBODY_Dynamic";	break;
				}
				sSrc += ");" + sNewLine;
			}
			if(physicsObj.contains("Fixed Rotation"))
				sSrc += sCodeName + "physics.SetFixedRotation(" + (physicsObj["Fixed Rotation"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(physicsObj.contains("Initially Awake"))
				sSrc += sCodeName + "physics.SetAwake(" + (physicsObj["Initially Awake"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(physicsObj.contains("Allow Sleep"))
				sSrc += sCodeName + "physics.SetSleepingAllowed(" + (physicsObj["Allow Sleep"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(physicsObj.contains("Gravity Scale"))
				sSrc += sCodeName + "physics.SetGravityScale(" + QString::number(physicsObj["Gravity Scale"].toDouble(), 'f') + "f);" + sNewLine;
			if(physicsObj.contains("Dynamic CCD"))
				sSrc += sCodeName + "physics.SetCcd(" + (physicsObj["Dynamic CCD"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(physicsObj.contains("Linear Damping"))
				sSrc += sCodeName + "physics.SetLinearDamping(" + QString::number(physicsObj["Linear Damping"].toDouble(), 'f') + "f);" + sNewLine;
			if(physicsObj.contains("Angular Damping"))
				sSrc += sCodeName + "physics.SetAngularDamping(" + QString::number(physicsObj["Angular Damping"].toDouble(), 'f') + "f);" + sNewLine;
			if(physicsObj.contains("Linear Velocity"))
			{
				QJsonArray velArray = physicsObj["Linear Velocity"].toArray();
				sSrc += sCodeName + "physics.SetVel(" + QString::number(velArray[0].toDouble(), 'f') + "f, " + QString::number(velArray[1].toDouble(), 'f') + "f);" + sNewLine;
			}
			if(physicsObj.contains("Angular Velocity"))
				sSrc += sCodeName + "physics.SetAngVel(" + QString::number(physicsObj["Angular Velocity"].toDouble(), 'f') + "f);" + sNewLine;
			if(physicsObj.contains("Activate/Deactivate"))
			{
				if(physicsObj["Activate/Deactivate"].toBool())
					sSrc += sCodeName + "physics.Activate();" + sNewLine;
				else
					sSrc += sCodeName + "physics.Deactivate();" + sNewLine;
			}
		}
		else if(sCategoryName == "Entity")
		{
			QJsonObject entityObj = propObj["Entity"].toObject();
			if(entityObj.contains("Mouse Input"))
			{
				if(entityObj["Mouse Input"].toBool())
					sSrc += sCodeName + "EnableMouseInput();" + sNewLine;
				else
					sSrc += sCodeName + "DisableMouseInput();" + sNewLine;
			}
		}
		else if(sCategoryName == "Primitive")
		{
			QJsonObject primitiveObj = propObj["Primitive"].toObject();
			if(primitiveObj.contains("Wireframe"))
				sSrc += sCodeName + "SetWireframe(" + (primitiveObj["Wireframe"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(primitiveObj.contains("Line Thickness"))
				sSrc += sCodeName + "SetLineThickness(" + QString::number(primitiveObj["Line Thickness"].toDouble(), 'f') + "f);" + sNewLine;
		}
		else if(sCategoryName == "Shape")
		{
			QJsonObject shapeObj = propObj["Shape"].toObject();
			if(shapeObj.contains("Type") && shapeObj.contains("Data"))
			{
				EditorShape eShapeType = HyGlobal::GetShapeFromString(shapeObj["Type"].toString());
				uint32 uiMaxVertListSizeOut = 0;
				sSrc += ShapeCtrl::DeserializeAsRuntimeCode(sCodeName, eShapeType, shapeObj["Data"].toString(), sNewLine, uiMaxVertListSizeOut);
			}
		}
		else if(sCategoryName == "Fixture")
		{
			QJsonObject fixtureObj = propObj["Fixture"].toObject();
			if(fixtureObj.contains("Density"))
				sSrc += sCodeName + "SetDensity(" + QString::number(fixtureObj["Density"].toDouble(), 'f') + "f);" + sNewLine;
			if(fixtureObj.contains("Friction"))
				sSrc += sCodeName + "SetFriction(" + QString::number(fixtureObj["Friction"].toDouble(), 'f') + "f);" + sNewLine;
			if(fixtureObj.contains("Restitution"))
				sSrc += sCodeName + "SetRestitution(" + QString::number(fixtureObj["Restitution"].toDouble(), 'f') + "f);" + sNewLine;
			if(fixtureObj.contains("Restitution Threshold"))
				sSrc += sCodeName + "SetRestitutionThreshold(" + QString::number(fixtureObj["Restitution Threshold"].toDouble(), 'f') + "f);" + sNewLine;
			if(fixtureObj.contains("Sensor"))
				sSrc += sCodeName + "SetSensor(" + (fixtureObj["Sensor"].toBool() ? "true" : "false") + ");" + sNewLine;

			// TODO: filter
			//m_pPropertiesModel->AppendProperty("Fixture", "Filter: Category Mask", PROPERTIESTYPE_int, 0x0001, "The collision category bits for this shape. Normally you would just set one bit", PROPERTIESACCESS_ToggleOff, 0, 0xFFFF, 1, QString(), QString(), QVariant());
			//m_pPropertiesModel->AppendProperty("Fixture", "Filter: Collision Mask", PROPERTIESTYPE_int, 0xFFFF, "The collision mask bits. This states the categories that this shape would accept for collision", PROPERTIESACCESS_ToggleOff, 0, 0xFFFF, 1, QString(), QString(), QVariant());
			//m_pPropertiesModel->AppendProperty("Fixture", "Filter: Group Override", PROPERTIESTYPE_int, 0, "Collision overrides allow a certain group of objects to never collide (negative) or always collide (positive). Zero means no collision override", PROPERTIESACCESS_ToggleOff, std::numeric_limits<int16>::min(), std::numeric_limits<int16>::max(), 1, QString(), QString(), QVariant());
		}
		else if(sCategoryName == "Sprite")
		{
			QJsonObject spriteObj = propObj["Sprite"].toObject();
			if(spriteObj.contains("Frame"))
				sSrc += sCodeName + "SetFrame(" + QString::number(spriteObj["Frame"].toInt()) + ");" + sNewLine;
			if(spriteObj.contains("Anim Pause"))
				sSrc += sCodeName + "SetAnimPause(" + (spriteObj["Anim Pause"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(spriteObj.contains("Anim Rate"))
				sSrc += sCodeName + "SetAnimRate(" + QString::number(spriteObj["Anim Rate"].toDouble(), 'f') + "f);" + sNewLine;
			if(spriteObj.contains("Anim Loop"))
				sSrc += sCodeName + "SetAnimCtrl(" + (spriteObj["Anim Loop"].toBool() ? "HYANIMCTRL_Loop" : "HYANIMCTRL_DontLoop") + ");" + sNewLine;
			if(spriteObj.contains("Anim Reverse"))
				sSrc += sCodeName + "SetAnimCtrl(" + (spriteObj["Anim Reverse"].toBool() ? "HYANIMCTRL_Reverse" : "HYANIMCTRL_DontReverse") + ");" + sNewLine;
			if(spriteObj.contains("Anim Bounce"))
				sSrc += sCodeName + "SetAnimCtrl(" + (spriteObj["Anim Bounce"].toBool() ? "HYANIMCTRL_Bounce" : "HYANIMCTRL_DontBounce") + ");" + sNewLine;
		}
		else if(sCategoryName == "Text")
		{
			QJsonObject textObj = propObj["Text"].toObject();
			if(textObj.contains("Text"))
				sSrc += sCodeName + "SetText(\"" + textObj["Text"].toString() + "\");" + sNewLine;
			if(textObj.contains("Style"))
			{
				HyTextType eTextStyle = HyGlobal::GetTextTypeFromString(textObj["Style"].toString());
				switch(eTextStyle)
				{
				case HYTEXT_Line:
					sSrc += sCodeName + "SetAsLine();" + sNewLine;
					break;
				case HYTEXT_Column:
					if(textObj.contains("Style Dimensions"))
						sSrc += sCodeName + "SetAsColumn(" + QString::number(textObj["Style Dimensions"].toArray()[0].toDouble(), 'f') + "f);" + sNewLine;
					else
						HyGuiLog("Missing Style Dimensions for Text Style: " + textObj["Style"].toString(), LOGTYPE_Error);
					break;
				case HYTEXT_Box:
					if(textObj.contains("Style Dimensions"))
						sSrc += sCodeName + "SetAsBox(" + QString::number(textObj["Style Dimensions"].toArray()[0].toDouble(), 'f') + "f, " + QString::number(textObj["Style Dimensions"].toArray()[1].toDouble(), 'f') + "f, false);" + sNewLine;
					else
						HyGuiLog("Missing Style Dimensions for Text Style: " + textObj["Style"].toString(), LOGTYPE_Error);
					break;
				case HYTEXT_ScaleBox:
					if(textObj.contains("Style Dimensions"))
						sSrc += sCodeName + "SetAsScaleBox(" + QString::number(textObj["Style Dimensions"].toArray()[0].toDouble(), 'f') + "f, " + QString::number(textObj["Style Dimensions"].toArray()[1].toDouble(), 'f') + "f, true);" + sNewLine;
					else
						HyGuiLog("Missing Style Dimensions for Text Style: " + textObj["Style"].toString(), LOGTYPE_Error);
					break;
				case HYTEXT_Vertical:
					sSrc += sCodeName + "SetAsVertical();" + sNewLine;
					break;
				}
			}
			if(textObj.contains("Alignment"))
			{
				HyAlignment eAlignment = HyGlobal::GetAlignmentFromString(textObj["Alignment"].toString());
				switch(eAlignment)
				{
				case HYALIGN_Left:		sSrc += sCodeName + "SetAlignment(HYALIGN_Left);" + sNewLine;	break;
				case HYALIGN_Center:	sSrc += sCodeName + "SetAlignment(HYALIGN_Center);" + sNewLine;	break;
				case HYALIGN_Right:		sSrc += sCodeName + "SetAlignment(HYALIGN_Right);" + sNewLine;	break;
				case HYALIGN_Justify:	sSrc += sCodeName + "SetAlignment(HYALIGN_Justify);" + sNewLine;break;
				}
			}
			if(textObj.contains("Monospaced Digits"))
				sSrc += sCodeName + "SetMonospacedDigits(" + (textObj["Monospaced Digits"].toBool() ? "true" : "false") + ");" + sNewLine;
			if(textObj.contains("Text Indent"))
				sSrc += sCodeName + "SetTextIndent(" + QString::number(textObj["Text Indent"].toInt()) + ");" + sNewLine;
		}
		else if(sCategoryName == "Tween Position")
		{
			QJsonObject tweenObj = propObj["Tween Position"].toObject();
			QJsonArray destArray = tweenObj["Destination"].toArray();
			QString sTweenType = "HyTween::" + tweenObj["Tween Type"].toString();
			
			sSrc += sCodeName + "pos.Tween(" + QString::number(destArray[0].toDouble(), 'f') + "f, " + QString::number(destArray[1].toDouble(), 'f') + "f, " + QString::number(tweenObj["Duration"].toDouble(), 'f') + "f, " + sTweenType + ");" + sNewLine;
		}
		else if(sCategoryName == "Tween Rotation")
		{
			QJsonObject tweenObj = propObj["Tween Rotation"].toObject();
			QString sTweenType = "HyTween::" + tweenObj["Tween Type"].toString();

			sSrc += sCodeName + "rot.Tween(" + QString::number(tweenObj["Destination"].toDouble(), 'f') + "f, " + QString::number(tweenObj["Duration"].toDouble(), 'f') + "f, " + sTweenType + ");" + sNewLine;
		}
		else if(sCategoryName == "Tween Scale")
		{
			QJsonObject tweenObj = propObj["Tween Scale"].toObject();
			QJsonArray destArray = tweenObj["Destination"].toArray();
			QString sTweenType = "HyTween::" + tweenObj["Tween Type"].toString();

			sSrc += sCodeName + "scale.Tween(" + QString::number(destArray[0].toDouble(), 'f') + "f, " + QString::number(destArray[1].toDouble(), 'f') + "f, " + QString::number(tweenObj["Duration"].toDouble(), 'f') + "f, " + sTweenType + ");" + sNewLine;
		}
		else if(sCategoryName == "Tween Alpha")
		{
			QJsonObject tweenObj = propObj["Tween Alpha"].toObject();
			QString sTweenType = "HyTween::" + tweenObj["Tween Type"].toString();

			sSrc += sCodeName + "alpha.Tween(" + QString::number(tweenObj["Destination"].toDouble(), 'f') + "f, " + QString::number(tweenObj["Duration"].toDouble(), 'f') + "f, " + sTweenType + ");" + sNewLine;
		}
	}

	return sSrc;
}

// Advance runtime by 1 frame on the timeline
QString EntityModel::GenerateSrc_TimelineAdvance() const
{
	QString sSrc;

	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	EntityTreeItemData *pCurArray = nullptr;
	for(EntityTreeItemData *pItem : itemList)
	{
		if(pCurArray)
		{
			if(pCurArray->GetCodeName() == pItem->GetCodeName())
				continue;
			pCurArray = nullptr;
		}

		QString sAccessOperator;
		if(pItem->GetDeclarationType() == ENTDECLTYPE_Static)
			sAccessOperator = ".";
		else
			sAccessOperator = "->";

		if(pItem->GetEntType() != ENTTYPE_ArrayItem)
		{
			if(pItem->GetType() == ITEM_Sprite)
				sSrc += "\t\t" + pItem->GetCodeName() + sAccessOperator + "AdvanceAnim(m_fTIMELINE_FRAME_DURATION);\n";
		}
		else // ENTTYPE_ArrayItem
		{
			if(pItem->GetType() == ITEM_Sprite)
			{
				for(int i = 0; i < m_TreeModel.GetArrayFolderTreeItem(pItem)->GetNumChildren(); ++i)
					sSrc += "\t\t" + pItem->GetCodeName() + "[" + QString::number(i) + "]" + sAccessOperator + "AdvanceAnim(m_fTIMELINE_FRAME_DURATION);\n";
			}
			pCurArray = pItem;
		}
	}

	return sSrc;
}

/*virtual*/ void EntityModel::OnPopState(int iPoppedStateIndex) /*override*/
{
}

/*virtual*/ bool EntityModel::OnPrepSave() /*override*/
{
	// Save generated C++ class in a .h
	SourceModel &sourceModelRef = m_ItemRef.GetProject().GetSourceModel();
	return sourceModelRef.GenerateEntitySrcFiles(*this);
}

/*virtual*/ void EntityModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
	itemSpecificFileDataOut.m_Meta.insert("codeName", m_TreeModel.GetRootTreeItemData()->GetCodeName());

	itemSpecificFileDataOut.m_Meta.insert("framesPerSecond", GetFramesPerSecond());
	itemSpecificFileDataOut.m_Meta.insert("autoInitialize", IsAutoInitialize());
	
	QList<EntityTreeItemData *> childList;
	QList<EntityTreeItemData *> shapeList;
	m_TreeModel.GetTreeItemData(childList, shapeList);

	QJsonArray childArray;
	QString sCurrentArrayCodeName = "";
	for(int i = 0; i < childList.size(); ) // 'i' is incremented inside the loop
	{
		if(childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) != 0)
		{
			sCurrentArrayCodeName = childList[i]->GetCodeName();
			QJsonArray packedArray;
			do
			{
				QJsonObject arrayItemObj;
				childList[i]->InsertJsonInfo_Desc(arrayItemObj);

				packedArray.append(arrayItemObj);
				++i;

			} while(i < childList.size() && childList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(childList[i]->GetCodeName()) == 0);

			childArray.append(packedArray);
		}
		else
		{
			QJsonObject childObj;
			childList[i]->InsertJsonInfo_Desc(childObj);
			
			childArray.append(childObj);
			++i;
		}
	}
	itemSpecificFileDataOut.m_Meta.insert("descChildList", childArray);

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
				shapeList[i]->InsertJsonInfo_Desc(arrayItemObj);
				
				packedArray.append(arrayItemObj);
				++i;

			} while(i < shapeList.size() && shapeList[i]->GetEntType() == ENTTYPE_ArrayItem && sCurrentArrayCodeName.compare(shapeList[i]->GetCodeName()) == 0);

			shapeArray.append(packedArray);
		}
		else
		{
			QJsonObject shapeObj;
			shapeList[i]->InsertJsonInfo_Desc(shapeObj);
			
			shapeArray.append(shapeObj);
			++i;
		}
	}
	itemSpecificFileDataOut.m_Meta.insert("descShapeList", shapeArray);

	//QJsonArray callbacksArray;
	//for(QString sCallback : m_CallbacksList)
	//	callbacksArray.append(sCallback);
	//itemSpecificFileDataOut.m_Meta.insert("callbacksList", callbacksArray);
}

/*virtual*/ void EntityModel::InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const /*override*/
{
	EntityStateData *pStateData = static_cast<EntityStateData *>(m_StateList[uiIndex]);

	// Combine all items (root, children, and shapes) into a single list 'itemList'
	QList<EntityTreeItemData *> itemList, shapeList;
	m_TreeModel.GetTreeItemData(itemList, shapeList);
	itemList.append(shapeList);
	itemList.prepend(m_TreeModel.GetRootTreeItemData());

	// Serialize all key frames for each item that has them
	QJsonObject stateKeyFramesObj;
	for(EntityTreeItemData *pItem : itemList)
	{
		QJsonArray keyFramesArray = pStateData->GetDopeSheetScene().SerializeAllKeyFrames(pItem);
		if(keyFramesArray.empty())
			continue;
		
		QString sUuid = pItem->GetThisUuid().toString(QUuid::StringFormat::WithoutBraces);
		stateKeyFramesObj.insert(sUuid, keyFramesArray);
	}
	stateFileDataOut.m_Meta.insert("keyFrames", stateKeyFramesObj);

	// Serialize all callbacks for this state
	QJsonArray callbacksArray = pStateData->GetDopeSheetScene().SerializeCallbacks();
	stateFileDataOut.m_Meta.insert("callbacks", callbacksArray);
}

/*virtual*/ void EntityModel::OnItemDeleted() /*override*/
{
	SourceModel &sourceModelRef = m_ItemRef.GetProject().GetSourceModel();
	sourceModelRef.DeleteEntitySrcFiles(*this);
}
