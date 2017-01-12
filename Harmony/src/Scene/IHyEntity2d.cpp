/**************************************************************************
 *	IHyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/IHyEntity2d.h"

IHyEntity2d::IHyEntity2d() :	IHyInst2d(HYTYPE_Entity2d, NULL, NULL),
							m_iDisplayOrderMax(0)
{
}

IHyEntity2d::IHyEntity2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Entity2d, szPrefix, szName),
																	m_iDisplayOrderMax(0)
{
}

IHyEntity2d::~IHyEntity2d(void)
{
}

/*virtual*/ void IHyEntity2d::SetEnabled(bool bEnabled)
{
	IHyTransformNode::SetEnabled(bEnabled);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetEnabled(m_bEnabled);
}

/*virtual*/ bool IHyEntity2d::IsLoaded() const
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
			if(static_cast<IHyEntity2d *>(m_ChildList[i])->IsLoaded() == false)
				return false;
			break;
		}
	}

	return true;
}

int32 IHyEntity2d::GetDisplayOrderMax()
{
	return m_iDisplayOrderMax;
}

/*virtual*/ void IHyEntity2d::SetDisplayOrder(int32 iOrderValue)
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
			static_cast<IHyEntity2d *>(m_ChildList[i])->SetDisplayOrder(iOrderValue);
			iOrderValue = static_cast<IHyEntity2d *>(m_ChildList[i])->GetDisplayOrderMax();
			break;
		}
	}

	iOrderValue--;
	m_iDisplayOrderMax = iOrderValue;
}
