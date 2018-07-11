/**************************************************************************
*	PropertiesTreeView.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
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
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PropertiesDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	PropertiesTreeView *    m_pTableView;

public:
	PropertiesDelegate(PropertiesTreeView *pTableView, QObject *pParent = 0);

	virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PROPERTIESTREEVIEW_H
