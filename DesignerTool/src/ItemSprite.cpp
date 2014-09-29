#include "ItemSprite.h"

ItemSprite::ItemSprite() : Item()
{
    m_eType = ITEM_Sprite;
}

/*virtual*/ void ItemSprite::ReadJson(const QJsonObject &json)
{
}

/*virtual*/ void ItemSprite::WriteJson(QJsonObject &json) const
{
}

/*virtual*/ void ItemSprite::Draw()
{
    
}
