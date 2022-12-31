/**************************************************************************
*	EntityModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYMODEL_H
#define ENTITYMODEL_H

#include "IModel.h"
#include "ProjectItemData.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"
#include "PropertiesTreeModel.h"
#include "Shared/TreeModel/ITreeModel.h"

#include <QObject>
#include <QJsonArray>

class EntityModel;

class EntityTreeItemData : public TreeModelItemData
{
	Q_OBJECT

	QUuid												m_Uuid;
	PropertiesTreeModel									m_PropertiesTreeModel;

public:
	EntityTreeItemData(ProjectItemData &entityItemDataRef, QString sCodeName, HyGuiItemType eItemType, QUuid uuidOfItem);
	EntityTreeItemData(ProjectItemData &entityItemDataRef, QJsonObject initObj);
	virtual ~EntityTreeItemData();

	QString GetCodeName() const;
	QUuid GetUuid() const;
	PropertiesTreeModel &GetPropertiesModel();

	void InsertJsonInfo(QJsonObject &childObjRef);

protected:
	void InitalizePropertiesTree();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityTreeModel : public ITreeModel
{
	Q_OBJECT

	EntityModel &										m_ModelRef;

	enum ColumnType
	{
		COLUMN_CodeName = 0,
		COLUMN_ItemPath,

		NUMCOLUMNS
	};

public:
	explicit EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QUuid uuidOfEntity, QObject *pParent = nullptr);
	virtual ~EntityTreeModel();
	
	TreeModelItem *GetEntityTreeItem() const;
	EntityTreeItemData *GetEntityTreeItemData() const;
	void GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;
	EntityTreeItemData *Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_InsertNewChild(ProjectItemData *pProjItem, QJsonObject initObj, int iRow = -1);
	bool Cmd_InsertChild(EntityTreeItemData *pItem, int iRow);
	int32 Cmd_PopChild(EntityTreeItemData *pItem);

	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

protected:
	QString GenerateCodeName(QString sDesiredName) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityStateData : public IStateData
{
public:
	EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~EntityStateData();

	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	ComboBoxMapper										m_EntityTypeMapper;
	EntityTreeModel										m_TreeModel;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	void RegisterWidgets(QComboBox &cmbEntityTypeRef);

	EntityTreeModel &GetNodeTreeModel();

	// Command Modifiers (Cmd_) - should only be called from this constructor and from UndoCmd's
	QList<EntityTreeItemData *> Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	EntityTreeItemData *Cmd_AddNewChild(ProjectItemData *pProjItemData, QJsonObject initObj, int iRow);
	EntityTreeItemData *Cmd_AddNewPrimitive(int iRow);
	EntityTreeItemData *Cmd_AddShape(int iRow);
	void Cmd_UpdateItem(EntityTreeItemData *pItem);
	int32 Cmd_RemoveTreeItem(EntityTreeItemData *pItem);
	bool Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow);

	virtual void OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) override;

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(AssetType eAssetType) const override;
};

#endif // ENTITYMODEL_H
