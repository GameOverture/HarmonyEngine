/*******************************************************************************
 *	IHyDraw2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *********************************************************************************/
#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "HyEngine.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Misc/HyCamera.h"
#include "Scene/Nodes/HyEntity2d.h"
#include "Renderer/Components/HyWindow.h"
#include "Diagnostics/HyGuiComms.h"

/*static*/ HyAssets *IHyDraw2d::sm_pHyAssets = nullptr;

IHyDraw2d::IHyDraw2d(HyType eInstType, const char *szPrefix, const char *szName, IHyNode2d *pParent /*= nullptr*/) :	IHyNode2d(eInstType, pParent),
																														m_eLoadState(HYLOADSTATE_Inactive),
																														m_sPREFIX(szPrefix ? szPrefix : ""),
																														m_sNAME(szName ? szName : ""),
																														m_pData(nullptr),
																														m_uiAttributes(0),
																														m_eMouseInputState(MOUSEINPUT_None),
																														m_pMouseInputUserParam(nullptr),
																														m_iDisplayOrder(0),
																														topColor(*this),
																														botColor(*this),
																														m_fAlpha(1.0f),
																														m_fPrevAlphaValue(1.0f),
																														alpha(m_fAlpha, *this)
{
	m_bIsDraw2d = true;

#ifdef HY_DEBUG
	if(m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d)
		HyAssert(m_sNAME.empty() == false, "IHyDraw2d of type '" << m_eTYPE << "' was constructed with a blank name");
#endif

	topColor.Set(1.0f);
	botColor.Set(1.0f);


	if(m_pParent && m_pParent->IsDraw2d())
	{
		if(static_cast<IHyDraw2d *>(m_pParent)->m_RenderState.IsScissorRect())
		{
			m_RenderState.SetScissorRect(static_cast<IHyDraw2d *>(m_pParent)->m_RenderState.GetScissorRect());
		}
	}
}

/*virtual*/ IHyDraw2d::~IHyDraw2d(void)
{
	Unload();
}

const std::string &IHyDraw2d::GetName()
{
	return m_sNAME;
}

const std::string &IHyDraw2d::GetPrefix()
{
	return m_sPREFIX;
}

IHyNodeData *IHyDraw2d::AcquireData()
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

HyCoordinateType IHyDraw2d::GetCoordinateType()
{
	return m_RenderState.IsEnabled(HyRenderState::USINGSCREENCOORDS) ? HYCOORDTYPE_Screen : HYCOORDTYPE_Camera;
}

void IHyDraw2d::SetCoordinateType(HyCoordinateType eCoordType)
{
	HyAssert(m_pParent == nullptr, "IHyDraw2d::SetCoordinateType() should only be set on a top level node (i.e. not a child node)");

	if(eCoordType == HYCOORDTYPE_Default)
		eCoordType = HyDefaultCoordinateType();

	if(eCoordType == HYCOORDTYPE_Screen)
		m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
	else
		m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
		{
			if(eCoordType == HYCOORDTYPE_Screen)
				static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
			else
				static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);
		}
	}
}

int32 IHyDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

// Returns the max display order assigned to children
int32 IHyDraw2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren /*= true*/)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			iOrderValue = static_cast<IHyDraw2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
	}

	HyScene::SetInstOrderingDirty();

	return iOrderValue;
}

void IHyDraw2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyDraw2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

void IHyDraw2d::EnableMouseInput(bool bEnable, void *pUserParam /*= NULL*/)
{
	if(bEnable)
		m_uiAttributes |= (ATTRIBFLAG_MouseInput | ATTRIBFLAG_BoundingVolumeDirty);
	else
		m_uiAttributes &= ~ATTRIBFLAG_MouseInput;

	m_pMouseInputUserParam = pUserParam;
}

void IHyDraw2d::EnableCollider(bool bEnable)
{
	if(bEnable)
		m_uiAttributes |= (ATTRIBFLAG_HasBoundingVolume | ATTRIBFLAG_BoundingVolumeDirty);
	else
		m_uiAttributes &= ~ATTRIBFLAG_HasBoundingVolume;
}

void IHyDraw2d::EnablePhysics(bool bEnable)
{
}

bool IHyDraw2d::IsScissorSet()
{
	return (0 != (m_uiAttributes & ATTRIBFLAG_Scissor));
}

const HyScreenRect<int32> &IHyDraw2d::GetScissor()
{
	return m_LocalScissorRect;
}

void IHyDraw2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	m_LocalScissorRect.x = uiLocalX;
	m_LocalScissorRect.y = uiLocalY;
	m_LocalScissorRect.width = uiWidth;
	m_LocalScissorRect.height = uiHeight;

	m_uiAttributes |= ATTRIBFLAG_Scissor;
}

void IHyDraw2d::ClearScissor()
{
	m_uiAttributes &= ~ATTRIBFLAG_Scissor;
	m_RenderState.ClearScissorRect();

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.ClearScissorRect();
	}
}

int32 IHyDraw2d::GetShaderId()
{
	return m_RenderState.GetShaderId();
}

void IHyDraw2d::SetCustomShader(IHyShader *pShader)
{
	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "IHyDraw2d::SetCustomShader was used on an already loaded instance - I can make this work I just haven't yet");
	HyAssert(pShader->IsFinalized(), "IHyDraw2d::SetCustomShader tried to set a non-finalized shader");
	HyAssert(pShader->GetId() >= HYSHADERPROG_CustomStartIndex, "HyGfxData::SetRequiredCustomShaderId was passed an invalid custom shader Id");

	m_RequiredCustomShaders.clear();
	m_RequiredCustomShaders.insert(pShader->GetId());
	m_RenderState.SetShaderId(pShader->GetId());
}

bool IHyDraw2d::IsLoaded() const
{
	if(m_eTYPE != HYTYPE_Entity2d && m_eLoadState != HYLOADSTATE_Loaded)
		return false;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d() && static_cast<IHyDraw2d *>(m_ChildList[i])->IsLoaded() == false)
			return false;
	}

	return true;
}

void IHyDraw2d::Load()
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

/*virtual*/ void IHyDraw2d::Unload()
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

bool IHyDraw2d::IsSelfLoaded()
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

IHyNodeData *IHyDraw2d::UncheckedGetData()
{
	return m_pData;
}

void IHyDraw2d::MakeBoundingVolumeDirty()
{
	m_uiAttributes |= ATTRIBFLAG_BoundingVolumeDirty;
}

/*virtual*/ void IHyDraw2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	IHyNode2d::SetNewChildAttributes(childInst);

	if(childInst.IsDraw2d())
	{
		if(GetCoordinateType() == HYCOORDTYPE_Screen)
			static_cast<IHyDraw2d &>(childInst).m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
		else
			static_cast<IHyDraw2d &>(childInst).m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);

		static_cast<IHyDraw2d &>(childInst).alpha.Set(this->alpha.Get());

		// This will reassign all the display orders in the hierarchy, including this newly added child
		//_SetDisplayOrder(m_iDisplayOrder, false);
	}
}

const HyRenderState &IHyDraw2d::GetRenderState() const
{
	return m_RenderState;
}

void IHyDraw2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

/*virtual*/ void IHyDraw2d::InstUpdate()
{
	if((m_uiAttributes & ATTRIBFLAG_Scissor) != 0)
	{
		glm::mat4 mtx;
		GetWorldTransform(mtx);

		m_RenderState.SetScissorRect(static_cast<int32>(mtx[3].x + m_LocalScissorRect.x),
									 static_cast<int32>(mtx[3].y + m_LocalScissorRect.y),
									 static_cast<uint32>(mtx[0].x * m_LocalScissorRect.width),
									 static_cast<uint32>(mtx[1].y * m_LocalScissorRect.height));

		ForEachChild([&](IHyNode2d *pChildNode)
					{
						if(pChildNode->IsDraw2d())
							static_cast<IHyDraw2d *>(pChildNode)->m_RenderState.SetScissorRect(this->m_RenderState.GetScissorRect());
					});
	}

	if((m_uiAttributes & (ATTRIBFLAG_HasBoundingVolume | ATTRIBFLAG_MouseInput)) != 0)
	{
		if(m_uiAttributes & ATTRIBFLAG_BoundingVolumeDirty)
		{
			OnCalcBoundingVolume();
			m_uiAttributes &= ~ATTRIBFLAG_BoundingVolumeDirty;
		}

		if((m_uiAttributes & ATTRIBFLAG_MouseInput) != 0)
		{
			bool bLeftClickDown = IHyInputMap::IsMouseLeftDown();
			bool bMouseInBounds = m_BoundingVolume.IsWorldPointCollide(IHyInputMap::GetWorldMousePos());

			switch(m_eMouseInputState)
			{
			case MOUSEINPUT_None:
				if(bLeftClickDown == false && bMouseInBounds)
				{
					m_eMouseInputState = MOUSEINPUT_Hover;
					OnMouseEnter(m_pMouseInputUserParam);
				}
				break;

			case MOUSEINPUT_Hover:
				if(bMouseInBounds == false)
				{
					m_eMouseInputState = MOUSEINPUT_None;
					OnMouseLeave(m_pMouseInputUserParam);
				}
				else if(bLeftClickDown)
				{
					m_eMouseInputState = MOUSEINPUT_Down;
					OnMouseDown(m_pMouseInputUserParam);
				}
				break;

			case MOUSEINPUT_Down:
				if(bLeftClickDown == false)
				{
					m_eMouseInputState = MOUSEINPUT_None;
					OnMouseUp(m_pMouseInputUserParam);

					if(bMouseInBounds)
						OnMouseClicked(m_pMouseInputUserParam);
				}
				break;
			}
		}
	}

	if(m_fPrevAlphaValue != alpha.Get())
	{
		m_fPrevAlphaValue = alpha.Get();

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsDraw2d())
				static_cast<IHyDraw2d *>(m_ChildList[i])->alpha.Set(alpha.Get());
		}

		//ForEachChild([&](IHyNode2d *pChildNode)
		//			{
		//				if(pChildNode->IsDraw2d())
		//				{
		//					static_cast<IHyDraw2d *>(pChildNode)->alpha.Set(this->alpha.Get());
		//				}
		//			});
	}

	OnUpdate();

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		OnUpdateUniforms();
		m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
	}
}

int32 IHyDraw2d::_SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
	{
		++iOrderValue;
		m_iDisplayOrder = iOrderValue;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsDraw2d())
				iOrderValue = static_cast<IHyDraw2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}

	return iOrderValue;
}
