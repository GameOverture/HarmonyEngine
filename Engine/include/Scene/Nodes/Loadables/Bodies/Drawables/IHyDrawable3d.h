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
#include "Scene/Nodes/Loadables/Bodies/IHyBody3d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h"

class IHyDrawable3d : public IHyBody3d, public IHyDrawable
{
	friend class HyScene;

public:
	IHyDrawable3d(HyType eInstType, const HyNodePath &nodePath, HyEntity3d *pParent);
	IHyDrawable3d(const IHyDrawable3d &copyRef);
	IHyDrawable3d(IHyDrawable3d &&donor) noexcept;
	virtual ~IHyDrawable3d();

	IHyDrawable3d &operator=(const IHyDrawable3d &rhs);
	IHyDrawable3d &operator=(IHyDrawable3d &&donor) noexcept;

	virtual bool IsValidToRender() override final;

protected:
	virtual void Update() override final;

	virtual void OnLoaded() override;									// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() override;									// HyAssets invokes this instance's data has been erased

	// Optional user overrides below
	virtual bool OnIsValidToRender() { return true; }

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(HyJsonObj itemDataObj, HyTextureHandle hTextureHandle)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE(HY_GUI_DATAOVERRIDE, itemDataObj, *IHyLoadable::sm_pHyAssets);
		OnDataAcquired();

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = HyEngine::DefaultShaderHandle(GetType());

		if(m_uiState >= GetNumStates())
			m_uiState = 0;
	}
#endif

private:
	virtual IHyNode &_DrawableGetNodeRef() override final;
};

#endif /* IHyDrawable3d_h__ */
