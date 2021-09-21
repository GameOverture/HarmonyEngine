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

	IHyDrawable2d *		m_pFill;
	HyRectangle<float>	m_FillMargins;
	
	HyNumberFormat		m_NumberFormat;

public:
	HyProgressBar(HyEntity2d *pParent = nullptr);
	HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyProgressBar(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyProgressBar(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyProgressBar();

	void SetFillMargins(const HyRectangle<int32> &fillMarginsRef);
	void SetFillMargins(int32 iFillMarginLeft, int32 iFillMarginBottom, int32 iFillMarginRight, int32 iFillMarginTop);

	void Reset();
	void SetMinimum(int32 iMinimum);
	void SetMaximum(int32 iMaximum);
	void SetRange(int32 iMinimum, int32 iMaximum);
	void SetValue(int32 iValue);

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

protected:
	virtual void OnSetup() override;
	void AdjustProgress();
};

#endif /* HyProgressBar_h__ */
