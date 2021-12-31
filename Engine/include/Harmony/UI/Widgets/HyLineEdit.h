/**************************************************************************
*	HyLineEdit.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLineEdit_h__
#define HyLineEdit_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"

class HyLineEdit : public HyLabel
{
protected:

public:
	HyLineEdit(HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLineEdit();

protected:
	virtual void OnUiTextInput(std::string sNewText) override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn) override;
	virtual void OnUiMouseClicked() override;

	virtual void OnTakeKeyboardFocus() override;
	virtual void OnRelinquishKeyboardFocus() override;

	virtual void OnSetup() override;
};

#endif /* HyLineEdit_h__ */
