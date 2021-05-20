/**************************************************************************
*	HyGridLayout.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyGridLayout_h__
#define HyGridLayout_h__

#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayout.h"

class HySpacer;

class HyGridLayout : public IHyLayout
{
	glm::ivec2							m_GridSize;
	std::map<glm::ivec2, HyEntityUi *>	m_IndexMap;

	std::vector<HySpacer *>				m_SpacerList;

public:
	HyGridLayout(HyEntity2d *pParent = nullptr);
	virtual ~HyGridLayout();

	void InsertItem(int32 iX, int32 iY, HyEntityUi *pItem);
	void Clear();

protected:

	// Children indices are laid out using row-major
	// [0] [1] [2]
	// [3] [4] [5]
	virtual void OnSetLayoutItems() override;

private:
	// Hide any children functionality inherited from HyEntity2d because derived layouts can only have 'HyUI' as children
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};

#endif /* HyGridLayout_h__ */
