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
	HyRenderMode					m_eRenderMode; //start here!
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyDrawable();
	IHyDrawable(const IHyDrawable &copyRef);
	IHyDrawable(IHyDrawable &&donor) noexcept;
	virtual ~IHyDrawable();

	IHyDrawable &operator=(const IHyDrawable &rhs);
	IHyDrawable &operator=(IHyDrawable &&donor) noexcept;

	HyRenderMode GetRenderMode() const;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();
	HyShaderUniforms &GetShaderUniforms();

protected:
	virtual bool IsValidToRender() = 0;

	//start_here_too
	//virtual void GetRenderInfo(HyRenderMode &eRenderModeOut, HyShaderHandle &hShaderHandleOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstanceOut) = 0;

	// Returns 'true' if finished writing vertex data. Returns 'false' if the render state needs to change (aka can't batch)
	// and needs to loop within HyRenderBuffer::AppendRenderState
	virtual bool WriteVertexData(HyVertexBuffer &vertexBufferRef) = 0;

private:
	virtual IHyNode &_DrawableGetNodeRef() = 0;
};

#endif /* IHyDrawable_h__ */
