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

class ItemSprite;
struct SpriteFrame;

namespace Ui {
class WidgetSpriteState;
}

class WidgetSprite;

class WidgetSpriteStateTableView : public QTableView
{
    Q_OBJECT
    
public:
    WidgetSpriteStateTableView(QWidget *pParent = 0);
    
protected:
    virtual void resizeEvent(QResizeEvent *pResizeEvent);
};

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


class WidgetSpriteState : public QWidget
{
    Q_OBJECT

    WidgetSprite *                      m_pOwner;
    SpriteFramesModel *                 m_pSpriteFramesModel;
    
    QString                             m_sName;

public:
    explicit WidgetSpriteState(WidgetSprite *pOwner, QList<QAction *> stateActionList, QWidget *parent = 0);
    ~WidgetSpriteState();
    
    QString GetName();
    void SetName(QString sNewName);

    void InsertFrame(HyGuiFrame *pFrame);
    void RemoveFrame(HyGuiFrame *pFrame);

    SpriteFrame *GetSelectedFrame();
    int GetSelectedIndex();
    void SelectIndex(int iIndex);
    int GetNumFrames();
    void OrderFrame(int iIndex, int iOffset);
    
    void AppendFramesToListRef(QList<HyGuiFrame *> &drawInstListRef);
    
    void GetStateFrameInfo(QJsonObject &stateObjOut);

private slots:
    void on_framesView_itemSelectionChanged(QModelIndex current, QModelIndex previous);
    
private:
    Ui::WidgetSpriteState *ui;
};
Q_DECLARE_METATYPE(WidgetSpriteState *)

#endif // WIDGETSPRITESTATE_H
