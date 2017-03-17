/**************************************************************************
 *	WidgetSpriteModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEMODELVIEW_H
#define SPRITEMODELVIEW_H

#include "AtlasFrame.h"

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QTableView>
#include <QStyledItemDelegate>

class SpriteItem;
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

    SpriteItem *                m_pItemSprite;
    SpriteTableView *     m_pTableView;

public:
    WidgetSpriteDelegate(SpriteItem *pItemSprite, SpriteTableView *pTableView, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SPRITEMODELVIEW_H
