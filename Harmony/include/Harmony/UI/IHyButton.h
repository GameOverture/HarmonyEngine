/**************************************************************************
*	IHyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyButton_h__
#define IHyButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyInfoPanel.h"

class IHyButton : public HyInfoPanel
{
public:
	IHyButton(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	virtual ~IHyButton();

	void SetAsSelected(bool bInvokeButtonClicked);
	void SetAsDisabled();

private:
	virtual void OnMouseDown(void *pUserParam) override final;
	virtual void OnMouseUp(void *pUserParam) override final;
	virtual void OnMouseLeave(void *pUserParam) override final;
	virtual void OnMouseClicked(void *pUserParam) override final;

	virtual void OnButtonClicked(HyEntity2d *pParent) = 0;
};

#endif /* IHyButton_h__ */
