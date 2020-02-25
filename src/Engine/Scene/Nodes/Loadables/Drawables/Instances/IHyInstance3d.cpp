/**************************************************************************
*	IHyInstance3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h"
#include "HyEngine.h"

IHyInstance3d::IHyInstance3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :
	IHyDrawable3d(eNodeType, szPrefix, szName, pParent)
{
}

IHyInstance3d::IHyInstance3d(const IHyInstance3d &copyRef) :
	IHyDrawable3d(copyRef),
	IHyInstance(copyRef)
{
}

IHyInstance3d::~IHyInstance3d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

const IHyInstance3d &IHyInstance3d::operator=(const IHyInstance3d &rhs)
{
	IHyLoadable3d::operator=(rhs);
	IHyInstance::operator=(rhs);

	return *this;
}

/*virtual*/ void IHyInstance3d::Update() /*override final*/
{
	IHyDrawable3d::Update();
}

/*virtual*/ bool IHyInstance3d::IsValid() /*override final*/
{
	return m_bVisible && OnIsValid();
}

/*virtual*/ void IHyInstance3d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyInstance3d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}

/*virtual*/ IHyNode &IHyInstance3d::_DrawableGetNodeRef() /*override final*/
{
	return *this;
}
