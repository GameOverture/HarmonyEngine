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
#include "EntityTreeModel.h"
#include "ProjectItemMimeData.h"

#include <QObject>
#include <QJsonArray>

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

	EntityTreeModel &GetTreeModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	QList<EntityTreeItemData *> Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	EntityTreeItemData *Cmd_AddNewChild(ProjectItemData *pProjItemData, QJsonObject initObj, int iRow);
	EntityTreeItemData *Cmd_AddNewPrimitive(int iRow);
	EntityTreeItemData *Cmd_AddNewShape();
	void Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList);
	int32 Cmd_RemoveTreeItem(EntityTreeItemData *pItem);
	bool Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow);

	virtual void OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) override;

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(AssetType eAssetType) const override;
};

#endif // ENTITYMODEL_H
