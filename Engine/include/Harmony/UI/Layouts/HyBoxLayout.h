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

class HyBoxLayout : protected IHyLayout
{
public:
	enum Orientation
	{
		Horizontal = 0,
		Vertical
	};

protected:
	Orientation		m_eOrientation;

public:
	HyBoxLayout(Orientation eOrientation, HyEntity2d *pParent = nullptr);
	virtual ~HyBoxLayout();

protected:
	virtual void OnDoLayout() override;
};

#endif /* HyBoxLayout_h__ */
