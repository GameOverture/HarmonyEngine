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
	int32			m_iMinimum;
	int32			m_iMaximum;
	int32			m_iValue;

	HyNumberFormat	m_NumberFormat;
	HySprite2d		m_sprFill;

public:
	HyProgressBar(HyEntity2d *pParent = nullptr);
	HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent = nullptr);
	HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sPanelFillPrefix, std::string sPanelFillName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d* pParent = nullptr);
	virtual ~HyProgressBar();

	void Reset();
	void SetMinimum(int32 iMinimum);
	void SetMaximum(int32 iMaximum);
	void SetRange(int32 iMinimum, int32 iMaximum);
	void SetValue(int32 iValue);

	HySprite2d& GetFill();

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

protected:
	void AdjustProgress();
};

#endif /* HyProgressBar_h__ */
