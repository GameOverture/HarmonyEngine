/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyButton_h__
#define HyButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyInfoPanel.h"

typedef std::function<void(HyInfoPanel *pSelf, void *pParam)> HyButtonClickedCallback;

class HyButton : public HyInfoPanel
{
	HyButtonClickedCallback		m_fpButtonClickedCallback;
	void *						m_pCallbackParam;

public:
	HyButton(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	virtual ~HyButton();

	void SetAsSelected(bool bInvokeButtonClicked);
	void SetAsDisabled();

	void SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam = nullptr);

private:
	virtual void OnMouseDown(void *pUserParam) override final;
	virtual void OnMouseUp(void *pUserParam) override final;
	virtual void OnMouseLeave(void *pUserParam) override final;
	virtual void OnMouseClicked(void *pUserParam) override final;
};

#endif /* HyButton_h__ */
