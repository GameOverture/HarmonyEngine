/**************************************************************************
 *	HyOpenGL.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGL_h__
#define __HyOpenGL_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/OpenGL/HyOpenGLShader.h"

class HyOpenGL : public IHyRenderer
{
protected:
	mat4					m_mtxView;
	mat4					m_mtxProj;
	mat4					m_mtxProjLocalCoords;
	const mat4				m_kmtxIdentity;

	int32					m_iNumRenderPassesLeft2d;
	int32					m_iNumPasses3d;

	///////// 2D MEMBERS ////////////////////
	enum eVAOTypes
	{
		QUADBATCH,
		PRIMITIVE,
		CUSTOM,

		NUM_VAOTYPES
	};
	GLuint					m_pVAO2d[NUM_VAOTYPES];
	HyOpenGLShader			m_pShader2d[NUM_VAOTYPES];
	
	GLuint					m_hVBO2d;
	GLenum					m_eDrawMode;

public:
	HyOpenGL(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef);
	virtual ~HyOpenGL(void);

	virtual void StartRender();
	
	virtual bool Begin_3d();
	virtual void SetRenderState_3d(uint32 uiNewRenderState);
	virtual void End_3d();

	virtual bool Begin_2d();
	virtual void DrawRenderState_2d(HyRenderState &renderState);
	virtual void End_2d();

	virtual void FinishRender();

	// Returns the texture ID used for API specific drawing.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, vector<unsigned char *> &vPixelData);
	virtual void DeleteTextureArray(uint32 uiTextureHandle);

	virtual void OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags);

protected:
	bool Initialize();
};

#endif /* __HyOpenGL_h__ */
