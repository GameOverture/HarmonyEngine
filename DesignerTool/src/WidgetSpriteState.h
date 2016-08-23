/**************************************************************************
 *	WidgetSpriteState.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITESTATE_H
#define WIDGETSPRITESTATE_H

#include "HyGuiFrame.h"

#include <QWidget>
#include <QResizeEvent>
#include <QTableView>
#include <QStyledItemDelegate>

class ItemSprite;
struct SpriteFrame;

namespace Ui {
class WidgetSpriteState;
}

class WidgetSprite;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteStateTableView : public QTableView
{
    Q_OBJECT
    
public:
    WidgetSpriteStateTableView(QWidget *pParent = 0);
    
protected:
    virtual void resizeEvent(QResizeEvent *pResizeEvent);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteStateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    WidgetSpriteStateDelegate(QObject *pParent = 0);
    
    virtual QWidget* createEditor(QWidget *pParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *pEditor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *pEditor, QAbstractItemModel *pModel, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *pEditor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteFramesModel : public QAbstractTableModel
{
    Q_OBJECT
    
    QList<SpriteFrame *>            m_FramesList;
    QMap<quint32, SpriteFrame *>    m_RemovedFrameMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes
    
    
public:
    enum eColumn
    {
        COLUMN_Frame = 0,
        COLUMN_Offset,
        COLUMN_Rotation,
        COLUMN_Scale,
        COLUMN_Duration,

        NUMCOLUMNS
    };

    SpriteFramesModel(QObject *pParent);
    
    void Add(HyGuiFrame *pFrame);
    void Remove(HyGuiFrame *pFrame);
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    void TranslateFrame(int iIndex, QPointF ptPos); // iIndex of -1 will apply to all
    void RotateFrame(int iIndex, float fRot);       // iIndex of -1 will apply to all
    void ScaleFrame(int iIndex, QPointF vScale);    // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);// iIndex of -1 will apply to all
    
    SpriteFrame *GetFrameAt(int iIndex);
    
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual Qt::ItemFlags flags(const QModelIndex & index) const;
    
signals:
    void editCompleted(const QString &);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteState : public QWidget
{
    Q_OBJECT

    WidgetSprite *                      m_pOwner;
    SpriteFramesModel *                 m_pSpriteFramesModel;
    
    QString                             m_sName;
    
    bool                                m_bPlayActive;
    double                              m_dElapsedTime;
    bool                                m_bIsBounced;

public:
    explicit WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();
    
    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);

    SpriteFrame *GetSelectedFrame();
    int GetSelectedIndex();
    int GetNumFrames();
    
    void AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef);
    
    void GetStateFrameInfo(QJsonObject &stateObjOut);
    
    void UpdateTimeStep(double dDelta);

private slots:
    void on_framesView_itemSelectionChanged(QModelIndex current, QModelIndex previous);
    
    void on_actionPlay_triggered();
    
    void on_btnHz10_clicked();
    
    void on_btnHz20_clicked();
    
    void on_btnHz30_clicked();
    
    void on_btnHz40_clicked();
    
    void on_btnHz50_clicked();
    
    void on_btnHz60_clicked();
    
    void on_actionOrderFrameUpwards_triggered();
    
    void on_actionOrderFrameDownwards_triggered();
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
