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
#include "UI/Widgets/IHyWidget.h"

class HySpacer : public IHyWidget
{
public:
	HySpacer(HyEntity2d *pParent = nullptr);
	virtual ~HySpacer();

protected:
	virtual glm::vec2 GetPosOffset() override;

	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) override;
};

#endif /* HySpacer_h__ */
