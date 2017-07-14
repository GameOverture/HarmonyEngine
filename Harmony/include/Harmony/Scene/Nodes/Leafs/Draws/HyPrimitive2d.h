/**************************************************************************
 *	HyPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyPrimitive2d_h__
#define __HyPrimitive2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyLeafDraw2d
{
protected:
	glm::vec2 *		m_pDrawBuffer;
	uint32			m_uiBufferSize;

public:
	HyPrimitive2d(HyEntity2d *pParent = nullptr);
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p);

	void SetAsQuad(int iWidth, int iHeight, bool bWireframe);
	void SetAsQuad(float fWidth, float fHeight, bool bWireframe);

	void SetAsCircle(float fRadius, int32 iNumSegments, bool bWireframe);

	void SetAsLineChain(std::vector<glm::vec2> &vertexList);

	float GetLineThickness();
	void SetLineThickness(float fThickness);

protected:
	void ClearData();

private:
	virtual void OnCalcBoundingVolume() override;
	
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;

#ifdef HY_PLATFORM_GUI
public:
	virtual void GuiOverrideData(jsonxx::Value &dataValueRef) override;
#endif
};

#endif /* __HyPrimitive2d_h__ */
