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

#include "ItemFont.h"
#include "freetype-gl/freetype-gl.h"

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QStyledItemDelegate>
#include <QColor>

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
    QComboBox *             m_pCmbStates;

public:
    WidgetFontDelegate(ItemFont *pItemFont, QComboBox *pCmbStates, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontModel : public QAbstractTableModel
{
    Q_OBJECT

    static int                      sm_iUniqueIdCounter;
    QList<FontLayer *>              m_LayerList;
    QList<QPair<int, FontLayer *> > m_RemovedLayerList; // QPair = <rowIndex, fontPtr>
    
    QStringList                     m_sRenderModeStringList;
    
public:
    enum eColumn
    {
        COLUMN_Type = 0,
        COLUMN_Thickness,
        COLUMN_DefaultColor,

        NUMCOLUMNS
    };
    
    WidgetFontModel(QObject *parent);
    virtual ~WidgetFontModel();

    QString GetRenderModeString(rendermode_t eMode) const;
    
    int AddNewLayer(rendermode_t eRenderMode, float fSize, float fOutlineThickness);
    void RemoveLayer(int iId);
    void ReAddLayer(int iId);

    int GetLayerId(int iRowIndex) const;
    
    rendermode_t GetLayerRenderMode(int iRowIndex) const;
    void SetLayerRenderMode(int iId, rendermode_t eMode);
    
    float GetLayerOutlineThickness(int iRowIndex) const;
    void SetLayerOutlineThickness(int iId, float fThickness);
    
    float GetLayerSize(int iRowIndex) const;

    //void SetTextureFont(int iRowIndex, texture_font_t *pTextureFont);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
};

#endif // WIDGETFONTMODELVIEW_H
