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

#include "Scene/IHyEntity2d.h"

#include "Diagnostics/HyGuiComms.h"

/*static*/ HyAssetManager *IHyInst2d::sm_pAssetManager = NULL;

IHyInst2d::IHyInst2d(HyType eInstType, const char *szPrefix, const char *szName) :	IHyTransform2d(eInstType),
																					m_sPREFIX(szPrefix ? szPrefix : ""),
																					m_sNAME(szName ? szName : ""),
																					m_pData(NULL),
																					m_eLoadState(HYLOADSTATE_Inactive),
																					m_iDisplayOrder(0),
																					m_iDisplayOrderMax(0),
																					topColor(*this),
																					botColor(*this),
																					m_fAlpha(1.0f),
																					m_fPrevAlphaValue(1.0f),
																					alpha(m_fAlpha, *this)
{
	m_bIsInst2d = true;

#ifdef HY_DEBUG
	if(m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d)
		HyAssert(m_sNAME.empty() == false, "IHyInst2d of type '" << m_eTYPE << "' was constructed with a blank name");
#endif

	topColor.Set(1.0f);
	botColor.Set(1.0f);
}

/*virtual*/ IHyInst2d::~IHyInst2d(void)
{
	if(m_ChildList.empty() == false)
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsInst2d())
				static_cast<IHyInst2d *>(m_ChildList[i])->Detach();
		}
		HyLogWarning("Deleting IHyInst2d (" << m_sPREFIX << "/" << m_sNAME << ") with '" << m_ChildList.size() << "' children attached. These children are now orphaned");
	}

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

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsInst2d())
			static_cast<IHyInst2d *>(m_ChildList[i])->SetCoordinateType(eCoordType, pCameraToCovertFrom);
	}
}

int32 IHyInst2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

int32 IHyInst2d::GetDisplayOrderMax() const
{
	return m_iDisplayOrderMax;
}

void IHyInst2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsInst2d())
		{
			++iOrderValue;
			static_cast<IHyInst2d *>(m_ChildList[i])->SetDisplayOrder(iOrderValue);
			iOrderValue = static_cast<IHyInst2d *>(m_ChildList[i])->GetDisplayOrderMax();
		}
	}

	m_iDisplayOrderMax = iOrderValue;

	HyScene::SetInstOrderingDirty();
}

void IHyInst2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyInst2d::SetScissor(int32 uiX, int32 uiY, uint32 uiWidth, uint32 uiHeight)
{
	m_LocalScissorRect.left = uiX;
	m_LocalScissorRect.bottom = uiY;
	m_LocalScissorRect.right = uiWidth;
	m_LocalScissorRect.top = uiHeight;

	m_LocalScissorRect.iTag = 1;	// '1' indicates this scissor rect is in use
}

void IHyInst2d::ClearScissor()
{
	m_LocalScissorRect.iTag = 0;	// '0' indicates this scissor rect is disabled
	m_RenderState.ClearScissorRect();

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
			static_cast<IHyInst2d *>(m_ChildList[i])->m_RenderState.ClearScissorRect();
			break;
		case HYTYPE_Entity2d:
			static_cast<IHyEntity2d *>(m_ChildList[i])->m_RenderState.ClearScissorRect();
			break;
		}
	}
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

bool IHyInst2d::IsLoaded() const
{
	if(m_eTYPE != HYTYPE_Entity2d && m_eLoadState != HYLOADSTATE_Loaded)
		return false;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsInst2d() && static_cast<IHyInst2d *>(m_ChildList[i])->IsLoaded() == false)
			return false;
	}

	return true;
}

void IHyInst2d::Load()
{
	HyAssert(sm_pAssetManager, "IHyInst2d::Load was invoked before engine has been initialized");

	if(GetCoordinateType() == HYCOORDTYPE_Default)
		SetCoordinateType(IHyApplication::DefaultCoordinateType(), NULL);
	if(GetCoordinateUnit() == HYCOORDUNIT_Default)
		SetCoordinateUnit(IHyApplication::DefaultCoordinateUnit(), false);

	if(m_eLoadState == HYLOADSTATE_Inactive)
		sm_pAssetManager->LoadInst2d(this);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsInst2d())
			static_cast<IHyInst2d *>(m_ChildList[i])->Load();
	}
}

void IHyInst2d::Unload()
{
	HyAssert(sm_pAssetManager, "IHyInst2d::Unload was invoked before engine has been initialized");

	sm_pAssetManager->RemoveInst(this);

	m_pData = NULL;
	m_eLoadState = HYLOADSTATE_Inactive;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsInst2d())
			static_cast<IHyInst2d *>(m_ChildList[i])->Unload();
	}
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

/*virtual*/ void IHyInst2d::InstUpdate()
{
	if(m_LocalScissorRect.iTag == 1)
	{
		glm::mat4 mtx;
		GetWorldTransform(mtx);

		m_RenderState.SetScissorRect(static_cast<int32>(mtx[3].x + m_LocalScissorRect.left),
									 static_cast<int32>(mtx[3].y + m_LocalScissorRect.bottom),
									 static_cast<uint32>(mtx[0].x * m_LocalScissorRect.right),
									 static_cast<uint32>(mtx[1].y * m_LocalScissorRect.top));

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsInst2d())
				static_cast<IHyInst2d *>(m_ChildList[i])->m_RenderState.SetScissorRect(m_RenderState.GetScissorRect());
		}
	}

	if(m_fPrevAlphaValue != alpha.Get())
	{
		m_fPrevAlphaValue = alpha.Get();

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(m_ChildList[i]->IsInst2d())
				static_cast<IHyInst2d *>(m_ChildList[i])->alpha.Set(alpha.Get());
		}
	}

	OnUpdate();

	if(m_eLoadState == HYLOADSTATE_Loaded)
	{
		OnUpdateUniforms();
		m_RenderState.SetUniformCrc32(m_ShaderUniforms.GetCrc32());
	}
}
