/**************************************************************************
*	IHyLeafDraw2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "HyEngine.h"

/*static*/ HyAssets *IHyLeafDraw2d::sm_pHyAssets = nullptr;

IHyLeafDraw2d::IHyLeafDraw2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyNodeDraw2d(eNodeType, pParent),
																												m_eLoadState(HYLOADSTATE_Inactive),
																												m_pData(nullptr),
																												m_sPREFIX(szPrefix ? szPrefix : ""),
																												m_sNAME(szName ? szName : "")
{
}

IHyLeafDraw2d::~IHyLeafDraw2d()
{
	Unload();
}

const std::string &IHyLeafDraw2d::GetName()
{
	return m_sNAME;
}

const std::string &IHyLeafDraw2d::GetPrefix()
{
	return m_sPREFIX;
}

bool IHyLeafDraw2d::IsLoaded() const
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

IHyNodeData *IHyLeafDraw2d::AcquireData()
{
	if(m_pData == nullptr)
	{
		sm_pHyAssets->GetNodeData(this, m_pData);
		if(m_pData)
		{
			MakeBoundingVolumeDirty();
			OnDataAcquired();
		}
	}

	return m_pData;
}

HyCoordinateType IHyLeafDraw2d::GetCoordinateType()
{
	return m_RenderState.IsEnabled(HyRenderState::USINGSCREENCOORDS) ? HYCOORDTYPE_Screen : HYCOORDTYPE_Camera;
}

void IHyLeafDraw2d::SetCoordinateType(HyCoordinateType eCoordType)
{
	HyAssert(m_pParent == nullptr, "IHyDraw2d::SetCoordinateType() should only be set on a top level node (i.e. not a child node)");

	if(eCoordType == HYCOORDTYPE_Default)
		eCoordType = HyDefaultCoordinateType();

	if(eCoordType == HYCOORDTYPE_Screen)
		m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
	else
		m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);

	//for(uint32 i = 0; i < m_ChildList.size(); ++i)
	//{
	//	if(m_ChildList[i]->IsDraw2d())
	//	{
	//		if(eCoordType == HYCOORDTYPE_Screen)
	//			static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
	//		else
	//			static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);
	//	}
	//}
}

int32 IHyLeafDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void IHyLeafDraw2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

int32 IHyLeafDraw2d::GetShaderId()
{
	return m_RenderState.GetShaderId();
}

void IHyLeafDraw2d::SetCustomShader(IHyShader *pShader)
{
	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "IHyDraw2d::SetCustomShader was used on an already loaded instance - I can make this work I just haven't yet");
	HyAssert(pShader->IsFinalized(), "IHyDraw2d::SetCustomShader tried to set a non-finalized shader");
	HyAssert(pShader->GetId() >= HYSHADERPROG_CustomStartIndex, "HyGfxData::SetRequiredCustomShaderId was passed an invalid custom shader Id");

	m_RequiredCustomShaders.clear();
	m_RequiredCustomShaders.insert(pShader->GetId());
	m_RenderState.SetShaderId(pShader->GetId());
}

void IHyLeafDraw2d::Load()
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Load was invoked before engine has been initialized");

	if(GetCoordinateType() == HYCOORDTYPE_Default)
		SetCoordinateType(HyDefaultCoordinateType());
	if(GetCoordinateUnit() == HYCOORDUNIT_Default)
		SetCoordinateUnit(HyDefaultCoordinateUnit(), false);

	if(m_eTYPE != HYTYPE_Entity2d)
	{
		m_RequiredAtlasIndices.clear();
		AcquireData();
		if(m_pData)
			m_pData->AppendRequiredAtlasIndices(m_RequiredAtlasIndices);

		sm_pHyAssets->LoadGfxData(this);
	}

	// Load any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			static_cast<IHyDraw2d *>(m_ChildList[i])->Load();
	}
}

/*virtual*/ void IHyLeafDraw2d::Unload()
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Unload was invoked before engine has been initialized");

	// Unload any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			static_cast<IHyDraw2d *>(m_ChildList[i])->Unload();
	}
	
	sm_pHyAssets->RemoveGfxData(this);
}

IHyNodeData *IHyLeafDraw2d::UncheckedGetData()
{
	return m_pData;
}

const HyRenderState &IHyLeafDraw2d::GetRenderState() const
{
	return m_RenderState;
}

void IHyLeafDraw2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

