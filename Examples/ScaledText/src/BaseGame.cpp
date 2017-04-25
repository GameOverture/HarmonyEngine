#include "BaseGame.h"
#include "LgSlotGame.h"

BaseGame::BaseGame(LgSlotGame &gameRef, const ILgxGameData *pXmlGameData) : LgReelGame(gameRef, pXmlGameData),
																			m_pMeter(nullptr),
																			m_Text("", "NewFont")
{
	LgInfoPanelInit init;
	init.text_loadPath.Set("", "NewFont");
	HySetVec(init.text_scaleBox, 300, 205);

	m_pMeter = new LgMeter(init);
	m_pMeter->Load();
	m_pMeter->SetValue(999, 0.0f);
	m_pMeter->pos.Set(300.0f, 150.0f);

	m_Text.Load();
	m_Text.TextSet("$12.99");
	m_Text.pos.Set(300.0f, 400.0f);
	m_Text.TextSetAlignment(HYALIGN_Center);
	m_Text.SetAsScaleBox(300, 205, true);

	m_TextBox.Load();
	m_TextBox.SetTint(1.0f, 1.0f, 1.0f);
	m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	m_TextBox.pos.Set(m_Text.pos.Get());

	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterSpin, HYKEY_J);
	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterToggleSpin, HYKEY_K);
	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterToggleCash, HYKEY_M);

	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterBoxSizeVertGrow, HYKEY_Up);
	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterBoxSizeVertShrink, HYKEY_Down);
	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterBoxSizeHorzGrow, HYKEY_Right);
	m_GameRef.Input().MapBtn_KB(DEBUGKEY_MeterBoxSizeHorzShrink, HYKEY_Left);
}

BaseGame::~BaseGame()
{
}

/*virtual*/ void BaseGame::OnUpdate() /*override*/
{
	if(m_GameRef.Input().IsBtnDownBuffered(DEBUGKEY_MeterSpin))
	{
		m_pMeter->SetValue(m_pMeter->GetValue() + 10, 1.0f);
	}

	if(m_GameRef.Input().IsBtnDownBuffered(DEBUGKEY_MeterToggleSpin))
		m_pMeter->SetAsSpinningMeter(!m_pMeter->IsSpinningMeter());

	if(m_GameRef.Input().IsBtnDownBuffered(DEBUGKEY_MeterToggleCash))
		m_pMeter->ShowAsCash(!m_pMeter->IsShowAsCash());

	glm::vec2 vScaleBox = m_pMeter->GetTextScaleBox();
	
	if(m_GameRef.Input().IsBtnDown(DEBUGKEY_MeterBoxSizeVertGrow))
	{
		m_pMeter->SetTextLocation(0, 0, vScaleBox.x, vScaleBox.y + 1.0f);
		m_Text.SetAsScaleBox(vScaleBox.x, vScaleBox.y + 1.0f);
		m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	}

	if(m_GameRef.Input().IsBtnDown(DEBUGKEY_MeterBoxSizeVertShrink))
	{
		m_pMeter->SetTextLocation(0, 0, vScaleBox.x, vScaleBox.y - 1.0f);
		m_Text.SetAsScaleBox(vScaleBox.x, vScaleBox.y - 1.0f);
		m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	}
	
	if(m_GameRef.Input().IsBtnDown(DEBUGKEY_MeterBoxSizeHorzGrow))
	{
		m_pMeter->SetTextLocation(0, 0, vScaleBox.x + 1.0f, vScaleBox.y);
		m_Text.SetAsScaleBox(vScaleBox.x + 1.0f, vScaleBox.y);
		m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	}
		
	if(m_GameRef.Input().IsBtnDown(DEBUGKEY_MeterBoxSizeHorzShrink))
	{
		m_pMeter->SetTextLocation(0, 0, vScaleBox.x - 1.0f, vScaleBox.y);
		m_Text.SetAsScaleBox(vScaleBox.x - 1.0f, vScaleBox.y);
		m_TextBox.SetAsQuad(m_Text.TextGetBox().x, m_Text.TextGetBox().y, true);
	}

	m_Text.TextSet(m_pMeter->GetStr());
}
