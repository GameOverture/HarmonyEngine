/**************************************************************************
*	HySpacer.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySpacer_h__
#define HySpacer_h__

#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"

class HySpacer : public IHyEntityUi
{
public:
	HySpacer(HyEntity2d *pParent = nullptr);
	virtual ~HySpacer();

	virtual void SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY) override;

protected:
	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;
};

#endif /* HySpacer_h__ */
