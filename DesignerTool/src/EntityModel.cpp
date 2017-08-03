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

EntityModel::EntityModel(ProjectItem *pItem, QJsonArray stateArray) : IModel(pItem)
{

}

/*virtual*/ EntityModel::~EntityModel()
{
}

/*virtual*/ QJsonObject EntityModel::PopStateAt(uint32 uiIndex) /*override*/
{
    return QJsonObject();
}

/*virtual*/ QJsonValue EntityModel::GetSaveInfo(bool bWritingToGameData) /*override*/
{
    return QJsonValue();
}

/*virtual*/ void EntityModel::Refresh() /*override*/
{
}
