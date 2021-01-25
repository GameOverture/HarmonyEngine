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

class HyButton;
typedef std::function<void(HyButton *pSelf)> HyButtonClickedCallback;

class HyButton : public HyInfoPanel
{
	HyButtonClickedCallback		m_fpButtonClickedCallback = nullptr;

public:
	HyButton();
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY);
	HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);
	virtual ~HyButton();

	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName) override;
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY) override;
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY) override;

	void SetAsSelected(bool bInvokeButtonClicked);
	void SetAsDisabled();

	void SetButtonClickedCallback(HyButtonClickedCallback fpCallBack);

private:
	virtual void OnMouseDown() override final;
	virtual void OnMouseUp() override final;
	virtual void OnMouseLeave() override final;
	virtual void OnMouseClicked() override final;
};

#endif /* HyButton_h__ */
