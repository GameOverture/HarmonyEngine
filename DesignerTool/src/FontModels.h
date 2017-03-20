/**************************************************************************
 *	FontModels.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELS_H
#define FONTMODELS_H

#include "ProjectItem.h"
#include <QAbstractListModel>

class FontModel : public QAbstractListModel
{
    Q_OBJECT

    ProjectItem *               m_pItem;

public:
    FontModel(ProjectItem *pItem, QJsonObject stateArray);
    virtual ~FontModel();

    int GetNumStates();
    //SpriteStateData *GetStateData(int iStateIndex);

    QList<AtlasFrame *> RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList);
    void RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList);

    void RefreshFrame(AtlasFrame *pFrame);

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

#endif // FONTMODELS_H
