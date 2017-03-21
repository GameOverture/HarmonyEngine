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

    QJsonArray GetFramesInfo(float &fTotalDurationRef);
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
class SpriteStateData : public IStateData
{
    CheckBoxMapper *    m_pChkMapper_Loop;
    CheckBoxMapper *    m_pChkMapper_Reverse;
    CheckBoxMapper *    m_pChkMapper_Bounce;
    SpriteFramesModel * m_pFramesModel;

public:
    SpriteStateData(IModel &modelRef, QJsonObject stateObj) :   IStateData(modelRef, stateObj["name"].toString()),
                                                                m_pChkMapper_Loop(nullptr),
                                                                m_pChkMapper_Reverse(nullptr),
                                                                m_pChkMapper_Bounce(nullptr),
                                                                m_pFramesModel(nullptr)
    {
        m_pChkMapper_Loop = new CheckBoxMapper(&m_ModelRef);
        m_pChkMapper_Reverse = new CheckBoxMapper(&m_ModelRef);
        m_pChkMapper_Bounce = new CheckBoxMapper(&m_ModelRef);
        m_pFramesModel = new SpriteFramesModel(&m_ModelRef);
        
        if(stateObj.empty() == false)
        {
            m_pChkMapper_Loop->SetChecked(stateObj["loop"].toBool());
            m_pChkMapper_Reverse->SetChecked(stateObj["reverse"].toBool());
            m_pChkMapper_Bounce->SetChecked(stateObj["bounce"].toBool());
            
            QJsonArray spriteFrameArray = stateObj["frames"].toArray();
            QList<quint32> checksumRequestList;
            for(int i = 0; i < spriteFrameArray.size(); ++i)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[i].toObject();
                checksumRequestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
            }
    
            
            QList<AtlasFrame *> requestedAtlasFramesList = m_ModelRef.RequestFrames(this, checksumRequestList);
            if(spriteFrameArray.size() != requestedAtlasFramesList.size())
                HyGuiLog("SpriteStatesModel::AppendState() failed to acquire all the stored frames", LOGTYPE_Error);
    
            for(int i = 0; i < requestedAtlasFramesList.size(); ++i)
            {
                QJsonObject spriteFrameObj = spriteFrameArray[i].toObject();
                QPoint vOffset(spriteFrameObj["offsetX"].toInt() - requestedAtlasFramesList[i]->GetCrop().left(),
                               spriteFrameObj["offsetY"].toInt() - (requestedAtlasFramesList[i]->GetSize().height() - requestedAtlasFramesList[i]->GetCrop().bottom()));
    
                m_pFramesModel->OffsetFrame(i, vOffset);
                m_pFramesModel->DurationFrame(i, spriteFrameObj["duration"].toDouble());
            }
        }
        else
        {
            m_pChkMapper_Loop->SetChecked(false);
            m_pChkMapper_Reverse->SetChecked(false);
            m_pChkMapper_Bounce->SetChecked(false);
        }
    }
    
    CheckBoxMapper *GetLoopMapper()     { return m_pChkMapper_Loop; }
    CheckBoxMapper *GetReverseMapper()  { return m_pChkMapper_Reverse; }
    CheckBoxMapper *GetBounceMapper()   { return m_pChkMapper_Bounce; }
    SpriteFramesModel *GetFramesModel() { return m_pFramesModel; }

    void GetStateInfo(QJsonObject &stateObjOut)
    {
        QJsonArray frameArray;
        float fTotalDuration = 0.0f;
        for(int i = 0; i < m_pFramesModel->rowCount(); ++i)
        {
            SpriteFrame *pSpriteFrame = m_pFramesModel->GetFrameAt(i);

            QJsonObject frameObj;
            frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
            fTotalDuration += pSpriteFrame->m_fDuration;
            frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x() + pSpriteFrame->m_pFrame->GetCrop().left()));
            frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y() + (pSpriteFrame->m_pFrame->GetSize().height() - pSpriteFrame->m_pFrame->GetCrop().bottom())));
            frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetChecksum())));

            frameArray.append(frameObj);
        }

        stateObjOut.insert("name", QJsonValue(GetName()));
        stateObjOut.insert("loop", m_pChkMapper_Loop->IsChecked());
        stateObjOut.insert("reverse", m_pChkMapper_Reverse->IsChecked());
        stateObjOut.insert("bounce", m_pChkMapper_Bounce->IsChecked());
        stateObjOut.insert("duration", QJsonValue(fTotalDuration));
        stateObjOut.insert("frames", QJsonValue(frameArray));
    }
    
    virtual void AddFrame(AtlasFrame *pFrame)
    {
        m_pFramesModel->Add(pFrame);
    }
    
    virtual void RelinquishFrame(AtlasFrame *pFrame)
    {
        m_pFramesModel->Remove(pFrame);
    }
    
    virtual void RefreshFrame(AtlasFrame *pFrame)
    {
        m_pFramesModel->RefreshFrame(pFrame);
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteModel : public IModel
{
    Q_OBJECT

public:
    SpriteModel(ProjectItem *pItem, QJsonArray stateArray);
    virtual ~SpriteModel();
    
    virtual int AppendState(QJsonObject stateObj);
    virtual void InsertState(int iStateIndex, QJsonObject stateObj);
    virtual QJsonObject PopStateAt(uint32 uiIndex);
    
    virtual QJsonValue GetSaveInfo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPRITEMODELS_H
