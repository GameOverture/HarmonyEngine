/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Assets/Nodes/HySpriteData.h"

HySprite2d::HySprite2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHySprite<IHyDrawable2d, HyEntity2d>(sPrefix, sName, pParent)
{
}

HySprite2d::HySprite2d(const HySprite2d &copyRef) :
	IHySprite<IHyDrawable2d, HyEntity2d>(copyRef)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(copyRef.m_AnimCallbackList[i].first, copyRef.m_AnimCallbackList[i].second));
}

HySprite2d::~HySprite2d(void)
{
}

const HySprite2d &HySprite2d::operator=(const HySprite2d &rhs)
{
	IHySprite<IHyDrawable2d, HyEntity2d>::operator=(rhs);

	m_AnimCallbackList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(rhs.m_AnimCallbackList[i].first, rhs.m_AnimCallbackList[i].second));

	return *this;
}

/*virtual*/ void HySprite2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	if(AcquireData() == nullptr)
	{
		HyLogDebug("HySprite2d::CalcLocalBoundingShape invoked on null data");
		return;
	}

	float fHalfWidth = GetFrameWidth(0.5f);
	float fHalfHeight = GetFrameHeight(0.5f);
	if(fHalfWidth <= HyShape2d::FloatSlop || fHalfHeight <= HyShape2d::FloatSlop)
		return;

	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);
	glm::vec2 ptBoxCenter(frameRef.vOFFSET.x + fHalfWidth, frameRef.vOFFSET.y + fHalfHeight);

	if((m_AnimCtrlAttribList[m_uiState] & ANIMCTRLATTRIB_BoundsIncludeAlphaCrop) != 0)
	{
		glm::vec2 ptBotLeft = ptBoxCenter - glm::vec2(fHalfWidth, fHalfHeight);
		glm::vec2 ptTopRight = ptBoxCenter + glm::vec2(fHalfWidth, fHalfHeight);

		ptBotLeft.x -= frameRef.rCROP_MARGINS.left;
		ptBotLeft.y -= frameRef.rCROP_MARGINS.bottom;
		ptTopRight.x += frameRef.rCROP_MARGINS.right;
		ptTopRight.y += frameRef.rCROP_MARGINS.top;

		fHalfWidth = (ptTopRight.x - ptBotLeft.x) * 0.5f;
		fHalfHeight = (ptTopRight.y - ptBotLeft.y) * 0.5f;
		ptBoxCenter = ptBotLeft + glm::vec2(fHalfWidth, fHalfHeight);
	}

	shapeOut.SetAsBox(fHalfWidth, fHalfHeight, ptBoxCenter, 0.0f);
}

void HySprite2d::SetAnimCallback(uint32 uiStateIndex, HySprite2dAnimFinishedCallback callBack /*= HySprite2d::NullAnimCallback*/, void *pParam /*= nullptr*/)
{
	if(AcquireData() == nullptr || uiStateIndex >= static_cast<const HySpriteData *>(UncheckedGetData())->GetNumStates())
	{
		if(UncheckedGetData() == nullptr)
			HyLogDebug("HySprite2d::AnimSetCallback invoked on null data");
		else
			HyLogWarning(this->m_sPrefix << "/" << this->m_sName << " (HySprite) wants to set anim callback on index of '" << uiStateIndex << "' when total number of states is '" << static_cast<const HySpriteData *>(AcquireData())->GetNumStates() << "'");

		return;
	}

	if(callBack == nullptr)
		m_AnimCallbackList[uiStateIndex].first = NullAnimCallback;
	else
		m_AnimCallbackList[uiStateIndex].first = callBack;

	m_AnimCallbackList[uiStateIndex].second = pParam;
}

/*virtual*/ void HySprite2d::OnInvokeCallback(uint32 uiStateIndex) /*override*/
{
	m_AnimCallbackList[uiStateIndex].first(this, m_AnimCallbackList[uiStateIndex].second);
}

/*virtual*/ void HySprite2d::OnDataAcquired() /*override*/
{
	IHySprite<IHyDrawable2d, HyEntity2d>::OnDataAcquired();

	const HySpriteData *pData = static_cast<const HySpriteData *>(UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCallbackList.size() < uiNumStates)
		m_AnimCallbackList.push_back(std::pair<HySprite2dAnimFinishedCallback, void *>(NullAnimCallback, nullptr));
}

/*virtual*/ void HySprite2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_TriangleStrip;
	uiNumInstancesOut = 1;
	uiNumVerticesPerInstOut = 4;
	bIsBatchable = true;
}

/*virtual*/ bool HySprite2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	const HySpriteFrame &frameRef = static_cast<const HySpriteData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);

	glm::vec2 vSize(frameRef.rSRC_RECT.Width() * frameRef.pAtlas->GetWidth(), frameRef.rSRC_RECT.Height() * frameRef.pAtlas->GetHeight());
	vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

	glm::vec2 vOffset(frameRef.vOFFSET.x, frameRef.vOFFSET.y);
	vertexBufferRef.AppendData2d(&vOffset, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&CalculateTopTint(fExtrapolatePercent), sizeof(glm::vec3));

	float fAlpha = CalculateAlpha(fExtrapolatePercent);
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	vertexBufferRef.AppendData2d(&CalculateBotTint(fExtrapolatePercent), sizeof(glm::vec3));

	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	glm::vec2 vUV;

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.right;//1.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = frameRef.rSRC_RECT.left;//0.0f;
	vUV.y = frameRef.rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&GetSceneTransform(fExtrapolatePercent), sizeof(glm::mat4));

	return true;
}
