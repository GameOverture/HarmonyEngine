/**************************************************************************
*	IHyLayoutItem.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLayoutItem_h__
#define IHyLayoutItem_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

class IHyLayoutItem : protected HyEntity2d
{
protected:
	enum SizePolicyFlag
	{
		// WARNING: THIS ENUM ORDER MATTERS
		Grow	= 1 << 0,
		Expand	= 1 << 1,
		Shrink	= 1 << 2,
		Ignore	= 1 << 3
	};

public:
	enum SizePolicy
	{
		Fixed = 0,							// GetSizeHint() is the only acceptable alternative, so the widget can never grow or shrink (e.g. the vertical direction of a push button).
		Minimum = Grow,						// GetSizeHint() is the minimum size. The widget can be expanded, but there is no advantage to it being larger (e.g. the horizontal direction of a push button).
		Maximum = Shrink,					// GetSizeHint() is the maximum size. The widget can be shrunk any amount without detriment if other widgets need the space (e.g. a separator line). It cannot be larger than the size provided by SizeHint().
		Preferred = Grow | Shrink,			// GetSizeHint() is best, but the widget can be shrunk and still be useful. The widget can be expanded, but there is no advantage to it being larger than SizeHint() (the default HyWidget policy).
		MinimumExpanding = Grow | Expand,	// GetSizeHint() is the minimum size. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
		Expanding = Grow | Shrink | Expand,	// GetSizeHint() is a sensible size, but the widget can be shrunk and still be useful. The widget can make use of extra space, so it should get as much space as possible (e.g. the horizontal direction of a horizontal slider).
		Fill = Grow | Shrink | Ignore		// GetSizeHint() is ignored. The widget will use as little or as much space as possible.
	};

protected:
	SizePolicy				m_HorzPolicy;
	SizePolicy				m_VertPolicy;
	glm::ivec2				m_vSizeHint;

public:
	IHyLayoutItem(HyEntity2d *pParent = nullptr);
	virtual ~IHyLayoutItem();

	SizePolicy GetHorizontalPolicy() const;
	SizePolicy GetVerticalPolicy() const;

	void SetSizePolicy(SizePolicy eHorizPolicy, SizePolicy eVertPolicy);
	void SetHorizontalPolicy(SizePolicy ePolicy);
	void SetVerticalPolicy(SizePolicy ePolicy);

	glm::ivec2 GetSizeHint() const;
	void SetSizeHint(int32 iWidth, int32 iHeight);

protected:
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) = 0;
};

#endif /* IHyLayoutItem_h__ */
