/**************************************************************************
*	HySpacer.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySpacer_h__
#define HySpacer_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"

// This class is used internally in HyUiContainer (via ::InsertSpacer()) and are managed indirectly with HySpacerHandle's (via ::SetSpacer())
class HyUiContainer;

class HySpacer : public IHyWidget
{
	friend class HyUiContainer;

	const HyOrientation		m_eORIEN_TYPE;
	int32					m_iSizeHint;
	int32					m_iActualSize;

	HySpacer(HyOrientation eOrienType);
public:
	virtual ~HySpacer();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	int32 GetActualSize() const;
	void Setup(HySizePolicy eSizePolicy, uint32 uiSizeHint);

	void SetSize(uint32 uiSizeHint);

protected:
	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;
};

#endif /* HySpacer_h__ */
