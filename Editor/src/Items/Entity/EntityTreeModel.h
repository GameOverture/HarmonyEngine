/**************************************************************************
*	EntityTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItemData.h"
#include "EntityTreeItemData.h"
#include "IAssetItemData.h"
#include "ITreeModel.h"

#include <QObject>
#include <QJsonArray>

class EntityModel;

class EntityTreeModel : public ITreeModel
{
	friend class EntityModel;

	Q_OBJECT

	EntityModel &										m_ModelRef;

	EntityTreeItemData *								m_pRootTreeItemData;						// The root item data itself. It stores all the properties for EVERY available base class to choose from. The properties are shown/hidden and serialized to runtime accordingly.
	EntityTreeItemData *								m_FusedTreeItemData[NUM_ENTBASECLASSTYPES];	// Each base class type can have one "fused" item that is a component of the class declaration

	QList<EntityTreeItemData *>							m_GuiLayoutItemList;

public:
	enum ColumnType
	{
		COLUMN_CodeName = 0,
		COLUMN_ItemPath,

		NUMCOLUMNS
	};

	// Only applicable when entity base class type is HyGui
	struct GuiItem
	{
		QUuid												m_Uuid;						// References a widget, or unique when layout/spacer
		QList<GuiItem>										m_ChildList;				// Only used when type is ITEM_UiLayout

		GuiItem(QUuid uuid) : m_Uuid(uuid) { }
		GuiItem(QJsonObject serializedObj)
		{
			if(serializedObj.isEmpty())
				return;

			m_Uuid = QUuid(serializedObj["uuid"].toString());
			QJsonArray childArray = serializedObj["children"].toArray();
			m_ChildList.reserve(childArray.size());
			for(int i = 0; i < childArray.size(); ++i)
				m_ChildList.emplace_back(childArray[i].toObject());
		}
		bool operator==(const QUuid &other) const { return m_Uuid == other; }
		bool operator!=(const QUuid &other) const { return m_Uuid != other; }
		bool IsValid() const { return m_Uuid.isNull() == false; }

		QJsonObject Serialize() const
		{
			QJsonObject obj;
			obj["uuid"] = m_Uuid.toString();
			QJsonArray childArray;
			for(const GuiItem &child : m_ChildList)
				childArray.append(child.Serialize());
			obj["children"] = childArray;
			return obj;
		}
	};

private:
	GuiItem													m_GuiLayout;				// Serialized as an object "guiLayout" - purpose is to store the GUI layout heirarchy
	QMap<EntityTreeItemData *, QJsonObject>					m_PoppedGuiItemsMap;

public:
	explicit EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QJsonObject fileMetaObj, QObject *pParent = nullptr);
	virtual ~EntityTreeModel();
	
	TreeModelItem *GetRootTreeItem() const;
	EntityTreeItemData *GetRootTreeItemData() const;

	TreeModelItem *GetFixtureFolderTreeItem() const;
	EntityTreeItemData *GetFixtureFolderTreeItemData() const;

	QList<EntityTreeItemData *> GetAllFusedItemData() const;

	TreeModelItem *GetArrayFolderTreeItem(EntityTreeItemData *pArrayItem) const;
	EntityTreeItemData *GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const;

	void InsertGuiItem(QUuid uuidParent, QJsonObject guiItemObj);
	bool PopGuiItem(EntityTreeItemData *pItem);
	QUuid FindGuiLayoutFromItemUuid(QUuid itemUuid) const;
	void GuiDisassemble();
	void GuiAssemble();

	void GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const;
	void GetSelectedTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &fixtureListOut) const;
	EntityTreeItemData *FindTreeItemData(QUuid uuid) const;

	int GetPrimLayerIndex(EntityTreeItemData *pPrimLayer, EntityTreeItemData *&pPrimNodeOut) const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;

	QJsonObject SerializeGuiLayout() const;

private: // These functions should only be called by EntityModel's Cmd_ functions
	void Cmd_ResetFusedItems(); // (Re)inserts fuse items into the item tree model. It will also destruct/construct the GUI layout heirarchy
	EntityTreeItemData *Cmd_AllocChildTreeItem(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_AllocAssetTreeItem(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_AllocExistingTreeItem(QJsonObject descObj, bool bIsArrayItem, bool bIsFusedItem, int iRow);
	EntityTreeItemData *Cmd_AllocGuiItemTreeItem(ItemType eWidgetType, QString sCodeNamePrefix, QUuid guiLayoutParentUuid, int iRow = -1);
	EntityTreeItemData *Cmd_AllocPrimNodeTreeItem(QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_AllocPrimLayerTreeItem(EntityTreeItemData *pPrimNode, int iRow = -1);
	EntityTreeItemData *Cmd_AllocFixtureTreeItem(bool bIsShape, QString sCodeNamePrefix, int iRow = -1);
	bool Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow);
	int32 Cmd_PopChild(EntityTreeItemData *pItem);

public:
	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

	QString GenerateCodeName(QString sDesiredName) const;

protected:
	bool FindOrCreateArrayFolder(TreeModelItem *&pParentTreeItemOut, QString sCodeName, ItemType eItemType, int iRowToCreateAt); // 'pParentTreeItemOut' must point to either Root or BvFolder, it will be reassigned to the ArrayFolder that is either found (return true), or newly created (return false)
};

#endif // ENTITYTREEMODEL_H
