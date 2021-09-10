/**************************************************************************
*	HyBoxLayout.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyBoxLayout_h__
#define HyBoxLayout_h__

#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayout.h"

class IHyWidget;

class HyBoxLayout : public IHyLayout
{
	HY_UILAYOUT

protected:
	HyOrientation		m_eOrientation;

public:
	HyBoxLayout(HyOrientation eOrientation, HyEntity2d *pParent = nullptr);
	virtual ~HyBoxLayout();


	void AppendItem(HyEntityUi &itemRef);

protected:
	virtual void OnSetLayoutItems() override;
};

class HyHBoxLayout : public HyBoxLayout
{
	HY_UILAYOUT

public:
	HyHBoxLayout(HyEntity2d *pParent = nullptr) :
		HyBoxLayout(HYORIEN_Horizontal, pParent)
	{ }
};
class HyVBoxLayout : public HyBoxLayout
{
	HY_UILAYOUT

public:
	HyVBoxLayout(HyEntity2d *pParent = nullptr) :
		HyBoxLayout(HYORIEN_Vertical, pParent)
	{ }
};

#endif /* HyBoxLayout_h__ */
