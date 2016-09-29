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

public:
    WidgetFontDelegate(ItemFont *pItemFont, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetFontModel : public QAbstractTableModel
{
    Q_OBJECT

    int                             m_iUniqueIdCounter;

    struct FontStage
    {
        const int           iUNIQUE_ID;

        rendermode_t        eMode;
        float               fSize;
        float               fOutlineThickness;

        QColor              topColor;
        QColor              botColor;

        texture_font_t *    pTextureFont;

        FontStage(int uiId, rendermode_t eRenderMode, float fSize, float fOutlineThickness, QColor topColor, QColor botColor) : iUNIQUE_ID(uiId),
                                                                                                                                eMode(eRenderMode),
                                                                                                                                fSize(fSize),
                                                                                                                                fOutlineThickness(fOutlineThickness),
                                                                                                                                topColor(topColor),
                                                                                                                                botColor(botColor),
                                                                                                                                pTextureFont(NULL)
        { }
    };
    QList<FontStage *>              m_StageList;
    QList<QPair<FontStage *, int> > m_RemovedStageList;

    QString                         m_sRenderModeStrings[5];
    
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

    QString GetRenderModeString(rendermode_t eMode) const;

    int AddNewStage(rendermode_t eRenderMode, float fSize, float fOutlineThickness, QColor topColor, QColor botColor);
    void AddExistingStage(int iId);
    void RemoveStage(int iId);

    int GetStageId(int iRowIndex) const;

    rendermode_t GetStageRenderMode(int iRowIndex) const;
    void SetStageRenderMode(int iRowIndex, rendermode_t eRenderMode);

    float GetStageSize(int iRowIndex) const;
    void SetStageSize(int iRowIndex, float fSize);

    float GetStageOutlineThickness(int iRowIndex) const;
    void SetStageOutlineThickness(int iRowIndex, float fThickness);

    void SetTextureFont(int iRowIndex, texture_font_t *pTextureFont);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
};

#endif // WIDGETFONTMODELVIEW_H
