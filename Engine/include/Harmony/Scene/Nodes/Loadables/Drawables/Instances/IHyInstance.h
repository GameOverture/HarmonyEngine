/**************************************************************************
*	IHyInstance.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyInstance_h__
#define IHyInstance_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Scene/Nodes/IHyNode.h"

class IHyInstance
{
	friend class HyScene;
	friend class IHyRenderer;
	friend class HyRenderBuffer;

protected:
	HyShaderHandle					m_hShader;
	HyRenderMode					m_eRenderMode;
	HyTextureHandle					m_hTextureHandle;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyInstance();
	IHyInstance(const IHyInstance &copyRef);
	IHyInstance(IHyInstance &&donor);
	virtual ~IHyInstance();

	IHyInstance &operator=(const IHyInstance &rhs);
	IHyInstance &operator=(IHyInstance &&donor);

	HyRenderMode GetRenderMode() const;
	HyTextureHandle GetTextureHandle() const;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();
	const HyShaderUniforms &GetShaderUniforms() const;

protected:
	virtual bool IsValidToRender() = 0;
	virtual void OnUpdateUniforms() { }			// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) { }

private:
	virtual IHyNode &_DrawableGetNodeRef() = 0;
};

#endif /* IHyInstance_h__ */
