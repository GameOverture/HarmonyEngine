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
	friend class HyViewport;
	HyViewport *		m_pViewportPtr;

	bool				m_bEnabled;

	HyRectangle<float>	m_RenderRect;	// Values are [0.0-1.0] representing percentages
	
	HyCamera2d(HyViewport *pViewport);
public:
	~HyCamera2d(void);

	void SetEnabled(bool bEnable)				{ m_bEnabled = bEnable; }
	bool IsEnabled()							{ return m_bEnabled; }
	const HyRectangle<float> &GetRenderRect()	{ return m_RenderRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetRenderPercentageCoordinates(float fPosX, float fPosY, float fWidth, float fHeight);

	inline void SetZoom(const float fZoom)		{ m_vScale.Set(fZoom, fZoom); }
	inline float GetZoom() const				{ return m_vScale.Get().x; }
};

#endif /* __HyCamera2d_h__ */
