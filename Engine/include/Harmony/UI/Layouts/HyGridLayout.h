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

class HyGridLayout : public IHyLayout
{
	glm::ivec2					m_GridSize;
	std::vector<HyUI *>			m_NullItemList;

public:
	HyGridLayout(HyEntity2d *pParent = nullptr);
	virtual ~HyGridLayout();

	void InsertLayoutItem(int32 iX, int32 iY, HyUI *pItem);
	void Clear();

protected:
	virtual void OnSetLayoutItems() override;

private:
	// Hide any children functionality inherited from HyEntity2d because derived layouts can only have 'HyUI' as children
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};

#endif /* HyGridLayout_h__ */
