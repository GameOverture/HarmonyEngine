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
#include "ProjectModel.h"

#include <QObject>
#include <QDataWidgetMapper>
#include <QCheckBox>
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
class CheckBoxMapper : public QDataWidgetMapper
{
    class ModelCheckBox : public QAbstractListModel
    {
    public:
        ModelCheckBox(QObject *pParent = nullptr) : QAbstractListModel(pParent)
        { }

        virtual ~ModelCheckBox()
        { }

        virtual int ModelCheckBox::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
        {
            return 2;
        }

        virtual QVariant ModelCheckBox::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
        {
            return index.row() == 0 ? false : true;
        }
    };

public:
    CheckBoxMapper(QObject *pParent = nullptr) : QDataWidgetMapper(pParent)
    {
        setModel(new ModelCheckBox(this));
    }
    virtual ~CheckBoxMapper()
    { }

    void AddCheckBoxMapping(QCheckBox *pCheckBox)
    {
        addMapping(pCheckBox, 0);
        this->setCurrentIndex(this->currentIndex());
    }

    bool IsChecked()
    {
        return currentIndex() == 0 ? false : true;
    }

    void SetChecked(bool bChecked)
    {
        setCurrentIndex(bChecked ? 1 : 0);
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SpriteStateData
{
    QString             sName;
    CheckBoxMapper *    pLoopMapper;
    CheckBoxMapper *    pReverseMapper;
    CheckBoxMapper *    pBounceMapper;
    SpriteFramesModel * pFramesModel;

    SpriteStateData() : pLoopMapper(nullptr),
                        pReverseMapper(nullptr),
                        pBounceMapper(nullptr),
                        pFramesModel(nullptr)
    { }

    void GetStateInfo(QJsonObject &stateObjOut)
    {
        QJsonArray frameArray;
        float fTotalDuration = 0.0f;
        for(int i = 0; i < pFramesModel->rowCount(); ++i)
        {
            SpriteFrame *pSpriteFrame = pFramesModel->GetFrameAt(i);

            QJsonObject frameObj;
            frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
            fTotalDuration += pSpriteFrame->m_fDuration;
            frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x() + pSpriteFrame->m_pFrame->GetCrop().left()));
            frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y() + (pSpriteFrame->m_pFrame->GetSize().height() - pSpriteFrame->m_pFrame->GetCrop().bottom())));
            frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetChecksum())));

            frameArray.append(frameObj);
        }

        stateObjOut.insert("name", QJsonValue(sName));
        stateObjOut.insert("loop", pLoopMapper->IsChecked());
        stateObjOut.insert("reverse", pReverseMapper->IsChecked());
        stateObjOut.insert("bounce", pBounceMapper->IsChecked());
        stateObjOut.insert("duration", QJsonValue(fTotalDuration));
        stateObjOut.insert("frames", QJsonValue(frameArray));
    }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteModel : public QAbstractListModel
{
    Q_OBJECT

    ProjectItem *               m_pItem;
    QList<SpriteStateData *>    m_StateList;

public:
    SpriteModel(ProjectItem *pItem, QJsonArray stateArray);
    virtual ~SpriteModel();

    int GetNumStates();
    SpriteStateData *GetStateData(int iStateIndex);

    QList<AtlasFrame *> RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList);
    void RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList);

    void RelinkFrame(AtlasFrame *pFrame);

    int AppendState(QJsonObject stateObj);
    void InsertState(int iStateIndex, QJsonObject stateObj);
    QJsonObject PopStateAt(uint32 uiIndex);

    QString SetStateName(int iStateIndex, QString sNewName);
    void MoveStateBack(int iStateIndex);
    void MoveStateForward(int iStateIndex);

    QJsonArray GetSaveInfo();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPRITEMODELS_H
