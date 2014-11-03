#ifndef HYAPP_H
#define HYAPP_H

#include "Harmony/HyEngine.h"
#include "Spine.h"

class Item;

class HyApp : public IApplication
{
    HyCamera2d *    m_pCam;

    Player          m_Player;
    
    Item *          m_pCurItem;
    
public:
    HyApp(HarmonyInit &initStruct);

    virtual bool Initialize();
    virtual bool Update();
    virtual bool Shutdown();
    
    void SetItem(Item *pItem);
    
    HyViewport &GetViewport();
};

#endif // HYAPP_H
