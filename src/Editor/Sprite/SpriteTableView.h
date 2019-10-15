/**************************************************************************
 *	SpriteTableView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITETABLEVIEW_H
#define SPRITETABLEVIEW_H

#include "AtlasFrame.h"

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QTableView>
#include <QStyledItemDelegate>

class SpriteFrame;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteTableView : public QTableView
{
	Q_OBJECT

public:
	SpriteTableView(QWidget *pParent = 0);

protected:
	virtual void resizeEvent(QResizeEvent *pResizeEvent);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteDelegate : public QStyledItemDelegate
{
	Q_OBJECT

	ProjectItem *         m_pItem;
	SpriteTableView *     m_pTableView;

public:
	WidgetSpriteDelegate(ProjectItem *pItem, SpriteTableView *pTableView, QObject *pParent = 0);

	virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SPRITETABLEVIEW_H
