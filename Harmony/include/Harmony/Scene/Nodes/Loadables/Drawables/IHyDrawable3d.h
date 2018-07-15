/**************************************************************************
*	IHyDrawable3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable3d_h__
#define IHyDrawable3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"

class HyStencil;
class HyPortal2d;

class IHyDrawable3d : public IHyLoadable3d
{
	friend class HyScene;
	friend class IHyRenderer;
	friend class HyShape2d;

protected:
	static HyScene *				sm_pScene;

	HyShaderHandle					m_hShader;
	HyRenderMode					m_eRenderMode;
	HyTextureHandle					m_hTextureHandle;
	HyShaderUniforms 				m_ShaderUniforms;

public:
	IHyDrawable3d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity3d *pParent);
	IHyDrawable3d(const IHyDrawable3d &copyRef);
	virtual ~IHyDrawable3d();

	const IHyDrawable3d &operator=(const IHyDrawable3d &rhs);
	virtual IHyDrawable3d *Clone() const = 0;

	bool IsValid();

	HyRenderMode GetRenderMode() const;
	HyTextureHandle GetTextureHandle() const;

	// Passing nullptr will use built-in default shader
	void SetShader(HyShader *pShader);
	HyShaderHandle GetShaderHandle();

protected:
	virtual void NodeUpdate() override final;

	void WriteShaderUniformBuffer(char *&pRefDataWritePos);

	virtual void OnLoaded() override;									// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() override;									// HyAssets invokes this instance's data has been erased
	virtual bool OnIsValid() { return true; }
	virtual void OnShapeSet(HyShape2d *pShape) { }
	virtual void CalcBoundingVolume() { }
	virtual void OnUpdateUniforms() { }									// Upon updating, this function will set the shaders' uniforms when using the default shader
	virtual void OnWriteVertexData(char *&pRefDataWritePos) { }			// This function is responsible for incrementing the passed in reference pointer the size of the data written


#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(jsonxx::Value &dataValueRef, HyTextureHandle hTextureHandle)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE(HY_GUI_DATAOVERRIDE, dataValueRef, *sm_pHyAssets);
		OnDataAcquired();

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

		if(hTextureHandle != HY_UNUSED_HANDLE)
			m_hTextureHandle = hTextureHandle;
	}
#endif
};

#endif /* IHyDrawable3d_h__ */
