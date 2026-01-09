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
#include "EntityDopeSheetScene.h"
#include "ProjectItemMimeData.h"

#include <QObject>
#include <QJsonArray>
#include <QGraphicsScene>

class EntityStateData : public IStateData
{
	EntityDopeSheetScene									m_DopeSheetScene;	// This stores the framesPerSecond and property key frames for this state

public:
	EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~EntityStateData();

	EntityDopeSheetScene &GetDopeSheetScene();
	const EntityDopeSheetScene &GetDopeSheetScene() const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	bool													m_bCtor;					// When true, "Set Constructor Mode" is on and the state is set to '0' and frame set to 'C' aka -1
	int														m_iCtorRestoreState;

	// These maps store the CONSTRUCTOR property data for the entire entity
	// HACK: These m_Ctor* member variables are declared before 'm_TreeModel' because its constructor needs to use/initialize these maps
	QMap<EntityTreeItemData *, QJsonObject>					m_CtorKeyFramesMap;			// Store properties and tween values
	QMap<EntityTreeItemData *, QJsonObject>					m_CtorPoppedKeyFramesMap;	// Keep removed items' keyframes, in case they are re-added with UNDO
	QList<QString *>										m_CtorCallbacksList;		// a list of strings that are the callback name(s) - still referenced from 'm_CallbacksList'

	EntityTreeModel											m_TreeModel;

	// This 'AuxWidgetsModel' is used to map (QDataWidgetMapper) to the widgets in AuxDopeSheet. It also contains the data for the widgets
	class AuxWidgetsModel : public QAbstractTableModel
	{
		EntityModel &										m_EntityModelRef;
		int													m_iFramesPerSecond;
		bool 												m_bAutoInitialize;
	public:
		AuxWidgetsModel(EntityModel &entityModelRef, int iFramesPerSecond, bool bAutoInitialize);
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const override;
		virtual bool setData(const QModelIndex &modelIndex, const QVariant &value, int role = Qt::EditRole) override;
	};
	AuxWidgetsModel											m_AuxWidgetsModel;

	QList<QString *>										m_CallbacksList;	// All callbacks (aka QString) is referenced from this list. 'EntityDopeSheetScene' will create and modify its state's callbacks using this list.

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	EntityTreeModel &GetTreeModel();

	QAbstractItemModel *GetAuxWidgetsModel();

	bool IsCtor() const;
	void SetCtor(bool bCtor);
	QMap<EntityTreeItemData *, QJsonObject> &GetCtorKeyFramesMap();
	QList<QString *> &GetCtorCallbacksList();
	QList<QString *> &GetCallbacksList();

	int GetFinalFrameIndex(int iStateIndex) const;
	int GetFramesPerSecond() const;
	bool IsAutoInitialize() const;

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from UndoCmd's
	QList<EntityTreeItemData *> Cmd_CreateNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	QList<EntityTreeItemData *> Cmd_CreateNewAssets(QList<IAssetItemData *> assetItemList, int iRow);
	EntityTreeItemData *Cmd_AddExistingItem(QJsonObject descObj, bool bIsArrayItem, int iRow); // If a newly created ArrayFolder is needed, it'll be placed at 'iRow'. If ArrayFolder already exists, 'iRow' is the row within the ArrayFolder
	EntityTreeItemData *Cmd_CreateNewWidget(ItemType eWidgetType, int iRow);
	EntityTreeItemData *Cmd_CreateNewShape(int iStateIndex, int iFrameIndex, EditorShape eShape, bool bIsPrimitive, int iRow);
	QList<EntityTreeItemData *> Cmd_AddNewPasteItems(QJsonObject mimeObject, EntityTreeItemData *pArrayFolder);
	QList<EntityTreeItemData *> Cmd_CreateNewArray(QList<EntityTreeItemData *> itemDataList, QString sArrayName, int iArrayFolderRow); // It is assumed that the items within 'itemDataList' have been removed/popped prior
	void Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList);
	int32 Cmd_RemoveTreeItem(EntityTreeItemData *pItem);
	bool Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow);
	void Cmd_RenameItem(EntityTreeItemData *pItemData, QString sNewName);

	QString GenerateCodeName(QString sDesiredName) const;

	QString GenerateSrc_FileIncludes() const;
	QString GenerateSrc_StateEnums() const;
	QString GenerateSrc_MemberVariables() const;
	QString GenerateSrc_AccessorDecl() const;
	QString GenerateSrc_AccessorDefinition(QString sClassName) const;
	QString GenerateSrc_CallbacksDecl() const;
	QString GenerateSrc_MemberInitializerList() const;
	QString GenerateSrc_Ctor() const;
	QString GenerateSrc_SetStateImpl() const;
	QString GenerateSrc_SetProperties(EntityTreeItemData *pItemData, QJsonObject propObj, QString sNewLine) const;
	QString GenerateSrc_TimelineAdvance() const; // Advance runtime by 1 frame on the timeline

	virtual void OnPopState(int iPoppedStateIndex) override;
	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual void OnItemDeleted() override;
};

#endif // ENTITYMODEL_H
