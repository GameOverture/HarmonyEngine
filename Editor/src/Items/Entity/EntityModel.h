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

class EntityNodeItem : public TreeModelItemData
{
	Q_OBJECT

	QUuid												m_Uuid;
	PropertiesTreeModel									m_PropertiesTreeModel;

public:
	EntityNodeItem(ProjectItemData &entityItemDataRef, QString sCodeName, HyGuiItemType eItemType, QUuid uuidOfItem);
	EntityNodeItem(ProjectItemData &entityItemDataRef, QJsonObject initObj);
	virtual ~EntityNodeItem();

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
	
	QList<EntityNodeItem *> GetChildrenNodes() const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;
	EntityNodeItem *InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	bool InsertChild(EntityNodeItem *pItem, int iRow);
	int32 PopChild(EntityNodeItem *pItem);

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

	LineEditMapper										m_CodeNameMapper;
	ComboBoxMapper										m_EntityTypeMapper;

	EntityTreeModel										m_TreeModel;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	void RegisterWidgets(QLineEdit &txtCodeNameRef, QComboBox &cmbEntityTypeRef);

	EntityTreeModel &GetNodeTreeModel();

	// Command Modifiers - should (only) be called from UndoCmd's
	QList<EntityNodeItem *> Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	bool Cmd_AddChild(EntityNodeItem *pNodeItem, int iRow);
	int32 Cmd_RemoveChild(EntityNodeItem *pItem);
	void Cmd_AddPrimitive();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(AssetType eAssetType) const override;
};

#endif // ENTITYMODEL_H
