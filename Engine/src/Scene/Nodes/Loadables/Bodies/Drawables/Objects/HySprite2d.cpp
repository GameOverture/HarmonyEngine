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
#include "Scene/Physics/Fixtures/HyShape2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Assets/Files/HyFileAtlas.h"

HySprite2d::HySprite2d(HyEntity2d *pParent /*= nullptr*/) :
	IHySprite<IHyDrawable2d, HyEntity2d>(HyNodePath(), pParent)
{
}

HySprite2d::HySprite2d(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHySprite<IHyDrawable2d, HyEntity2d>(nodePath, pParent)
{
}

HySprite2d::HySprite2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :
	IHySprite<IHyDrawable2d, HyEntity2d>(HyNodePath(szPrefix, szName), pParent)
{
}

HySprite2d::HySprite2d(const HySprite2d &copyRef) :
	IHySprite<IHyDrawable2d, HyEntity2d>(copyRef)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(HySprite2d::NullAnimCallback);
}

HySprite2d::~HySprite2d(void)
{
}

const HySprite2d &HySprite2d::operator=(const HySprite2d &rhs)
{
	IHySprite<IHyDrawable2d, HyEntity2d>::operator=(rhs);

	m_AnimCallbackList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AnimCallbackList.size()); ++i)
		m_AnimCallbackList.push_back(HySprite2d::NullAnimCallback);

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

	const HySpriteFrame *pFrameRef = static_cast<const HySpriteData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);
	glm::vec2 ptBoxCenter(pFrameRef->vOFFSET.x + fHalfWidth, pFrameRef->vOFFSET.y + fHalfHeight);

	if((m_AnimCtrlAttribList[m_uiState] & ANIMCTRLATTRIB_BoundsIncludeAlphaCrop) != 0)
	{
		glm::vec2 ptBotLeft = ptBoxCenter - glm::vec2(fHalfWidth, fHalfHeight);
		glm::vec2 ptTopRight = ptBoxCenter + glm::vec2(fHalfWidth, fHalfHeight);

		ptBotLeft.x -= pFrameRef->rCROP_MARGINS.left;
		ptBotLeft.y -= pFrameRef->rCROP_MARGINS.bottom;
		ptTopRight.x += pFrameRef->rCROP_MARGINS.right;
		ptTopRight.y += pFrameRef->rCROP_MARGINS.top;

		fHalfWidth = (ptTopRight.x - ptBotLeft.x) * 0.5f;
		fHalfHeight = (ptTopRight.y - ptBotLeft.y) * 0.5f;
		ptBoxCenter = ptBotLeft + glm::vec2(fHalfWidth, fHalfHeight);
	}

	if(fHalfWidth <= HyMath::FloatSlop || fHalfHeight <= HyMath::FloatSlop)
		return;

	shapeOut.SetAsBox(HyRect(fHalfWidth, fHalfHeight, ptBoxCenter, 0.0f));
}

void HySprite2d::SetAnimCallback(uint32 uiStateIndex, std::function<void(HySprite2d *)> fpAnimFinishedCallBack /*= HySprite2d::NullAnimCallback*/)
{
	if(AcquireData() == nullptr || uiStateIndex >= static_cast<const HySpriteData *>(UncheckedGetData())->GetNumStates())
	{
		if(UncheckedGetData() == nullptr)
			HyLogDebug("HySprite2d::AnimSetCallback invoked on null data");
		else
			HyLogWarning(this->m_NodePath.GetPath() << " (HySprite) wants to set anim callback on index of '" << uiStateIndex << "' when total number of states is '" << static_cast<const HySpriteData *>(AcquireData())->GetNumStates() << "'");

		return;
	}

	if(fpAnimFinishedCallBack == nullptr)
		m_AnimCallbackList[uiStateIndex] = NullAnimCallback;
	else
		m_AnimCallbackList[uiStateIndex] = fpAnimFinishedCallBack;
}

/*virtual*/ void HySprite2d::OnInvokeCallback(uint32 uiStateIndex) /*override*/
{
	m_AnimCallbackList[uiStateIndex](this);
}

/*virtual*/ void HySprite2d::OnDataAcquired() /*override*/
{
	IHySprite<IHyDrawable2d, HyEntity2d>::OnDataAcquired();

	const HySpriteData *pData = static_cast<const HySpriteData *>(UncheckedGetData());
	uint32 uiNumStates = pData->GetNumStates();

	while(m_AnimCallbackList.size() < uiNumStates)
		m_AnimCallbackList.push_back(NullAnimCallback);
}

/*virtual*/ void HySprite2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_TriangleStrip;
	eBlendModeOut = HYBLENDMODE_Normal;
	uiNumInstancesOut = 1;
	uiNumVerticesPerInstOut = 4;
	bIsBatchable = true;
}

/*virtual*/ bool HySprite2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	const HySpriteFrame *pFrameRef = static_cast<const HySpriteData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);

	glm::vec2 vSize(pFrameRef->rSRC_RECT.Width() * pFrameRef->pAtlas->GetWidth(), pFrameRef->rSRC_RECT.Height() * pFrameRef->pAtlas->GetHeight());
	vertexBufferRef.AppendVertexData(&vSize, sizeof(glm::vec2));

	glm::vec2 vOffset(pFrameRef->vOFFSET.x, pFrameRef->vOFFSET.y);
	vertexBufferRef.AppendVertexData(&vOffset, sizeof(glm::vec2));

	vertexBufferRef.AppendVertexData(&CalculateTopTint(fExtrapolatePercent), sizeof(glm::vec3));

	float fAlpha = CalculateAlpha(fExtrapolatePercent);
	vertexBufferRef.AppendVertexData(&fAlpha, sizeof(float));

	vertexBufferRef.AppendVertexData(&CalculateBotTint(fExtrapolatePercent), sizeof(glm::vec3));

	vertexBufferRef.AppendVertexData(&fAlpha, sizeof(float));

	glm::vec2 vUV;

	vUV.x = pFrameRef->rSRC_RECT.right;//1.0f;
	vUV.y = pFrameRef->rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

	vUV.x = pFrameRef->rSRC_RECT.left;//0.0f;
	vUV.y = pFrameRef->rSRC_RECT.top;//1.0f;
	vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

	vUV.x = pFrameRef->rSRC_RECT.right;//1.0f;
	vUV.y = pFrameRef->rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

	vUV.x = pFrameRef->rSRC_RECT.left;//0.0f;
	vUV.y = pFrameRef->rSRC_RECT.bottom;//0.0f;
	vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));

	vertexBufferRef.AppendVertexData(&GetSceneTransform(fExtrapolatePercent), sizeof(glm::mat4));

	return true;
}
