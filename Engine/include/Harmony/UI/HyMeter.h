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
#include "UI/HyInfoPanel.h"

class HyMeter : public HyInfoPanel
{
	int32					m_iCurValue = 0;
	int32					m_iPrevValue = 0;
	int32					m_iTargetValue = 0;
	int32					m_iDenomination = 1; // Used when not displaying as cash

	float					m_fRackingDuration = 1.0f;
	float					m_fElapsedTimeRack = 0.0f;

	bool					m_bShowAsCash = false;
	bool					m_bSpinDigits = false;
	bool					m_bUseCommas = false;

	double					m_dTotalDistance = 0.0f;
	double					m_dPrevDistance = 0.0;
	float					m_fThresholdDist = 0.0f;

	uint32					m_uiScissorLayerIndex = 0;

	class SpinText : public HyEntity2d
	{
	public:
		HyText2d			m_SpinText_Shown;
		HyText2d			m_SpinText_Padded;

		SpinText() = default;
		virtual ~SpinText() = default;

		void Setup(std::string sTextPrefix, std::string sTextName)
		{
			m_SpinText_Shown.Init(sTextPrefix, sTextName, this);
			m_SpinText_Shown.SetTextAlignment(HYALIGN_Center);
			m_SpinText_Shown.SetMonospacedDigits(true);

			m_SpinText_Padded.Init(sTextPrefix, sTextName, this);
			m_SpinText_Padded.SetTextAlignment(HYALIGN_Center);
			m_SpinText_Padded.SetMonospacedDigits(true);
		}
	};
	SpinText 				m_SpinText;

public:
	HyMeter();
	HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY);
	HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);
	virtual ~HyMeter();

	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName) override;
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY) override;
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY) override;

	int32 GetValue();
	void SetValue(int32 iPennies, float fRackDuration);
	void OffsetValue(int32 iPenniesOffsetAmt, float fRackDuration);

	void SetDenomination(int32 iDenom);

	void Slam();
	bool IsRacking();

	bool IsShowAsCash();
	void ShowAsCash(bool bShow);

	bool IsSpinningMeter();
	void SetAsSpinningMeter(bool bSet);

	bool IsUsingCommas();
	void SetAsUsingCommas(bool bSet);

	void SetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);

	void TextSetState(uint32 uiAnimState);

	virtual std::string GetStr() override;
	virtual void SetStr(std::string sText) override;
	virtual void SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY) override;
	virtual void SetTextAlignment(HyTextAlign eAlignment) override;

private:
	std::string ToStringWithCommas(int32 iValue);
	std::string FormatString(int32 iValue);
	void FormatDigits();

	virtual void OnUpdate() override;
};

#endif /* HyMeter_h__ */
