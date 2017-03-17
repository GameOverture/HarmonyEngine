/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEITEM_H
#define SPRITEITEM_H

#include "IProjItem.h"
#include "SpriteWidget.h"

#include <QStringListModel>
#include <QStandardItemModel>
#include <QDataWidgetMapper>

class AtlasesWidget;

class SpriteFrame
{
public:
    AtlasFrame *            m_pFrame;
    int                     m_iRowIndex;
    
    QPoint                  m_vOffset;
    float                   m_fDuration;
    
    SpriteFrame(AtlasFrame *pFrame, int iRowIndex) :    m_pFrame(pFrame),
                                                        m_iRowIndex(iRowIndex),
                                                        m_vOffset(0, 0),
                                                        m_fDuration(0.016f)
    { }

    QPoint GetRenderOffset()
    {
        QPoint ptRenderOffset;
        
        ptRenderOffset.setX(m_vOffset.x() + m_pFrame->GetCrop().left());
        ptRenderOffset.setY(m_vOffset.y() + (m_pFrame->GetSize().height() - m_pFrame->GetCrop().bottom()));

        return ptRenderOffset;
    }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpriteTableModel : public QAbstractTableModel
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

    SpriteTableModel(QObject *pParent);

    int Add(AtlasFrame *pFrame);
    void Remove(AtlasFrame *pFrame);
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    void RefreshFrame(AtlasFrame *pFrame);
    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

    SpriteFrame *GetFrameAt(int iIndex);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;

Q_SIGNALS:
    void editCompleted(const QString &);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpriteStateModel : public QAbstractListModel
{
    Q_OBJECT
    
    struct AnimState
    {
        QString             sName;
        bool                bBounce;
        bool                bLoop;
        bool                bReverse;
        SpriteTableModel *  framesModel;
    };
    QList<AnimState *>      m_StateList;
    
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpriteItem : public IProjItem
{
    Q_OBJECT

    SpriteStateModel *      m_pStateNameModel;
    
public:
    SpriteItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~SpriteItem();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu) override;

    virtual void OnLink(AtlasFrame *pFrame) override;
    virtual void OnUnlink(AtlasFrame *pFrame) override;
    
    virtual QJsonValue OnSave() override;
};

#endif // SPRITEITEM_H
