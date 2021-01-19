/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h"
#include "Assets/Nodes/HyText2dData.h"

HyText2d::HyText2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyText<IHyInstance2d, HyEntity2d>(sPrefix, sName, pParent)
{
}

HyText2d::HyText2d(const HyText2d &copyRef) :
	IHyText<IHyInstance2d, HyEntity2d>(copyRef)
{
}

HyText2d::~HyText2d(void)
{
}

const HyText2d &HyText2d::operator=(const HyText2d &rhs)
{
	IHyText<IHyInstance2d, HyEntity2d>::operator=(rhs);
	return *this;
}

#ifdef HY_USE_TEXT_DEBUG_BOXES
/*virtual*/ void HyText2d::OnSetDebugBox() /*override*/
{
	// SetAsLine or SetAsVertical
	if(m_uiBoxAttributes == 0 || 0 != (m_uiBoxAttributes & BOXATTRIB_IsVertical))
		m_DebugBox.SetAsNothing();
	else if(0 != (m_uiBoxAttributes & BOXATTRIB_IsColumn)) // SetAsColumn
	{
		glm::vec2 ptVerts[4] = { glm::vec2(0.0f, -100.0f), glm::vec2(0.0f, 0.0f), glm::vec2(m_vBoxDimensions.x, 0.0f), glm::vec2(m_vBoxDimensions.x, -100.0f) };
		m_DebugBox.SetAsLineChain(ptVerts, 4);
		m_DebugBox.SetTint(1.0f, 0.0f, 0.0f);
		m_DebugBox.Load();
	}
	else if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox)) // SetAsScaleBox
	{
		m_DebugBox.SetAsBox(m_vBoxDimensions.x, m_vBoxDimensions.y);
		m_DebugBox.SetWireframe(true);
		m_DebugBox.SetTint(1.0f, 0.0f, 0.0f);
		m_DebugBox.Load();
	}
}
#endif

/*virtual*/ void HyText2d::OnLoadedUpdate() /*override*/
{
#ifdef HY_USE_TEXT_DEBUG_BOXES
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	glm::decompose(GetWorldTransform(), vScale, quatRot, ptTranslation, vSkew, vPerspective);

	m_DebugBox.pos.Set(ptTranslation);
	m_DebugBox.rot.Set(rot.Get()); // TODO: This is wrong! Needs World transform's amount of rotation
	m_DebugBox.scale.Set(vScale);
	m_DebugBox.UseWindowCoordinates(GetCoordinateSystem());
	m_DebugBox.SetDisplayOrder(GetDisplayOrder());
	m_DebugBox.SetVisible(IsVisible());
#endif

	CalculateGlyphInfos();
}

/*virtual*/ void HyText2d::OnCalcBoundingVolume() /*override*/
{
	glm::vec2 ptCenter(0.0f, 0.0f);

	if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox))
	{
		ptCenter.x = GetTextBox().x * 0.5f;
		ptCenter.y = GetTextBox().y * 0.5f;
	}

	m_LocalBoundingVolume.SetAsBox(m_fUsedPixelWidth * 0.5f, m_fUsedPixelHeight * 0.5f, ptCenter, rot.Get());
}

/*virtual*/ void HyText2d::OnWriteVertexData(HyVertexBuffer &vertexBufferRef) /*override*/
{
	// CalculateGlyphInfos called here to ensure 'm_uiNumValidCharacters' is up to date with 'm_sCurrentString'
	CalculateGlyphInfos();

	const HyText2dData *pData = static_cast<const HyText2dData *>(UncheckedGetData());

	const uint32 uiNUMLAYERS = pData->GetNumLayers(m_uiState);
	const glm::mat4 &mtxTransformRef = GetWorldTransform();

	uint32 iOffsetIndex = 0;
	for(int32 i = uiNUMLAYERS - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < m_uiNumValidCharacters; ++j, ++iOffsetIndex)
		{
			uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(j, uiNUMLAYERS, i);

			if(m_Utf32CodeList[j] == '\n')
				continue;

			const HyText2dGlyphInfo *pGlyphRef = pData->GetGlyph(m_uiState, i, m_Utf32CodeList[j]);
			if(pGlyphRef == nullptr)
				continue;

			glm::vec2 vSize(pGlyphRef->uiWIDTH, pGlyphRef->uiHEIGHT);
			vSize *= m_fScaleBoxModifier;
			vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

			vertexBufferRef.AppendData2d(&m_pGlyphInfos[uiGlyphOffsetIndex].vOffset, sizeof(glm::vec2));

			vertexBufferRef.AppendData2d(&m_StateColors[m_uiState]->m_LayerColors[i]->topColor.Get(), sizeof(glm::vec3));

			float fAlpha = CalculateAlpha() * m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
			vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

			vertexBufferRef.AppendData2d(&m_StateColors[m_uiState]->m_LayerColors[i]->botColor.Get(), sizeof(glm::vec3));

			vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

			glm::vec2 vUV;

			vUV.x = pGlyphRef->rSRC_RECT.right;//1.0f;
			vUV.y = pGlyphRef->rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.left;//0.0f;
			vUV.y = pGlyphRef->rSRC_RECT.top;//1.0f;
			vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.right;//1.0f;
			vUV.y = pGlyphRef->rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

			vUV.x = pGlyphRef->rSRC_RECT.left;//0.0f;
			vUV.y = pGlyphRef->rSRC_RECT.bottom;//0.0f;
			vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

			vertexBufferRef.AppendData2d(&mtxTransformRef, sizeof(glm::mat4));
		}
	}
}
