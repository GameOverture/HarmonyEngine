/**************************************************************************
*	IHyInstance2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyInstance2d_h__
#define IHyInstance2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.h"
#include "Scene/Physics/HyShape2d.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"

class IHyInstance2d : public IHyDrawable2d, public IHyInstance
{
	friend class HyScene;

protected:
	HyShape2d						m_LocalBoundingVolume;

public:
	IHyInstance2d(HyType eInstType, std::string sPrefix, std::string sName, HyEntity2d *pParent);
	IHyInstance2d(const IHyInstance2d &copyRef);
	IHyInstance2d(IHyInstance2d &&donor) noexcept;
	virtual ~IHyInstance2d();

	IHyInstance2d &operator=(const IHyInstance2d &rhs);
	IHyInstance2d &operator=(IHyInstance2d &&donor) noexcept;

	const HyShape2d &GetLocalBoundingVolume();
	virtual const b2AABB &GetWorldAABB() override;

protected:
	virtual void Update() override final;
	virtual bool IsValidToRender() override final;

	virtual void OnLoaded() override;
	virtual void OnUnloaded() override;

	// Optional user overrides below
	virtual bool OnIsValidToRender() { return true; }
	virtual void OnCalcBoundingVolume() { }

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(HyJsonObj itemDataObj, HyTextureHandle hTextureHandle)
	{
		delete m_pData;
		m_pData = HY_NEW HYDATATYPE(HY_GUI_DATAOVERRIDE, itemDataObj, *IHyLoadable::sm_pHyAssets);
		OnDataAcquired();

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = Hy_DefaultShaderHandle(GetType());

		if(hTextureHandle != HY_UNUSED_HANDLE)
			m_hTextureHandle = hTextureHandle;
	}
#endif

private:
	virtual IHyNode &_DrawableGetNodeRef() override final;
};

#endif /* IHyInstance2d_h__ */
