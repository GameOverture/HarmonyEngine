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
																					m_iTag(0)
{
	topColor.Set(1.0f);
	botColor.Set(1.0f);
}

/*virtual*/ IHyInst2d::~IHyInst2d(void)
{
	Unload();
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

void IHyInst2d::Load()
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

void IHyInst2d::Unload()
{
	if(sm_pAssetManager)
		sm_pAssetManager->RemoveInst(this);

	m_pData = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;
}

/*virtual*/ void IHyInst2d::OnUpdate()
{
	OnInstUpdate();
	OnUpdateUniforms(m_RenderState.PrimeShaderUniforms());
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

void IHyInst2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	HyScene::SetInstOrderingDirty();
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
