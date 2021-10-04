/**************************************************************************
*	HyMeter.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyMeter_h__
#define HyMeter_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "UI/Localization/HyLocale.h"

class HyMeter : public HyLabel
{
protected:

	int64					m_iCurValue = 0;
	int64					m_iPrevValue = 0;
	int64					m_iTargetValue = 0;
	int32					m_iDenomination = 1; // Used when not displaying as cash

	float					m_fRackingDuration = 1.0f;
	float					m_fElapsedTimeRack = 0.0f;

	bool					m_bShowAsCash = false;
	bool					m_bSpinDigits = false;
	HyNumberFormat			m_NumberFormat;

	double					m_dTotalDistance = 0.0f;
	double					m_dPrevDistance = 0.0;
	float					m_fThresholdDist = 0.0f;

	uint32					m_uiScissorLayerIndex = 0;

	class SpinText : public HyEntity2d
	{
	public:
		HyText2d			m_SpinText_Shown;
		HyText2d			m_SpinText_Padded;

		SpinText(HyEntity2d *pParent) :
			HyEntity2d(pParent)
		{ }
		virtual ~SpinText()
		{ }

		void Setup(std::string sTextPrefix, std::string sTextName)
		{
			m_SpinText_Shown.Init(sTextPrefix, sTextName, this);
			m_SpinText_Shown.SetTextAlignment(HYALIGN_HCenter);
			m_SpinText_Shown.SetMonospacedDigits(true);

			m_SpinText_Padded.Init(sTextPrefix, sTextName, this);
			m_SpinText_Padded.SetTextAlignment(HYALIGN_HCenter);
			m_SpinText_Padded.SetMonospacedDigits(true);
		}
	};
	SpinText 				m_SpinText;

public:
	HyMeter(HyEntity2d *pParent = nullptr);
	HyMeter(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyMeter(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyMeter();

	virtual void SetAsStacked(HyAlignment eTextAlignment /*= HYALIGN_HCenter*/) override;

	int64 GetValue();
	void SetValue(int64 iPennies, float fRackDuration);
	void OffsetValue(int64 iPenniesOffsetAmt, float fRackDuration);

	void SetDenomination(int32 iDenom);

	void Slam();
	bool IsRacking();

	bool IsShowAsCash();
	void ShowAsCash(bool bShow);

	bool IsSpinningMeter();
	void SetAsSpinningMeter(bool bSet);

	HyNumberFormat GetNumFormat() const;
	void SetNumFormat(HyNumberFormat format);

	void SetText(const std::stringstream &ssUtf8Text) = delete;	// Hiding SetText() since it doesn't make sense to use with HyMeters
	void SetText(const std::string &sUtf8Text) = delete;		// Hiding SetText() since it doesn't make sense to use with HyMeters
	virtual void SetTextState(uint32 uiStateIndex) override;
	virtual void ResetTextAndPanel() override;
	virtual void SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB) override;

protected:
	void FormatDigits();

	

	virtual void OnUpdate() override;
	virtual void OnSetup() override;
};

#endif /* HyMeter_h__ */
