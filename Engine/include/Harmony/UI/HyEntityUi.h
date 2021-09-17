/**************************************************************************
*	HyEntityUi.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyEntityUi_h__
#define HyEntityUi_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

class HyEntityUi : public HyEntity2d
{
protected:
	enum UiType
	{
		Ui_Widget = 0,
		Ui_Layout,
		Ui_Container
	};
	const UiType			m_eUI_TYPE;

public:
	HyEntityUi(UiType eType, HyEntity2d *pParent = nullptr);
	virtual ~HyEntityUi();

	UiType GetUiType() const;
	virtual glm::ivec2 GetSizeHint() = 0;	// The preferred size of *this
};

#endif /* HyEntityUi_h__ */
