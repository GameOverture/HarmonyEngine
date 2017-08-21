#include "EntityModel.h"

EntityStateData::EntityStateData(IModel &modelRef, QJsonObject stateObj) : IStateData(modelRef, stateObj["name"].toString())
{
}

/*virtual*/ EntityStateData::~EntityStateData()
{
}

/*virtual*/ void EntityStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
}

/*virtual*/ void EntityStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItem *pItem, QJsonArray stateArray) :   IModel(pItem),
                                                                        m_TreeModel(*pItem)
{

}

/*virtual*/ EntityModel::~EntityModel()
{
}

EntityTreeModel &EntityModel::GetTreeModel()
{
    return m_TreeModel;
}

/*virtual*/ void EntityModel::OnSave() /*override*/
{
}

/*virtual*/ QJsonObject EntityModel::PopStateAt(uint32 uiIndex) /*override*/
{
    return QJsonObject();
}

/*virtual*/ QJsonValue EntityModel::GetJson() const /*override*/
{
    return QJsonValue();
}

/*virtual*/ QList<AtlasFrame *> EntityModel::GetAtlasFrames() const /*override*/
{
    return QList<AtlasFrame *>();
}

/*virtual*/ QStringList EntityModel::GetFontUrls() const /*override*/
{
    return QStringList();
}

/*virtual*/ void EntityModel::Refresh() /*override*/
{
}
