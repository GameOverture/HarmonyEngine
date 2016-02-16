/**************************************************************************
 *	HyCamera3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCamera3d_h__
#define __HyCamera3d_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/ITransform.h"
#include "Utilities/Animation/HyAnimVec3.h"
#include "Utilities/HyMath.h"

#include <vector>
using std::vector;

class HyCamera3d : public ITransform<HyAnimVec3>
{
	friend class HyWindow;
	HyWindow *		m_kpViewportPtr;

	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	HyCamera3d(HyWindow *pViewport);
public:
	~HyCamera3d(void);

	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);
};

#endif /* __HyCamera3d_h__ */
