/*******************************************************************************
 *	IObjInst2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *
 *	This software is provided 'as-is', without any express or implied 
 *	warranty. In no event will the authors be held liable for any damages 
 *	arising from the use of this software. 
 *
 *	Permission is granted to anyone to use this software for any purpose, 
 *	including commercial applications, and to alter it and redistribute it 
 *	freely, subject to the following restrictions: 
 *
 *     1. The origin of this software must not be misrepresented; you must not 
 *     claim that you wrote the original software. If you use this software 
 *     in a product, an acknowledgment in the product documentation would be 
 *     appreciated but is not required. 
 *
 *     2. Altered source versions must be plainly marked as such, and must not be 
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source 
 *     distribution. 
 *********************************************************************************/
#include "Creator/Instances/IObjInst2d.h"
#include "Creator/Data/IHyData.h"

#include "FileIO/HyFileIO.h"

/*static*/ HyCreator *IObjInst2d::sm_pCtor = NULL;

IObjInst2d::IObjInst2d(HyInstanceType eInstType, const char *szPrefix, const char *szName) :	m_keInstType(eInstType),
																								m_ksPath(HyFileIO::GetFilePath(m_keInstType, szPrefix, szName))
{
	CtorInit();
}

IObjInst2d::IObjInst2d(HyInstanceType eInstType, uint32 uiTextureIndex) :	m_keInstType(eInstType),
																			m_ksPath(std::to_string(uiTextureIndex))
{
	CtorInit();
}

/*virtual*/ IObjInst2d::~IObjInst2d(void)
{
	Unload();
}

void IObjInst2d::CtorInit()
{
	m_pDataPtr = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;
	m_pParent = NULL;
	m_bDirty = true;
	m_bEnabled = true;

	m_vColor.Set(1.0f);
	SetOnDirtyCallback(OnDirty, this);
}

void IObjInst2d::Load()
{
	// TODO: fix this code. Handle default more eloquently
	if(GetCoordinateType() == HYCOORD_Default && HyCreator::DefaultCoordinateType() != HYCOORD_Default)
		SetCoordinateType(HyCreator::DefaultCoordinateType(), true);

	sm_pCtor->LoadInst2d(this);
}

void IObjInst2d::Unload()
{
	sm_pCtor->RemoveInst(this);
}

void IObjInst2d::GetWorldTransform(mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_pParent)
			m_pParent->GetWorldTransform(m_mtxCached);

		mat4 mtxLocal;
		GetLocalTransform(mtxLocal);

		m_mtxCached *= mtxLocal;
		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}

void IObjInst2d::AddChild(IObjInst2d &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_vChildList.push_back(&childInst);
}

void IObjInst2d::Detach()
{
	if(m_pParent == NULL)
		return;

	for(vector<IObjInst2d *>::iterator iter = m_pParent->m_vChildList.begin(); iter != m_pParent->m_vChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_vChildList.erase(iter);
			return;
		}
	}

	HyError("IObjInst2d::Detach() could not find itself in parent's child list");
}

void IObjInst2d::SetDisplayOrder(float fOrderValue)
{
	m_fDisplayOrder = fOrderValue;

	HyAssert(sm_pCtor, "IObjInst2d::SetDisplayOrder() cannot be used before IGame::Initialize() is invoked.");
	sm_pCtor->SetInstOrderingDirty();
}

void IObjInst2d::SetData(IHyData *pData)
{
	m_pDataPtr = pData;
	
	if(m_pDataPtr == NULL)
		m_eLoadState = HYLOADSTATE_Loaded;
	else
		m_eLoadState = (m_pDataPtr->GetLoadState() == HYLOADSTATE_Loaded) ? HYLOADSTATE_Loaded : HYLOADSTATE_Queued;
}

void IObjInst2d::SetLoaded()
{
	m_eLoadState = HYLOADSTATE_Loaded;
	OnDataLoaded();
}

void IObjInst2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_vChildList.size(); ++i)
		m_vChildList[i]->SetDirty();
}

/*static*/ void IObjInst2d::OnDirty(void *pParam)
{
	IObjInst2d *pThis = reinterpret_cast<IObjInst2d *>(pParam);
	pThis->SetDirty();
}
