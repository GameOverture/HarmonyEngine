/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/HyEntity2d.h"

#include "Creator/HyCreator.h"
#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyText2d.h"
#include "Creator/Instances/HyPrimitive2d.h"

/*static*/ HyCreator *HyEntity2d::sm_pCtor = NULL;

// Hidden ctor used within AddChild()
HyEntity2d::HyEntity2d(HyEntity2d *pParent) : m_kpParent(pParent)
{
	CtorInit();
}

// Public ctor
HyEntity2d::HyEntity2d(void) : m_kpParent(NULL)
{
	CtorInit();
}

HyEntity2d::~HyEntity2d(void)
{
	Clear(false);

	//for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
	//	m_vChildNodes[i]->m_kpParent = NULL;
}

void HyEntity2d::CtorInit()
{
	m_bDirty = true;
	m_eCoordType = HyCreator::DefaultCoordinateType();
	SetOnDirtyCallback(OnDirty, this);

}

void HyEntity2d::Update()
{
	
}

IObjInst2d *HyEntity2d::Set(HyInstanceType eType, const char *szPrefix, const char *szName)
{
	HyAssert(sm_pCtor, "HyEntity2d::Set() cannot be used before IGame::Initialize() is invoked.");

	IObjInst2d *pNewInst = NULL;
	switch(eType)
	{
	case HYINST_Sprite2d:	pNewInst = new HySprite2d(szPrefix, szName);	break;
	case HYINST_Spine2d:	pNewInst = new HySpine2d(szPrefix, szName);		break;
	case HYINST_Text2d:		pNewInst = new HyText2d(szPrefix, szName);		break;
	default:
		break;
	}

	if(pNewInst != NULL)
	{
		LinkInst(pNewInst);
	}

	return pNewInst;
}

HySprite2d *HyEntity2d::SetSprite(const char *szPrefix, const char *szName)
{
	HySprite2d *pNewInst = new HySprite2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HySpine2d *HyEntity2d::SetSpine(const char *szPrefix, const char *szName)
{
	HySpine2d *pNewInst = new HySpine2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HyText2d *HyEntity2d::SetText(const char *szPrefix, const char *szName)
{
	HyText2d *pNewInst = new HyText2d(szPrefix, szName);
	LinkInst(pNewInst);

	return pNewInst;
}

HyPrimitive2d *HyEntity2d::SetPrimitive()
{
	HyPrimitive2d *pNewInst = new HyPrimitive2d();
	LinkInst(pNewInst);

	return pNewInst;
}

HySprite2d *HyEntity2d::GetSprite()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Sprite2d)
			return reinterpret_cast<HySprite2d *>(m_vInstList[i]);
	}
	return NULL;
}
HySpine2d *HyEntity2d::GetSpine()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Spine2d)
			return reinterpret_cast<HySpine2d *>(m_vInstList[i]);
	}
	return NULL;
}
HyText2d *HyEntity2d::GetText()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Text2d)
			return reinterpret_cast<HyText2d *>(m_vInstList[i]);
	}
	return NULL;
}
HyPrimitive2d *HyEntity2d::GetPrimitive()
{
	for(uint32 i = 0; i < m_vInstList.size(); ++i)
	{
		if(m_vInstList[i]->GetInstType() == HYINST_Primitive2d)
			return reinterpret_cast<HyPrimitive2d *>(m_vInstList[i]);
	}
	return NULL;
}

void HyEntity2d::LinkInst(IObjInst2d *pInst)
{
	//// TODO: fix this code. Handle default more eloquently
	//if(GetCoordinateType() == HYCOORD_Default && HyCreator::DefaultCoordinateType() != HYCOORD_Default)
	//	SetCoordinateType(HyCreator::DefaultCoordinateType(), true);

	//// Call LoadInst2d() before SetParent() to prevent accidentially deleting the instance's IData if
	//// it's the only associated instance of the data.
	//sm_pCtor->LoadInst2d(pInst);
	//pInst->SetParent(this);

	//m_vInstList.push_back(pInst);
}

bool HyEntity2d::Remove(IObjInst2d *pInst)
{
	HyAssert(sm_pCtor, "HyEntity2d::Remove() cannot be used before IGame::Initialize() is invoked.");
	
	for (vector<IObjInst2d *>::iterator iter = m_vInstList.begin(); iter != m_vInstList.end(); ++iter)
	{
		if((*iter) == pInst)
		{
			Erase(iter);
			return true;
		}
	}

	return false;
}

void HyEntity2d::Clear(bool bClearChildren)
{
	while(m_vInstList.empty() == false)
		Erase(--m_vInstList.end());

	if(bClearChildren)
	{
		for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
			m_vChildNodes[i]->Clear(true);
	}
}

HyEntity2d *HyEntity2d::AddChild()
{
	HyEntity2d *pEnt2d = new HyEntity2d(this);
	m_vChildNodes.push_back(pEnt2d);

	pEnt2d->SetDirty();

	return pEnt2d;
}

void HyEntity2d::RemoveChild(HyEntity2d *pEnt2d)
{
	for(vector<HyEntity2d *>::iterator iter = m_vChildNodes.begin(); iter != m_vChildNodes.end(); ++iter)
	{
		if((*iter) == pEnt2d)
		{
			m_vChildNodes.erase(iter);
			break;
		}
	}
}

void HyEntity2d::Erase(vector<IObjInst2d *>::iterator &iterRef)
{
	//sm_pCtor->RemoveInst((*iterRef));
	//(*iterRef)->RemoveParent();

	//m_vInstList.erase(iterRef);
}

void HyEntity2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_vChildNodes.size(); ++i)
		m_vChildNodes[i]->SetDirty();
}

void HyEntity2d::SetInstOrderingDirty()
{
	HyAssert(sm_pCtor, "HyEntity2d::SetInstOrderingDirty() cannot be used before IGame::Initialize() is invoked.");
	sm_pCtor->SetInstOrderingDirty();
}

void HyEntity2d::GetWorldTransform(mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_kpParent)
			m_kpParent->GetWorldTransform(m_mtxCached);

		mat4 mtxLocal;
		GetLocalTransform(mtxLocal);
	
		m_mtxCached *= mtxLocal;
		m_bDirty = false;
	}
	
	outMtx = m_mtxCached;
}

/*static*/ void HyEntity2d::OnDirty(void *pParam)
{
	HyEntity2d *pThis = reinterpret_cast<HyEntity2d *>(pParam);
	pThis->SetDirty();
}
