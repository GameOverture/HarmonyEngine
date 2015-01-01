#ifndef HYAPP_H
#define HYAPP_H

#include "Harmony/HyEngine.h"

#include <QTcpServer>
#include <QTcpSocket>

class Item;

class HyApp : public IApplication
{
    HyCamera2d *    m_pCam;
    
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
