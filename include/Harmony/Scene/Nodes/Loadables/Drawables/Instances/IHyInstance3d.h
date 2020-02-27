/**************************************************************************
*	IHyInstance3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyInstance3d_h__
#define IHyInstance3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"

class IHyInstance3d : public IHyDrawable3d, public IHyInstance
{
	friend class HyScene;

public:
	IHyInstance3d(HyType eInstType, std::string sPrefix, std::string sName, HyEntity3d *pParent);
	IHyInstance3d(const IHyInstance3d &copyRef);
	IHyInstance3d(IHyInstance3d &&donor);
	virtual ~IHyInstance3d();

	IHyInstance3d &operator=(const IHyInstance3d &rhs);
	IHyInstance3d &operator=(IHyInstance3d &&donor);

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

#endif /* IHyInstance3d_h__ */
