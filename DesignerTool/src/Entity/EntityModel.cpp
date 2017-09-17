#include "EntityModel.h"

#include <QVariant>

EntityStateData::EntityStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj) : IStateData(iStateIndex, modelRef, stateObj["name"].toString())
{
    if(stateObj.empty() == false)
    {
    }
    else
    {

    }
}

/*virtual*/ EntityStateData::~EntityStateData()
{
    for(auto iter = m_PropertiesMap.begin(); iter != m_PropertiesMap.end(); ++iter)
    {
        PropertiesTreeModel *pPropertiesModel = iter.value();
        delete pPropertiesModel;
    }

    m_PropertiesMap.clear();
}

PropertiesTreeModel *EntityStateData::GetPropertiesModel(EntityTreeItem *pTreeItem)
{
    if(pTreeItem == nullptr)
    {
        HyGuiLog("EntityStateData::GetPropertiesModel was given a nullptr", LOGTYPE_Error);
        return nullptr;
    }

    if(m_PropertiesMap.contains(pTreeItem) == false)
    {
        QVariant var(reinterpret_cast<qulonglong>(pTreeItem));
        m_PropertiesMap[pTreeItem] = AllocNewPropertiesModel(m_ModelRef.GetItem(), var, pTreeItem->GetItem()->GetType());
    }

    return m_PropertiesMap[pTreeItem];
}

/*virtual*/ void EntityStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
}

/*virtual*/ void EntityStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

PropertiesTreeModel *EntityStateData::AllocNewPropertiesModel(ProjectItem &entityItemRef, QVariant &subState, HyGuiItemType eSelectedType)
{
    PropertiesTreeModel *pNewPropertiesModel = new PropertiesTreeModel(entityItemRef, GetIndex(), subState);

    pNewPropertiesModel->AppendCategory("Transformation");
    pNewPropertiesModel->AppendProperty_Vec2("Transformation", "Position", glm::vec2(0.0f, 0.0f));
    pNewPropertiesModel->AppendProperty_Vec2("Transformation", "Scale", glm::vec2(0.0f, 0.0f));
    pNewPropertiesModel->AppendProperty_Double("Transformation", "Rotation", 0.0, 0.0, 360.0, "", "°");

    pNewPropertiesModel->AppendCategory("Common");
    pNewPropertiesModel->AppendProperty_Bool("Common", "Enabled", true);
    pNewPropertiesModel->AppendProperty_Bool("Common", "Update while game paused", false);
    pNewPropertiesModel->AppendProperty_Int("Common", "User Tag", 0);
    pNewPropertiesModel->AppendProperty_Int("Common", "Display Order", 0);

    switch(eSelectedType)
    {
    case ITEM_Primitive:
        pNewPropertiesModel->AppendCategory("Transformation");
        break;
    case ITEM_AtlasImage:
        break;
    case ITEM_Font:
        break;
    case ITEM_Sprite:
        break;
    case ITEM_Entity:
        break;
    default:
        HyGuiLog("EntityTreeItem::EntityTreeItem - unsupported type: " % QString::number(eSelectedType), LOGTYPE_Error);
    }

    return pNewPropertiesModel;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityModel::EntityModel(ProjectItem &itemRef, QJsonArray stateArray) : IModel(itemRef),
                                                                        m_TreeModel(this, itemRef)
{
    // If item's init value is defined, parse and initalize with it, otherwise make default empty sprite
    if(stateArray.empty() == false)
    {
        for(int i = 0; i < stateArray.size(); ++i)
            AppendState<EntityStateData>(stateArray[i].toObject());
    }
    else
        AppendState<EntityStateData>(QJsonObject());
}

/*virtual*/ EntityModel::~EntityModel()
{
}

EntityTreeModel &EntityModel::GetTreeModel()
{
    return m_TreeModel;
}

PropertiesTreeModel *EntityModel::GetPropertiesModel(int iStateIndex, EntityTreeItem *pTreeItem)
{
    if(iStateIndex < 0)
        return nullptr;

    PropertiesTreeModel *pPropertiesModel = static_cast<EntityStateData *>(m_StateList[iStateIndex])->GetPropertiesModel(pTreeItem);
    return pPropertiesModel;
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
