/**************************************************************************
*	PropertiesTreeView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESTREEVIEW_H
#define PROPERTIESTREEVIEW_H

#include <QObject>
#include <QTreeView>
#include <QStyledItemDelegate>

class PropertiesTreeView : public QTreeView
{
public:
	PropertiesTreeView(QWidget *pParent = nullptr);
	virtual ~PropertiesTreeView();

	virtual void setModel(QAbstractItemModel *pModel) override;
	virtual void paintEvent(QPaintEvent *pEvent) override;

protected:
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PropertiesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	PropertiesTreeView *	m_pTableView;

public:
	PropertiesDelegate(PropertiesTreeView *pTableView, QObject *pParent = 0);

	virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const override;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const override;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected Q_SLOTS:
	void OnComboBoxEditorSubmit(int iIndex);
};

#endif // PROPERTIESTREEVIEW_H
