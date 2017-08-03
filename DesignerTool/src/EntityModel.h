#ifndef ENTITYMODEL_H
#define ENTITYMODEL_H

#include "IModel.h"

#include <QObject>
#include <QJsonArray>

class EntityStateData : public IStateData
{
    CheckBoxMapper *    m_pChkMapper_Loop;
    CheckBoxMapper *    m_pChkMapper_Reverse;
    CheckBoxMapper *    m_pChkMapper_Bounce;

public:
    EntityStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~EntityStateData();

    CheckBoxMapper *GetLoopMapper();
    CheckBoxMapper *GetReverseMapper();
    CheckBoxMapper *GetBounceMapper();

    void GetStateInfo(QJsonObject &stateObjOut);

    void Refresh();

    virtual void AddFrame(AtlasFrame *pFrame) override;
    virtual void RelinquishFrame(AtlasFrame *pFrame) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
    Q_OBJECT

public:
    EntityModel(ProjectItem *pItem, QJsonArray stateArray);
    virtual ~EntityModel();

    virtual QJsonObject PopStateAt(uint32 uiIndex) override;
    virtual QJsonValue GetSaveInfo(bool bWritingToGameData) override;
    virtual void Refresh() override;
};

#endif // ENTITYMODEL_H
