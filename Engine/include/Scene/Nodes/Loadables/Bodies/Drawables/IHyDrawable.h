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
	HyShaderHandle					m_hShader;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyDrawable();
	IHyDrawable(const IHyDrawable &copyRef);
	IHyDrawable(IHyDrawable &&donor) noexcept;
	virtual ~IHyDrawable();

	IHyDrawable &operator=(const IHyDrawable &rhs);
	IHyDrawable &operator=(IHyDrawable &&donor) noexcept;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();
	HyShaderUniforms &GetShaderUniforms();

protected:
	virtual bool IsValidToRender() = 0;
	
	virtual void OnUpdateUniforms() { } // Invoked before AppendRenderState(), last chance to update ShaderUniforms. HyPrimitives need this or they glitch out upon new texture loads

	// Prepares the render stage for the next WriteVertexData. Also returns the required render state information for this stage.
	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) = 0;

	// Returns 'true' if finished writing vertex data. Returns 'false' if another render stage needs to be preformed, and the render state 
	// needs to change (loop within HyRenderBuffer::AppendRenderState)
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) = 0;

private:
	virtual IHyNode &_DrawableGetNodeRef() = 0;
};

#endif /* IHyDrawable_h__ */
