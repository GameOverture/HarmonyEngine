#ifndef HYAPP_H
#define HYAPP_H

#include "Harmony/HyEngine.h"
#include "Spine.h"

#include "Item.h"

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
};

#endif // HYAPP_H
