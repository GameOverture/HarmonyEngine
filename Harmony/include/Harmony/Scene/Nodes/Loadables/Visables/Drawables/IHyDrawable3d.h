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
#include "Scene/Nodes/Loadables/Visables/IHyVisable3d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"

class IHyDrawable3d : public IHyVisable3d, public IHyDrawable
{
	friend class HyScene;

public:
	IHyDrawable3d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity3d *pParent);
	IHyDrawable3d(const IHyDrawable3d &copyRef);
	virtual ~IHyDrawable3d();

	const IHyDrawable3d &operator=(const IHyDrawable3d &rhs);
	virtual IHyDrawable3d *Clone() const = 0;

protected:
	virtual void Update() override final;
	virtual bool IsValid() override final;

	virtual void OnLoaded() override;									// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() override;									// HyAssets invokes this instance's data has been erased

	// Optional user overrides below
	virtual bool OnIsValid() { return true; }

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(jsonxx::Value &dataValueRef, HyTextureHandle hTextureHandle)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE(HY_GUI_DATAOVERRIDE, dataValueRef, *IHyLoadable::sm_pHyAssets);
		OnDataAcquired();

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

		if(hTextureHandle != HY_UNUSED_HANDLE)
			m_hTextureHandle = hTextureHandle;
	}
#endif

private:
	virtual IHyNode &_DrawableGetNodeRef() override final;
};

#endif /* IHyDrawable3d_h__ */
