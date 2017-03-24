/**************************************************************************
 *	WidgetFontModelView.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELVIEW_H
#define FONTMODELVIEW_H

#include "FontItem.h"
#include "freetype-gl/freetype-gl.h"

#include <QWidget>
#include <QTableView>
#include <QResizeEvent>
#include <QStyledItemDelegate>
#include <QColor>

struct FontTypeface;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontTableView : public QTableView
{
    Q_OBJECT

 public:
     FontTableView(QWidget *pParent = 0);

 protected:
     virtual void resizeEvent(QResizeEvent *pResizeEvent) override;
 };
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    ProjectItem *           m_pItem;
    QComboBox *             m_pCmbStates;

public:
    FontDelegate(ProjectItem *pItem, QComboBox *pCmbStates, QObject *pParent = 0);

    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const override;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const override;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontTableModel : public QAbstractTableModel
{
    Q_OBJECT

    struct FontLayer
    {
        const int           iUNIQUE_ID;
        FontTypeface *     pReference;

        rendermode_t        eMode;
        float               fSize;
        float               fOutlineThickness;

        glm::vec4           vTopColor;
        glm::vec4           vBotColor;

        FontLayer(int iUniqueId, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :  iUNIQUE_ID(iUniqueId),
                                                                                                    pReference(NULL),
                                                                                                    eMode(eRenderMode),
                                                                                                    fSize(fSize),
                                                                                                    fOutlineThickness(fOutlineThickness)
        { }
    };

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
    
    FontTableModel(QObject *parent);
    virtual ~FontTableModel();

    QString GetRenderModeString(rendermode_t eMode) const;
    
    int AddNewLayer(rendermode_t eRenderMode, float fSize, float fOutlineThickness);
    void RemoveLayer(int iId);
    void ReAddLayer(int iId);

    int GetLayerId(int iRowIndex) const;
    FontTypeface *GetStageRef(int iRowIndex);
    
    rendermode_t GetLayerRenderMode(int iRowIndex) const;
    void SetLayerRenderMode(int iId, rendermode_t eMode);
    
    float GetLayerOutlineThickness(int iRowIndex) const;
    void SetLayerOutlineThickness(int iId, float fThickness);

    QColor GetLayerTopColor(int iRowIndex) const;
    QColor GetLayerBotColor(int iRowIndex) const;
    void SetLayerColors(int iId, QColor topColor, QColor botColor);
    
    float GetLineHeight();
    float GetLineAscender();
    float GetLineDescender();
    float GetLeftSideNudgeAmt(QString sAvailableTypefaceGlyphs);
    
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    
    void SetFontSize(float fSize);
    
    void SetFontStageReference(int iRowIndex, FontTypeface *pStageRef);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
};

#endif // FONTMODELVIEW_H
