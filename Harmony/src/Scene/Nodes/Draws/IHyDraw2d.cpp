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

#include "IHyApplication.h"

#include "Assets/Data/IHyData.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Misc/HyCamera.h"
#include "Renderer/Components/HyWindow.h"

#include "Scene/Nodes/HyEntity2d.h"

#include "Diagnostics/HyGuiComms.h"

/*static*/ HyAssets *IHyDraw2d::sm_pHyAssets = NULL;

IHyDraw2d::IHyDraw2d(HyType eInstType, const char *szPrefix, const char *szName) :	IHyTransform2d(eInstType),
																					m_sPREFIX(szPrefix ? szPrefix : ""),
																					m_sNAME(szName ? szName : ""),
																					m_pData(NULL),
																					m_eLoadState(HYLOADSTATE_Inactive),
																					m_uiAttributes(0),
																					m_eMouseInputState(MOUSEINPUT_None),
																					m_pMouseInputUserParam(NULL),
																					m_iDisplayOrder(0),
																					m_iDisplayOrderMax(0),
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

IHyData &IHyDraw2d::GetData()
{
	if(m_pData == nullptr)
		sm_pHyAssets->GetNodeData(this, m_pData);

	return *m_pData;
}

HyCoordinateType IHyDraw2d::GetCoordinateType()
{
	return m_eCoordType;
}

void IHyDraw2d::SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom)
{
	if(eCoordType == HYCOORDTYPE_Default)
		eCoordType = IHyApplication::DefaultCoordinateType();

	if(pCameraToCovertFrom)
	{
		HyError("IHyDraw2d::SetCoordinateType() conversion code needs implementation");
		switch(eCoordType)
		{
		case HYCOORDTYPE_Camera:
			if(m_eCoordType == HYCOORDTYPE_Camera)
				break;

			pos.X(pCameraToCovertFrom->pos.X() + (pCameraToCovertFrom->GetWindow().GetResolution().x * 0.5f));
			pos.X(pCameraToCovertFrom->GetWindow().GetResolution().y * 0.5f);
			break;

		case HYCOORDTYPE_Screen:
			if(m_eCoordType == HYCOORDTYPE_Screen)
				break;

			//pos.X(pCameraToCovertFrom->
			break;
		}
	}
	
	m_eCoordType = eCoordType;
	if(m_eCoordType == HYCOORDTYPE_Screen)
		m_RenderState.Enable(HyRenderState::USINGSCREENCOORDS);
	else
		m_RenderState.Disable(HyRenderState::USINGSCREENCOORDS);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
			static_cast<IHyDraw2d *>(m_ChildList[i])->SetCoordinateType(eCoordType, pCameraToCovertFrom);
	}
}

int32 IHyDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

int32 IHyDraw2d::GetDisplayOrderMax() const
{
	return m_iDisplayOrderMax;
}

void IHyDraw2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsDraw2d())
		{
			++iOrderValue;
			static_cast<IHyDraw2d *>(m_ChildList[i])->SetDisplayOrder(iOrderValue);
			iOrderValue = static_cast<IHyDraw2d *>(m_ChildList[i])->GetDisplayOrderMax();
		}
	}

	m_iDisplayOrderMax = iOrderValue;

	HyScene::SetInstOrderingDirty();
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

const HyRectangle<int32> &IHyDraw2d::GetScissor()
{
	return m_LocalScissorRect;
}

void IHyDraw2d::SetScissor(int32 uiX, int32 uiY, uint32 uiWidth, uint32 uiHeight)
{
	m_LocalScissorRect.left = uiX;
	m_LocalScissorRect.bottom = uiY;
	m_LocalScissorRect.right = uiWidth;
	m_LocalScissorRect.top = uiHeight;

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
		SetCoordinateType(IHyApplication::DefaultCoordinateType(), NULL);
	if(GetCoordinateUnit() == HYCOORDUNIT_Default)
		SetCoordinateUnit(IHyApplication::DefaultCoordinateUnit(), false);


	GetData().SetRequiredAtlasIds(m_GfxData);

	sm_pHyAssets->LoadGfxData(m_GfxData);

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
	
	// Remove self from scene (and possibly clean up any unused gfx assets)
	sm_pHyAssets->RemoveInst(this);
	
	// *THEN* clear/reset your load data members
	m_pData = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;
}

void IHyDraw2d::MakeBoundingVolumeDirty()
{
	m_uiAttributes |= ATTRIBFLAG_BoundingVolumeDirty;
}

void IHyDraw2d::SetData(IHyData *pData)
{
	m_pData = pData;
	
	if(m_pData == NULL)
		m_eLoadState = HYLOADSTATE_Loaded;
	else
		m_eLoadState = (m_pData->GetLoadState() == HYLOADSTATE_Loaded) ? HYLOADSTATE_Loaded : HYLOADSTATE_Queued;
}

void IHyDraw2d::SetGfxLoaded()
{
	m_eLoadState = HYLOADSTATE_Loaded;
	OnDataLoaded();
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

		m_RenderState.SetScissorRect(static_cast<int32>(mtx[3].x + m_LocalScissorRect.left),
									 static_cast<int32>(mtx[3].y + m_LocalScissorRect.bottom),
									 static_cast<uint32>(mtx[0].x * m_LocalScissorRect.right),
									 static_cast<uint32>(mtx[1].y * m_LocalScissorRect.top));

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsDraw2d())
				static_cast<IHyDraw2d *>(m_ChildList[i])->m_RenderState.SetScissorRect(m_RenderState.GetScissorRect());
		}
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
	}

	OnUpdate();

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		OnUpdateUniforms();
		m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
	}
}
