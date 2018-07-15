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

class IHyDrawable
{
	friend class IHyRenderer;

protected:
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

protected:
	virtual bool IsValid() = 0;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteVertexData(char *&pRefDataWritePos) { }			// This function is responsible for incrementing the passed in reference pointer the size of the data written

private:
	virtual HyType _DrawableGetType() = 0;
};

#endif /* IHyDrawable_h__ */
