/**************************************************************************
*	HyProfiler.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyProfiler_h__
#define HyProfiler_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyUiContainer.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

class HyTime;

class HyGraphWidget : public IHyWidget
{
	float				m_fWidth = 200.0f;
	const float			m_fHEIGHT = 25.0f;

	enum PortionType
	{
		PORTION_Update = 0,
		PORTION_RenderPrep,
		PORTION_Render,

		NUM_PORTIONS
	};
	struct Portion
	{
		std::string			m_sName;
		double				m_dDuration = 0.0;
		HyPrimitive2d		m_Bar;
	};
	Portion					m_PortionList[NUM_PORTIONS];

public:
	HyGraphWidget() : IHyWidget(HyPanelInit())
	{
		m_PortionList[PORTION_Update].m_sName = "Update";
		m_PortionList[PORTION_Update].m_Bar.SetTint(HyColor::Cyan);
		ChildAppend(m_PortionList[PORTION_Update].m_Bar);

		m_PortionList[PORTION_RenderPrep].m_sName = "Prep";
		m_PortionList[PORTION_RenderPrep].m_Bar.SetTint(HyColor::DarkRed);
		ChildAppend(m_PortionList[PORTION_RenderPrep].m_Bar);

		m_PortionList[PORTION_Render].m_sName = "Render";
		m_PortionList[PORTION_Render].m_Bar.SetTint(HyColor::Red);
		ChildAppend(m_PortionList[PORTION_Render].m_Bar);

		UpdatePortions();
	}
	virtual float GetWidth(float fPercent = 1.0f) override { return m_fWidth; }
	virtual float GetHeight(float fPercent = 1.0f) override { return m_fHEIGHT; }
	void SetWidth(float fWidth) { m_fWidth = fWidth; SetSizeAndLayoutDirty(); }
	void AddDeltas(double dUpdate, double dRenderPrep, double dRender) {
		m_PortionList[PORTION_Update].m_dDuration += dUpdate;
		m_PortionList[PORTION_RenderPrep].m_dDuration += dRenderPrep;
		m_PortionList[PORTION_Render].m_dDuration += dRender;
	}

	void Flush() {
		UpdatePortions();
		m_PortionList[PORTION_Update].m_dDuration = 0.0;
		m_PortionList[PORTION_RenderPrep].m_dDuration = 0.0;
		m_PortionList[PORTION_Render].m_dDuration = 0.0;
	}

protected:
	virtual glm::vec2 GetPosOffset() override { return glm::vec2(0, 0); }
	virtual void OnSetSizeHint() override { HySetVec(m_vSizeHint, static_cast<int32>(m_fWidth), static_cast<int32>(m_fHEIGHT)); }
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override {
		m_fWidth = static_cast<float>(uiNewWidth);
		OnSetSizeHint();
		UpdatePortions();

		return m_vSizeHint;
	}
	void UpdatePortions() {
		double dTotal = 0.0;
		for(uint32 i = 0; i < NUM_PORTIONS; ++i)
		{
			if(m_PortionList[i].m_dDuration == 0.0)
				return;
			dTotal += m_PortionList[i].m_dDuration;
		}

		float fXPos = 0.0f;
		for(uint32 i = 0; i < NUM_PORTIONS; ++i)
		{
			m_PortionList[i].m_Bar.SetAsBox(m_fWidth * static_cast<float>(m_PortionList[i].m_dDuration / dTotal), m_fHEIGHT);
			m_PortionList[i].m_Bar.pos.Set(fXPos, 0.0f);
			fXPos += m_PortionList[i].m_Bar.GetSceneWidth();
		}
	}
};

class HyProfiler : public HyUiContainer
{
	double					m_dFrameTime_Low;
	double					m_dFrameTime_High;
	double					m_dFrameTime_Cumulative;
	uint32					m_uiFrameCount;
	uint32					m_uiUpdateCount;

	double					m_dUpdateTimeStamp;
	double					m_dPrepTimeStamp;
	double					m_dRenderTimeStamp;

	HyLabel					m_txtFps;
	HyLabel					m_txtFrameTimes;
	HyGraphWidget			m_Graph;
	HyLabel					m_txtMouse;
	HyLabel					m_txtMouseWorld;
	HyLabel					m_txtMouseBtns;

	uint32					m_uiShowFlags;

public:
	HyProfiler();
	virtual ~HyProfiler();

	void InitText(std::string sPrefix, std::string sName, uint32 uiTextState);

	void SetShowFlags(uint32 uiDiagFlags);
	uint32 GetShowFlags();

	void BeginFrame(const HyTime &timeRef);
	void BeginUpdate(const HyTime &timeRef);
	void BeginRenderPrep(const HyTime &timeRef);
	void BeginRender(const HyTime &timeRef);
};

#endif /* HyProfiler_h__ */
