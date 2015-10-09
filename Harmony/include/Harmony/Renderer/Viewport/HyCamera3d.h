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
	friend class HyViewport;
	HyViewport *		m_kpViewportPtr;

	bool				m_bEnabled;

	HyRectangle<float>	m_RenderRect;	// Values are [0.0-1.0] representing percentages

	HyCamera3d(HyViewport *pViewport);
public:
	~HyCamera3d(void);

	void SetEnabled(bool bEnable)				{ m_bEnabled = bEnable; }
	bool IsEnabled()							{ return m_bEnabled; }
	const HyRectangle<float> &GetRenderRect()	{ return m_RenderRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetRenderPercentageCoordinates(float fPosX, float fPosY, float fWidth, float fHeight);
};

#endif /* __HyCamera3d_h__ */
