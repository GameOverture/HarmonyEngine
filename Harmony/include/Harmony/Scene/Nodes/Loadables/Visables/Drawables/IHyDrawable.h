/**************************************************************************
*	IHyDrawable.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable_h__
#define IHyDrawable_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Scene/Nodes/IHyNode.h"

class IHyDrawable
{
	friend class HyScene;
	friend class IHyRenderer;
	friend class HyRenderBuffer;

protected:
	static HyScene *				sm_pScene;

	HyShaderHandle					m_hShader;
	HyRenderMode					m_eRenderMode;
	HyTextureHandle					m_hTextureHandle;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyDrawable();
	IHyDrawable(const IHyDrawable &copyRef);
	virtual ~IHyDrawable();

	const IHyDrawable &operator=(const IHyDrawable &rhs);

	HyRenderMode GetRenderMode() const;
	HyTextureHandle GetTextureHandle() const;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();
	const HyShaderUniforms &GetShaderUniforms() const;

protected:
	virtual bool IsValid() = 0;
	virtual void DrawLoadedUpdate() { }			// Optional override for derived classes
	virtual void OnUpdateUniforms() { }			// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) { }

private:
	virtual IHyNode &_DrawableGetNodeRef() = 0;
};

#endif /* IHyDrawable_h__ */
