#ifndef ENTITYMODEL_H
#define ENTITYMODEL_H

#include "IModel.h"
#include "ProjectItem.h"
#include "EntityTreeModel.h"
#include "GlobalWidgetMappers.h"

#include <QObject>
#include <QJsonArray>

class EntityStateData : public IStateData
{
    QMap<EntityTreeItem *, PropertiesModel *>   m_PropertiesMap;

public:
    EntityStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~EntityStateData();

    PropertiesModel *GetPropertiesModel(EntityTreeItem *pTreeItem);
    void GetStateInfo(QJsonObject &stateObjOut);

    void Refresh();

    virtual void AddFrame(AtlasFrame *pFrame) override;
    virtual void RelinquishFrame(AtlasFrame *pFrame) override;

private:
    PropertiesModel *AllocNewPropertiesModel(ProjectItem *pProjItem);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
    Q_OBJECT

    EntityTreeModel         m_TreeModel;

public:
    EntityModel(ProjectItem *pItem, QJsonArray stateArray);
    virtual ~EntityModel();

    EntityTreeModel &GetTreeModel();
    PropertiesModel *GetPropertiesModel(int iStateIndex, EntityTreeItem *pTreeItem);

    virtual void OnSave() override;
    virtual QJsonObject PopStateAt(uint32 uiIndex) override;
    virtual QJsonValue GetJson() const override;
    virtual QList<AtlasFrame *> GetAtlasFrames() const override;
    virtual QStringList GetFontUrls() const override;
    virtual void Refresh() override;
};

#endif // ENTITYMODEL_H
