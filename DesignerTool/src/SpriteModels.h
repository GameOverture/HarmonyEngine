/**************************************************************************
 *	SpriteModels.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEMODELS_H
#define SPRITEMODELS_H

#include "AtlasFrame.h"

#include <QObject>

class SpriteItem;

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

class SpriteFramesModel : public QAbstractTableModel
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

    SpriteFramesModel(QObject *pParent);

    int Add(AtlasFrame *pFrame);
    void Remove(AtlasFrame *pFrame);
    void MoveRowUp(int iIndex);
    void MoveRowDown(int iIndex);
    void RefreshFrame(AtlasFrame *pFrame);
    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

    QJsonArray GetFrames(float &fTotalDurationRef);
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

class SpriteStatesModel : public QAbstractListModel
{
    Q_OBJECT

    SpriteItem *            m_pSpriteItem;

    struct AnimState
    {
        QString             sName;
        bool                bBounce;
        bool                bLoop;
        bool                bReverse;
        SpriteFramesModel *  pFramesModel;
    };
    QList<AnimState *>      m_StateList;

public:
    SpriteStatesModel(SpriteItem *pSpriteItem);

    int AppendState(QJsonObject stateObj);
    void InsertState(int iStateIndex, QJsonObject stateObj);
    QJsonObject PopStateAt(uint32 uiIndex);

    QString SetStateName(int iStateIndex, QString sNewName);
    void MoveStateBack(int iStateIndex);
    void MoveStateForward(int iStateIndex);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPRITEMODELS_H
