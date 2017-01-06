/*******************************************************************************
 *	IHyInst2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *********************************************************************************/
#include "Scene/Instances/IHyInst2d.h"

#include "IHyApplication.h"

#include "Assets/Data/IHyData.h"
#include "Assets/HyAssetManager.h"
#include "Renderer/Viewport/HyCamera.h"
#include "Renderer/Viewport/HyWindow.h"

/*static*/ HyAssetManager *IHyInst2d::sm_pAssetManager = NULL;

IHyInst2d::IHyInst2d(HyType eInstType, const char *szPrefix, const char *szName) :	IHyTransform2d(eInstType),
																					m_sPREFIX(szPrefix ? szPrefix : ""),
																					m_sNAME(szName ? szName : ""),
																					m_pData(NULL),
																					m_eLoadState(HYLOADSTATE_Inactive),
																					m_bInvalidLoad(false),
																					m_iDisplayOrder(0),
																					topColor(*this),
																					botColor(*this),
																					m_fAlpha(1.0f),
																					alpha(m_fAlpha, *this)
{
#ifdef HY_DEBUG
	if(m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d)
		HyAssert(m_sPREFIX.empty() == false && m_sNAME.empty() == false, "IHyInst2d of type '" << m_eTYPE << "' was constructed with a blank prefix or name");
#endif

	topColor.Set(1.0f);
	botColor.Set(1.0f);
}

/*virtual*/ IHyInst2d::~IHyInst2d(void)
{
	Unload();
}

const std::string &IHyInst2d::GetName()
{
	return m_sNAME;
}

const std::string &IHyInst2d::GetPrefix()
{
	return m_sPREFIX;
}

/*virtual*/ bool IHyInst2d::IsLoaded() const
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

HyCoordinateType IHyInst2d::GetCoordinateType()
{
	return m_eCoordType;
}

void IHyInst2d::SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom)
{
	if(eCoordType == HYCOORDTYPE_Default)
		eCoordType = IHyApplication::DefaultCoordinateType();

	if(pCameraToCovertFrom)
	{
		HyError("IHyInst2d::SetCoordinateType() conversion code needs implementation");
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
}

int32 IHyInst2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void IHyInst2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	HyScene::SetInstOrderingDirty();
}

void IHyInst2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

/*virtual*/ void IHyInst2d::SetScissor(uint32 uiX, uint32 uiY, uint32 uiWidth, uint32 uiHeight)
{
	m_RenderState.SetScissorRect(uiX, uiY, uiWidth, uiHeight);
}

/*virtual*/ void IHyInst2d::ClearScissor()
{
	m_RenderState.ClearScissorRect();
}

int32 IHyInst2d::GetShaderId()
{
	return m_RenderState.GetShaderId();
}

void IHyInst2d::SetCustomShader(IHyShader *pShader)
{
	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "IHyInst2d::SetCustomShader was used on an already loaded instance - I can make this work I just haven't yet");
	HyAssert(pShader->IsFinalized(), "IHyInst2d::SetCustomShader tried to set a non-finalized shader");

	m_RenderState.SetShaderId(pShader->GetId());
}

/*virtual*/ void IHyInst2d::Load()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		return;

	if(GetCoordinateType() == HYCOORDTYPE_Default)
		SetCoordinateType(IHyApplication::DefaultCoordinateType(), NULL);
	if(GetCoordinateUnit() == HYCOORDUNIT_Default)
		SetCoordinateUnit(IHyApplication::DefaultCoordinateUnit(), false);

	if(sm_pAssetManager)
	{
		sm_pAssetManager->LoadInst2d(this);
		m_bInvalidLoad = false;
	}
	else
		m_bInvalidLoad = true;
}

/*virtual*/ void IHyInst2d::Unload()
{
	if(sm_pAssetManager)
		sm_pAssetManager->RemoveInst(this);

	m_pData = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;
}

void IHyInst2d::SetData(IHyData *pData)
{
	m_pData = pData;
	
	if(m_pData == NULL)
		m_eLoadState = HYLOADSTATE_Loaded;
	else
		m_eLoadState = (m_pData->GetLoadState() == HYLOADSTATE_Loaded) ? HYLOADSTATE_Loaded : HYLOADSTATE_Queued;
}

void IHyInst2d::SetLoaded()
{
	m_eLoadState = HYLOADSTATE_Loaded;
	OnDataLoaded();
}

void IHyInst2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

/*virtual*/ void IHyInst2d::OnUpdate()
{
	if(m_eLoadState != HYLOADSTATE_Loaded)
		return;

	OnInstUpdate();

	OnUpdateUniforms();
	m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
}
