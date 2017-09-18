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
    // Default ranges
    const int iRANGE = 16777215;        // Uses 3 bytes (0xFFFFFF)... Qt uses this value for their default ranges in QSpinBox
    const double fRANGE = 16777215.0f;
    const double dRANGE = 16777215.0;

    PropertiesTreeModel *pNewPropertiesModel = new PropertiesTreeModel(entityItemRef, GetIndex(), subState);

    PropertiesDef def;
    PropertiesDef defInt(PROPERTIESTYPE_int, 0, -iRANGE, iRANGE, 1, "", "");
    PropertiesDef defVec2(PROPERTIESTYPE_vec2, QPointF(0.0f, 0.0f), QPointF(-fRANGE, -fRANGE), QPointF(fRANGE, fRANGE), 1.0, "[", "]");

    pNewPropertiesModel->AppendCategory("Transformation", QColor(220, 220, 0));
    pNewPropertiesModel->AppendProperty("Transformation", "Position", defVec2);
    defVec2.defaultData = QPointF(1.0f, 1.0f);
    defVec2.stepAmt = 0.01;
    pNewPropertiesModel->AppendProperty("Transformation", "Scale", defVec2);
    pNewPropertiesModel->AppendProperty("Transformation", "Rotation", PropertiesDef(PROPERTIESTYPE_double, 0.0, 0.0, 360.0, 1.0, "", "°"));

    pNewPropertiesModel->AppendCategory("Common", QColor(0, 220, 220));
    def.eType = PROPERTIESTYPE_bool;
    def.defaultData = Qt::Checked;
    pNewPropertiesModel->AppendProperty("Common", "Enabled", def);
    def.defaultData = Qt::Unchecked;
    pNewPropertiesModel->AppendProperty("Common", "Update while game paused", def);
    pNewPropertiesModel->AppendProperty("Common", "User Tag", defInt);
    pNewPropertiesModel->AppendProperty("Common", "Display Order", defInt);

    switch(eSelectedType)
    {
        case ITEM_Entity: {
            pNewPropertiesModel->AppendCategory("Physics", QColor(0, 220, 220));

            PropertiesDef defComboBox;
            defComboBox.eType = PROPERTIESTYPE_ComboBox;
            defComboBox.defaultData = 0;
            QStringList sList;
            sList << "Static" << "Kinematic" << "Dynamic";
            defComboBox.delegateBuilder = sList;
            pNewPropertiesModel->AppendProperty("Physics", "Type", defComboBox);
        } break;

        case ITEM_Primitive:
            break;
        case ITEM_AtlasImage:
            break;
        case ITEM_Font:
            break;
        case ITEM_Sprite:
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
