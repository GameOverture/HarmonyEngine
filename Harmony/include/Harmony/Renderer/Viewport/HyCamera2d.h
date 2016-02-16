/**************************************************************************
 *	HyCamera2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCamera2d_h__
#define __HyCamera2d_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/ITransform.h"
#include "Utilities/Animation/HyAnimVec2.h"
#include "Utilities/HyMath.h"

class HyCamera2d : public ITransform<HyAnimVec2>
{
	friend class HyWindow;
	HyWindow *		m_pViewportPtr;

	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages
	
	HyCamera2d(HyWindow *pViewport);
public:
	~HyCamera2d(void);

	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);

	inline void SetZoom(const float fZoom)		{ scale.Set(fZoom, fZoom); }
	inline float GetZoom() const				{ return scale.Get().x; }
};

#endif /* __HyCamera2d_h__ */
