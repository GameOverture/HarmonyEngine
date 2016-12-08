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

#include "Scene/Instances/IHyInst2d.h"

class HyAtlasGroupData;

class HyPrimitive2d : public IHyInst2d
{
protected:
	glm::vec4 *		m_pVertices;

public:
	HyPrimitive2d();
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p);

	uint32 GetNumVerts()			{ return m_RenderState.GetNumVerticesPerInstance(); }
	glm::vec4 *GetVertexDataPtr()	{ return m_pVertices; }

	void SetAsQuad(int iWidth, int iHeight, bool bWireframe, glm::vec2 &vOffset = glm::vec2(0.0f));
	void SetAsQuad(float fWidth, float fHeight, bool bWireframe, glm::vec2 &vOffset = glm::vec2(0.0f));

	void SetAsCircle(float fRadius, int32 iNumSegments, bool bWireframe, glm::vec2 &vOffset = glm::vec2(0.0f));

	void SetAsEdgeChain(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, glm::vec2 &vOffset = glm::vec2(0.0f));

	void OffsetVerts(glm::vec2 vOffset, float fAngleOffset);

private:

	void ClearData();

	virtual void OnDataLoaded();

	virtual void OnInstUpdate();
	
	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyPrimitive2d_h__ */
