/**************************************************************************
*	EntityWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYWIDGET_H
#define ENTITYWIDGET_H

#include "Global.h"
#include "EntityModel.h"
#include "IWidget.h"

#include <QWidget>
#include <QToolBar>
#include <QElapsedTimer>

namespace Ui {
class EntityWidget;
}

class PropertiesTreeMultiModel;

class EntityWidget : public IWidget
{
	Q_OBJECT

	QActionGroup						m_AddShapeActionGroup;
	QMenu								m_ContextMenu;

	EntityPropertiesTreeMultiModel *	m_pMultiPropModel;

	QTimer *							m_pPreviewUpdateTimer;
	QList<QUuid>						m_PreviewSelectedItemsList; // Used to restore what selected items were after previewing ends
	QElapsedTimer						m_PreviewElapsedTimer;
	int									m_iPreviewStartingFrame;

public:
	explicit EntityWidget(ProjectItemData &itemRef, QWidget *pParent = nullptr);
	~EntityWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

	QModelIndexList GetSelectedItems();
	void RequestSelectedItems(QList<QUuid> uuidList); // Will clear and select only what 'uuidList' contains
	void RequestSelectedItemChange(EntityTreeItemData *pTreeItemData, QItemSelectionModel::SelectionFlags flags);

	void SetExtrapolatedProperties();

	void CheckShapeAddBtn(EditorShape eShapeType, bool bAsPrimitive);
	void SetAsShapeEditMode(bool bEnableSem);
	void UncheckAll();

protected:
	virtual void showEvent(QShowEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *pEvent) override;

	void StopPreview();

public Q_SLOTS:
	void OnKeySpace();

private Q_SLOTS:
	void OnKeyQ();
	void OnKeyE();
	void OnKeyShiftQ();
	void OnKeyShiftE();
	void OnKeyF();

	void OnContextMenu(const QPoint &pos);
	void OnTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void OnCollapsedNode(const QModelIndex &indexRef);
	void on_actionAddChildren_triggered();

	void on_actionAddLabel_triggered();
	void on_actionAddRichLabel_triggered();
	void on_actionAddButton_triggered();
	void on_actionAddRackMeter_triggered();
	void on_actionAddBarMeter_triggered();
	void on_actionAddCheckBox_triggered();
	void on_actionAddRadioButton_triggered();
	void on_actionAddTextField_triggered();
	void on_actionAddComboBox_triggered();
	void on_actionAddSlider_triggered();
	
	void on_actionAddBoxPrimitive_triggered();
	void on_actionAddCirclePrimitive_triggered();
	void on_actionAddPolygonPrimitive_triggered();
	void on_actionAddSegmentPrimitive_triggered();
	void on_actionAddLineChainPrimitive_triggered();
	void on_actionAddCapsulePrimitive_triggered();

	void on_actionAddBoxShape_triggered();
	void on_actionAddCircleShape_triggered();
	void on_actionAddPolygonShape_triggered();
	void on_actionAddSegmentShape_triggered();
	void on_actionAddLineChainShape_triggered();
	void on_actionAddCapsuleShape_triggered();

	void on_actionVertexEditMode_toggled(bool bChecked);

	void on_actionOrderChildrenUp_triggered();
	void on_actionOrderChildrenDown_triggered();
	
	void on_actionReplaceItems_triggered();
	void on_actionRemoveItems_triggered();

	void on_actionConvertShape_triggered();

	void on_actionRenameItem_triggered();

	void on_actionUnpackFromArray_triggered();
	void on_actionPackToArray_triggered();
	void on_actionDuplicateToArray_triggered();
	
	void on_actionCutEntityItems_triggered();
	void on_actionCopyEntityItems_triggered();
	void on_actionPasteEntityItems_triggered();

	void OnPreviewUpdate();

private:
	Ui::EntityWidget *ui;
};

#endif // ENTITYWIDGET_H
