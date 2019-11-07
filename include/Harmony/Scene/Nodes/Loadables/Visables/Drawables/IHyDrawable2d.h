/**************************************************************************
*	IHyDrawable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable2d_h__
#define IHyDrawable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/IHyVisable2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable.h"
#include "Scene/Physics/HyShape2d.h"
#include "Renderer/Effects/HyShader.h"
#include "Renderer/Components/HyShaderUniforms.h"

class IHyDrawable2d : public IHyVisable2d, public IHyDrawable
{
	friend class HyScene;

protected:
	HyShape2d						m_LocalBoundingVolume;

public:
	IHyDrawable2d(HyType eInstType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	IHyDrawable2d(const IHyDrawable2d &copyRef);
	virtual ~IHyDrawable2d();

	const IHyDrawable2d &operator=(const IHyDrawable2d &rhs);
	virtual IHyDrawable2d *Clone() const = 0;

	const HyShape2d &GetLocalBoundingVolume();
	virtual const b2AABB &GetWorldAABB() override;

protected:
	virtual void Update() override final;
	virtual bool IsValid() override final;

	virtual void OnLoaded() override;
	virtual void OnUnloaded() override;

	// Optional user overrides below
	virtual bool OnIsValid() { return true; }
	virtual void OnCalcBoundingVolume() { }

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

#endif /* IHyDrawable2d_h__ */
