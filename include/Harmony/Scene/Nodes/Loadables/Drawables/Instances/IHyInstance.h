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
	static HyScene *				sm_pScene;

	HyShaderHandle					m_hShader;
	HyRenderMode					m_eRenderMode;
	HyTextureHandle					m_hTextureHandle;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyInstance();
	IHyInstance(const IHyInstance &copyRef);
	virtual ~IHyInstance();

	const IHyInstance &operator=(const IHyInstance &rhs);

	HyRenderMode GetRenderMode() const;
	HyTextureHandle GetTextureHandle() const;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();
	const HyShaderUniforms &GetShaderUniforms() const;

protected:
	virtual bool IsValid() = 0;
	virtual void OnUpdateUniforms() { }			// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) { }

private:
	virtual IHyNode &_DrawableGetNodeRef() = 0;
};

#endif /* IHyInstance_h__ */
