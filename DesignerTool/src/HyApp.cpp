#include "HyApp.h"
#include "Item.h"

HyApp::HyApp(HarmonyInit &initStruct) :     IApplication(initStruct),
                                            m_pCurItem(NULL)
{
}

/*virtual*/ bool HyApp::Initialize()
{
    vector<uint32>	vGamePadIds;
    m_pInputArray->GetGamePadIds(vGamePadIds);

    if(vGamePadIds.empty() == false)
    {
        m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(vGamePadIds[0], GP360_ButtonA));
        m_pInputArray->BindBtnMap(GP360_ButtonB, HyInputKey(vGamePadIds[0], GP360_ButtonB));

        m_pInputArray->BindAxisMap(GP_RStickX, HyInputKey(vGamePadIds[0], GP_RStickX));
        m_pInputArray->BindAxisMap(GP_RStickY, HyInputKey(vGamePadIds[0], GP_RStickY));
        m_pInputArray->BindAxisMap(GP_Triggers, HyInputKey(vGamePadIds[0], GP_Triggers));

        m_pInputArray->BindAxisMap(GP_LStickX, HyInputKey(vGamePadIds[0], GP_LStickX));
    }
    m_pInputArray->BindAxisMapPos(GP_LStickX, HyInputKey('D'));
    m_pInputArray->BindAxisMapNeg(GP_LStickX, HyInputKey('A'));

    m_pInputArray->BindBtnMap(GP360_ButtonA, HyInputKey(' '));

    m_pCam = m_Viewport.CreateCamera2d();

    //HyGfxWindow::tResolution tRes;
    //m_Window.GetResolution(tRes);
    //m_Camera.SetOrthographic(static_cast<float>(tRes.iWidth), static_cast<float>(tRes.iHeight));

    m_pCam->Pos().Set(0.0f, 0.0f);
    m_pCam->SetZoom(0.8f);

    return true;
}

/*virtual*/ bool HyApp::Update()
{
    
    
    if(m_pCurItem)
        m_pCurItem->Draw(this);

    return true;
}

/*virtual*/ bool HyApp::Shutdown()
{
    return true;
}

void HyApp::SetItem(Item *pItem)
{
    if(m_pCurItem != NULL)
        m_pCurItem->Hide();
    
    m_pCurItem = pItem;
    
    if(m_pCurItem != NULL)
        m_pCurItem->Show();
}

HyViewport &HyApp::GetViewport()
{
    return m_Viewport;
}
