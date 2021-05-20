/**************************************************************************
*	HyContainer.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyContainer_h__
#define HyContainer_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyEntityUi.h"
#include "UI/Layouts/IHyLayout.h"

class HyContainer : public HyEntityUi
{
	glm::vec2				m_vSize;
	IHyLayout *				m_pRootLayout;

public:
	HyContainer(HyLayoutType eRootLayout, HyEntity2d *pParent = nullptr);
	virtual ~HyContainer();

	glm::ivec2 GetSize() const;
	void SetSize(int32 iNewWidth, int32 iNewHeight);

	IHyLayout *GetRootLayout();

	template<typename LAYOUT>
	LAYOUT *GetRootLayout() {
		return static_cast<LAYOUT *>(m_pRootLayout);
	}
};

#endif /* HyContainer_h__ */
