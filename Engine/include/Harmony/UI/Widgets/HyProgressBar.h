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
	int32				m_iMinimum;
	int32				m_iMaximum;
	int32				m_iValue;

	IHyDrawable2d *		m_pBar;
	glm::ivec2			m_vBarOffset;
	float				m_fBarScissorAmt;
	HyAnimFloat			m_BarScissorAmt;
	
	HyNumberFormat		m_NumberFormat;

public:
	HyProgressBar(HyEntity2d *pParent = nullptr);
	HyProgressBar(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyProgressBar(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyProgressBar();

	const glm::ivec2 &GetBarOffset() const;
	void SetBarOffset(const glm::ivec2 &barOffset);
	void SetBarOffset(int32 iBarOffsetX, int32 iBarOffsetY);

	void Reset();
	void SetMinimum(int32 iMinimum);
	void SetMaximum(int32 iMaximum);
	void SetRange(int32 iMinimum, int32 iMaximum);
	void SetValue(int32 iValue);

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

protected:
	virtual void OnUpdate() override;
	virtual void OnSetup() override;
	void AdjustProgress();
};

#endif /* HyProgressBar_h__ */
