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
		HyText2d *				m_pSpinText_Shown;
		HyText2d *				m_pSpinText_Padded;

		SpinText(HyEntity2d *pParent) :	HyEntity2d(pParent),
			m_pSpinText_Shown(nullptr),
			m_pSpinText_Padded(nullptr)
		{
		}

		virtual ~SpinText()
		{
			delete m_pSpinText_Shown;
			delete m_pSpinText_Padded;
		}

		void Init(const char *szTextPrefix, const char *szTextName)
		{
			if(szTextName == nullptr)
				return;

			delete m_pSpinText_Shown;
			delete m_pSpinText_Padded;

			m_pSpinText_Shown = HY_NEW HyText2d(szTextPrefix, szTextName, this);
			m_pSpinText_Shown->TextSetAlignment(HYALIGN_Center);
			m_pSpinText_Shown->TextSetMonospacedDigits(true);

			m_pSpinText_Padded = HY_NEW HyText2d(szTextPrefix, szTextName, this);
			m_pSpinText_Padded->TextSetAlignment(HYALIGN_Center);
			m_pSpinText_Padded->TextSetMonospacedDigits(true);
		}
	};
	SpinText 				m_SpinText;

public:
	HyMeter(HyEntity2d *pParent = nullptr);
	HyMeter(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent);
	HyMeter(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	HyMeter(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	virtual ~HyMeter();

	virtual void Init(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) override;
	virtual void Init(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) override;
	virtual void Init(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) override;

	int32 GetValue();
	void SetValue(int32 iPennies, float fRackDuration);
	void OffsetValue(int32 iPenniesOffsetAmt, float fRackDuration);

	void Slam();
	bool IsRacking();

	bool IsShowAsCash();
	void ShowAsCash(bool bShow);

	bool IsSpinningMeter();
	void SetAsSpinningMeter(bool bSet);

	bool IsUsingCommas();
	void SetAsUsingCommas(bool bSet);

	void TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);

	void TextSetState(uint32 uiAnimState);

	virtual std::string GetStr() override;
	virtual void SetStr(std::string sText) override;
	virtual void SetTextLocation(int32 iOffsetX, int32 iOffsetY, int32 iWidth, int32 iHeight) override;
	virtual void SetTextAlignment(HyTextAlign eAlignment) override;

private:
	std::string ToStringWithCommas(int32 iValue);
	std::string FormatString(int32 iValue);
	void FormatDigits();

	virtual void OnUpdate() override;
};

#endif /* HyMeter_h__ */
