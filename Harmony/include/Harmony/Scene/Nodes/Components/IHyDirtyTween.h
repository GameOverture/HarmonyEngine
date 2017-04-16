/**************************************************************************
*	IHyDirtyTween.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDirtyTween_h__
#define IHyDirtyTween_h__

#include "Afx/HyStdAfx.h"

class IHyDirtyTween
{
	friend class HyTweenFloat;

	bool		m_bDirty;

public:
	IHyDirtyTween();
	~IHyDirtyTween();

private:
	void SetDirty();
};

#endif /* IHyDirtyTween_h__ */
