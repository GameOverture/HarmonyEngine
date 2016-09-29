/**************************************************************************
 *	WidgetFontModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETFONTMODELVIEW_H
#define WIDGETFONTMODELVIEW_H

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QStyledItemDelegate>

#include "ItemFont.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontTableView : public QTableView
{
    Q_OBJECT

 public:
     WidgetFontTableView(QWidget *pParent = 0);

 protected:
     virtual void resizeEvent(QResizeEvent *pResizeEvent);
 };
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    ItemFont *              m_pItemFont;
    WidgetFontTableView *   m_pTableView;

public:
    WidgetFontDelegate(ItemFont *pItemFont, WidgetFontTableView *pTableView, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontModel : public QAbstractTableModel
{
    Q_OBJECT
    
    QList<FontStage *>      m_StageList;
    QList<FontStage *>      m_RemovedStageList;
    
public:
    enum eColumn
    {
        COLUMN_Type = 0,
        COLUMN_Size,
        COLUMN_Thickness,
        COLUMN_DefaultColor,

        NUMCOLUMNS
    };
    
    WidgetFontModel(QObject *parent);
    virtual ~WidgetFontModel();

    FontStage *AddStage(FontStage::eType eRenderType, float fSize, float fOutlineThickness, QColor topColor, QColor botColor);
    void AddStage(FontStage *pExistingStage, int iRowIndex);
    int RemoveStage(FontStage *pStage);

    FontStage *GetStageAt(int iIndex);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
};

#endif // WIDGETFONTMODELVIEW_H
