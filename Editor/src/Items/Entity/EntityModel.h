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

#define ENTCOLOR_TransformBv HyColor::Blue.Lighten()
#define ENTCOLOR_Primitive HyColor::DarkMagenta
#define ENTCOLOR_Shape HyColor::Cyan
#define ENTCOLOR_Marquee HyColor::Blue.Lighten()
#define ENTCOLOR_Clear HyColor::White

#define ENTCOLORPOINT_Transform HyColor::White, HyColor::Blue.Lighten()
#define ENTCOLORPOINT_TransformRotate HyColor::Blue.Lighten(), HyColor::White
#define ENTCOLORPOINT_Vem HyColor::White, HyColor::Red
#define ENTCOLORPOINT_VemSelected HyColor::White, HyColor::Blue.Lighten()

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

	bool												m_bVertexEditMode;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	void RegisterWidgets(QComboBox &cmbEntityTypeRef);

	EntityTreeModel &GetTreeModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	QList<EntityTreeItemData *> Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	EntityTreeItemData *Cmd_AddNewChild(QJsonObject initObj, int iRow);
	EntityTreeItemData *Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow);
	void Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList);
	int32 Cmd_RemoveTreeItem(EntityTreeItemData *pItem);
	bool Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow);

	void SetShapeEditDrag(EditorShape eShapeType, bool bAsPrimitive);
	void SetShapeEditVemMode(bool bEnable);
	void ClearShapeEdit();

	virtual void OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) override;

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(AssetType eAssetType) const override;
};

#endif // ENTITYMODEL_H
