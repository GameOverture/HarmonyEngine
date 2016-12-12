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

HyEntity2d::HyEntity2d() :	IHyInst2d(HYTYPE_Entity2d, NULL, NULL),
							m_iDisplayOrderMax(0)
{
}

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Entity2d, szPrefix, szName),
																	m_iDisplayOrderMax(0)
{
}

HyEntity2d::~HyEntity2d(void)
{
}

/*virtual*/ void HyEntity2d::SetEnabled(bool bEnabled)
{
	IHyTransformNode::SetEnabled(bEnabled);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetEnabled(m_bEnabled);
}

/*virtual*/ bool HyEntity2d::IsLoaded()
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
			if(static_cast<IHyInst2d *>(m_ChildList[i])->IsLoaded() == false)
				return false;
			break;
		case HYTYPE_Entity2d:
			if(static_cast<HyEntity2d *>(m_ChildList[i])->IsLoaded() == false)
				return false;
			break;
		}
	}

	return true;
}

/*virtual*/ void HyEntity2d::SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom)
{
	m_eCoordType = eCoordType;

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
			static_cast<IHyInst2d *>(m_ChildList[i])->SetCoordinateType(eCoordType, pCameraToCovertFrom);
			break;
		case HYTYPE_Entity2d:
			static_cast<HyEntity2d *>(m_ChildList[i])->SetCoordinateType(eCoordType, pCameraToCovertFrom);
			break;
		}
	}
}

int32 HyEntity2d::GetDisplayOrderMax()
{
	return m_iDisplayOrderMax;
}

/*virtual*/ void HyEntity2d::SetDisplayOrder(int32 iOrderValue)
{
	IHyInst2d::SetDisplayOrder(iOrderValue);

	for(uint32 i = 0; i < m_ChildList.size(); ++i, ++iOrderValue)
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
			break;
		case HYTYPE_Entity2d:
			static_cast<HyEntity2d *>(m_ChildList[i])->SetDisplayOrder(iOrderValue);
			iOrderValue = static_cast<HyEntity2d *>(m_ChildList[i])->GetDisplayOrderMax();
			break;
		}
	}

	iOrderValue--;
	m_iDisplayOrderMax = iOrderValue;
}


/*virtual*/ void HyEntity2d::Load()
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		switch(m_ChildList[i]->GetType())
		{
		case HYTYPE_Sound2d:
		case HYTYPE_Particles2d:
		case HYTYPE_Sprite2d:
		case HYTYPE_Spine2d:
		case HYTYPE_TexturedQuad2d:
		case HYTYPE_Primitive2d:
		case HYTYPE_Text2d:
			static_cast<IHyInst2d *>(m_ChildList[i])->Load();
			break;
		case HYTYPE_Entity2d:
			static_cast<HyEntity2d *>(m_ChildList[i])->Load();
			break;
		}
	}
}

/*virtual*/ void HyEntity2d::Unload()
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		switch(m_ChildList[i]->GetType())
		{
		case HYTYPE_Sound2d:
		case HYTYPE_Particles2d:
		case HYTYPE_Sprite2d:
		case HYTYPE_Spine2d:
		case HYTYPE_TexturedQuad2d:
		case HYTYPE_Primitive2d:
		case HYTYPE_Text2d:
			static_cast<IHyInst2d *>(m_ChildList[i])->Unload();
			break;
		case HYTYPE_Entity2d:
			static_cast<HyEntity2d *>(m_ChildList[i])->Unload();
			break;
		}
	}
}
