/**************************************************************************
*	HyProgressBar.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyProgressBar_h__
#define HyProgressBar_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "UI/Localization/HyLocale.h"

class HyProgressBar : public HyLabel
{
protected:
	enum ProgressBarAttributes
	{
		PROGBARATTRIB_IsVertical		= 1 << 20,	// If set, the bar will grow vertically instead of horizontally
		PROGBARATTRIB_IsInverted		= 1 << 21,	// If set, the bar will grow from right to left or bottom to top
		PROGBARATTRIB_IsBarStretched	= 1 << 22,	// If set and the bar is a Node item, the bar node will be scaled to fit the range of the progress bar. Otherwise, the bar is stenciled/cropped to fit the range (default)
		
		PROGBARATTRIB_IsBarUnderPanel	= 1 << 23,	// If set, the bar will be drawn under the panel instead of over it. Only useful if panel is a node item with transparent center
		PROGBARATTRIB_IsTextOverride	= 1 << 24,	// This is set by this class, and indicates that the text is being overridden by the user SetText(), and should not update to the percentage

		PROGBARATTRIB_NEXTFLAG			= 1 << 25
	};
	static_assert((int)PROGBARATTRIB_IsVertical == (int)LABELATTRIB_NEXTFLAG, "HyProgressBar is not matching with base classes attrib flags");

	glm::vec2			m_vBarOffset;

	int32				m_iMinimum;
	int32				m_iMaximum;
	int32				m_iValue;

	HyPanel				m_Bar;
	HyPrimitive2d		m_BarMask;					// Used with m_BarStencil to crop m_Bar when 'PROGBARATTRIB_IsBarStretched' is off
	HyStencil			m_BarStencil;

	float				m_fBarProgressAmt;
	HyAnimFloat			m_BarProgressAmt;
	
	HyNumberFormat		m_NumberFormat;

public:
	HyProgressBar(HyEntity2d *pParent = nullptr);
	HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, HyEntity2d *pParent = nullptr);
	HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyProgressBar(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyProgressBar();

	virtual void SetText(const std::string &sUtf8Text) override;

	void Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit);
	void Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath);
	void Setup(const HyPanelInit &panelInit, const HyPanelInit &barInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop);

	glm::vec2 GetBarOffset() const;
	void SetBarOffset(const glm::ivec2 &barOffset);
	void SetBarOffset(int32 iBarOffsetX, int32 iBarOffsetY);
	bool SetBarState(uint32 uiStateIndex);

	bool IsVertical() const;
	void SetVertical(bool bIsVertical);

	bool IsInverted() const;
	void SetInverted(bool bIsInverted);

	bool IsBarStretched() const;
	void SetBarStreteched(bool bIsBarStretched);

	bool IsBarUnderPanel() const;
	void SetBarUnderPanel(bool bIsBarUnderPanel);

	void SetMinimum(int32 iMinimum);
	void SetMaximum(int32 iMaximum);
	void SetRange(int32 iMinimum, int32 iMaximum);
	void SetValue(int32 iValue, float fAdjustDuration);

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

protected:
	virtual void Update() override;
	virtual void OnSetup() override;
	void AdjustProgress(float fDuration);

private:
	using HyLabel::Setup;
	using HyLabel::SetText;

	void ApplyProgress();
};

#endif /* HyProgressBar_h__ */
