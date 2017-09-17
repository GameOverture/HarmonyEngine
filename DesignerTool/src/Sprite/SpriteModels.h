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

#include "IModel.h"
#include "AtlasFrame.h"
#include "GlobalWidgetMappers.h"

#include <QObject>
#include <QJsonArray>

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
    QMap<quint32, SpriteFrame *>    m_RemovedFrameIdMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes


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
    void OffsetFrame(int iIndex, QPoint vOffset);       // iIndex of -1 will apply to all
    void DurationFrame(int iIndex, float fDuration);    // iIndex of -1 will apply to all

    QJsonArray GetFramesInfo(float &fTotalDurationRef);
    SpriteFrame *GetFrameAt(int iIndex);

    void Refresh();

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
class SpriteStateData : public IStateData
{
    CheckBoxMapper *    m_pChkMapper_Loop;
    CheckBoxMapper *    m_pChkMapper_Reverse;
    CheckBoxMapper *    m_pChkMapper_Bounce;
    SpriteFramesModel * m_pFramesModel;

public:
    SpriteStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
    virtual ~SpriteStateData();
    
    CheckBoxMapper *GetLoopMapper();
    CheckBoxMapper *GetReverseMapper();
    CheckBoxMapper *GetBounceMapper();
    SpriteFramesModel *GetFramesModel();

    void GetStateInfo(QJsonObject &stateObjOut);

    QSet<AtlasFrame *> GetAtlasFrames();

    void Refresh();
    
    virtual void AddFrame(AtlasFrame *pFrame) override;
    virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteModel : public IModel
{
    Q_OBJECT

public:
    SpriteModel(ProjectItem &itemRef, QJsonArray stateArray);
    virtual ~SpriteModel();
    
    virtual void OnSave() override;
    virtual QJsonObject PopStateAt(uint32 uiIndex) override;
    virtual QJsonValue GetJson() const override;
    virtual QList<AtlasFrame *> GetAtlasFrames() const override;
    virtual QStringList GetFontUrls() const override;
    virtual void Refresh() override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPRITEMODELS_H
