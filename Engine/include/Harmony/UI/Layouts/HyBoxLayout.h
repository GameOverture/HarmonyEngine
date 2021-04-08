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
protected:
	HyOrientation		m_eOrientation;

public:
	HyBoxLayout(HyOrientation eOrientation, HyEntity2d *pParent = nullptr);
	virtual ~HyBoxLayout();

	void AppendItem(HyEntityUi &itemRef);
	void Clear();

protected:
	virtual void OnSetLayoutItems() override;

private:
	// Hide any children functionality inherited from HyEntity2d because derived layouts can only have 'HyUI' as children
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};

class HyHBoxLayout : public HyBoxLayout
{
public:
	HyHBoxLayout(HyEntity2d *pParent = nullptr) :
		HyBoxLayout(HYORIEN_Horizontal, pParent)
	{ }

private:
	// Hide any children functionality inherited from HyEntity2d because derived layouts can only have 'HyUI' as children
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};
class HyVBoxLayout : public HyBoxLayout
{
public:
	HyVBoxLayout(HyEntity2d *pParent = nullptr) :
		HyBoxLayout(HYORIEN_Vertical, pParent)
	{ }

private:
	// Hide any children functionality inherited from HyEntity2d because derived layouts can only have 'HyUI' as children
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};

#endif /* HyBoxLayout_h__ */
