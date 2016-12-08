/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyEntity2d.h"

HyEntity2d::HyEntity2d() :	IHyTransform2d(HYTYPE_Entity2d),
							m_eLoadState(HYLOADSTATE_Inactive)
{
}

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName) :	IHyTransform2d(HYTYPE_Entity2d), 
																	m_eLoadState(HYLOADSTATE_Inactive)
{
}

HyEntity2d::~HyEntity2d(void)
{
}

void HyEntity2d::SetEnabled(bool bEnabled)
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetEnabled(bEnabled);
}

void HyEntity2d::SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom)
{
}

void HyEntity2d::SetDisplayOrder(int32 iOrderValue)
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		switch(m_ChildList[i]->GetType())
		{
		case HYTYPE_Particles2d:
		case HYTYPE_Sprite2d:
		case HYTYPE_Spine2d:
		case HYTYPE_TexturedQuad2d:
		case HYTYPE_Primitive2d:
		case HYTYPE_Text2d:
			static_cast<IHyInst2d *>(m_ChildList[i])->SetDisplayOrder(iOrderValue);
			iOrderValue++;
			break;
		}
	}
}

void HyEntity2d::SetTint(float fR, float fG, float fB)
{
}

void HyEntity2d::SetTransparency(float fTransparency)
{
}

void HyEntity2d::Load()
{
}

void HyEntity2d::Unload()
{
}

/*virtual*/ void HyEntity2d::OnUpdate()
{
}
