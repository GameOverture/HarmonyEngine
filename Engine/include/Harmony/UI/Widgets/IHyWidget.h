/**************************************************************************
*	IHyWidget.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyWidget_h__
#define IHyWidget_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyEntityUi.h"

class IHyWidget : public HyEntityUi
{
	friend class IHyLayout;

protected:
	HySizePolicy			m_SizePolicies[HYNUM_ORIENTATIONS];

public:
	IHyWidget(HyEntity2d *pParent = nullptr);
	virtual ~IHyWidget();

	HySizePolicy GetHorizontalPolicy() const;
	HySizePolicy GetVerticalPolicy() const;

	void SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy);
	void SetHorizontalPolicy(HySizePolicy ePolicy);
	void SetVerticalPolicy(HySizePolicy ePolicy);

protected:
	virtual glm::ivec2 GetSizeHint() = 0;
	virtual glm::vec2 GetPosOffset() = 0;
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) = 0;
};

#endif /* IHyWidget_h__ */
