/**************************************************************************
 *	WidgetSpriteModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITEMODELVIEW_H
#define WIDGETSPRITEMODELVIEW_H

#include "HyGuiFrame.h"

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QTableView>
#include <QStyledItemDelegate>

class ItemSprite;
struct SpriteFrame;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteTableView : public QTableView
{
    Q_OBJECT

public:
    WidgetSpriteTableView(QWidget *pParent = 0);

protected:
    virtual void resizeEvent(QResizeEvent *pResizeEvent);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    ItemSprite *                m_pItemSprite;
    WidgetSpriteTableView *     m_pTableView;

public:
    WidgetSpriteDelegate(ItemSprite *pItemSprite, WidgetSpriteTableView *pTableView, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteModel : public QAbstractTableModel
{
    Q_OBJECT

    QList<SpriteFrame *>            m_FramesList;
    QMap<quint32, SpriteFrame *>    m_RemovedFrameMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes


public:
    enum eColumn
    {
        COLUMN_Frame = 0,
        COLUMN_OffsetX,
        COLUMN_OffsetY,
        COLUMN_Duration,

        NUMCOLUMNS
    };

    WidgetSpriteModel(QObject *pParent);

    int Add(HyGuiFrame *pFrame);
    void Remove(HyGuiFrame *pFrame);
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    void RefreshFrame(HyGuiFrame *pFrame);
    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

    SpriteFrame *GetFrameAt(int iIndex);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;

Q_SIGNALS:
    void editCompleted(const QString &);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // WIDGETSPRITEMODELVIEW_H
