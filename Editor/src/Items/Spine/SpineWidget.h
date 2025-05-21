/**************************************************************************
*	SpineWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINEWIDGET_H
#define SPINEWIDGET_H

#include "SpineModel.h"

#include <QWidget>
#include <QTableView>
#include "ui_SpineWidget.h"

#include <QStyledItemDelegate>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//class SpineCrossFadesTableView : public QTableView
//{
//	Q_OBJECT
//
//public:
//	SpineCrossFadesTableView(QWidget *pParent = 0);
//
//protected:
//	virtual void resizeEvent(QResizeEvent *pResizeEvent);
//};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpineCrossFadesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	ProjectItemData *m_pItem;
	QTableView *m_pTableView;

public:
	SpineCrossFadesDelegate(ProjectItemData *pItem, QTableView *pTableView, QObject *pParent = 0);

	virtual QWidget *createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpineWidget : public IWidget
{
	Q_OBJECT

public:
	SpineWidget(ProjectItemData &itemRef, QWidget *parent = nullptr);
	~SpineWidget();

	virtual void OnGiveMenuActions(QMenu *pMenu) override;
	virtual void OnUpdateActions() override;
	virtual void OnFocusState(int iStateIndex, QVariant subState) override;

private Q_SLOTS:
	void on_sbDefaultMix_valueChanged(double dValue);
	void on_mixTableView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection);
	void on_btnAddMix_clicked();

	void on_actionAddMix_triggered();
	void on_actionRemoveMix_triggered();
	void on_actionOrderMixUp_triggered();
	void on_actionOrderMixDown_triggered();

private:
	Ui::SpineWidget *ui;
};

#endif // SPINEWIDGET_H
