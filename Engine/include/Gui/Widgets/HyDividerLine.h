/**************************************************************************
*	HyDividerLine.h
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDividerLine_h__
#define HyDividerLine_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/IHyWidget.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

class HyDividerLine : public IHyWidget
{
	HyPrimitive2d							m_Line;

protected:
	enum WidgetAttributes
	{
		DIVIDERLINEATTRIB_IsHorizontal		= 1 << 16,

		DIVIDERLINEATTRIB_NEXTFLAG			= 1 << 17,
	};
	static_assert((int)DIVIDERLINEATTRIB_IsHorizontal == (int)WIDGETATTRIB_NEXTFLAG, "HyDividerLine is not matching with base classes attrib flags");

public:
	HyDividerLine(HyEntity2d *pParent = nullptr);
	HyDividerLine(HyOrientation eOrientation, float fThickness, HyColor eColor, HyEntity2d *pParent = nullptr);
	virtual ~HyDividerLine();

	HyOrientation GetOrientation() const;
	void SetOrientation(HyOrientation eOrientation);

	float GetThickness() const;
	void SetThickness(float fThickness);

protected:
	virtual glm::ivec2 OnCalcPreferredSize() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;	// Returns the size *this was actually set to (which may be different than what's passed in due to constraints)
};

#endif /* HyDividerLine_h__ */
