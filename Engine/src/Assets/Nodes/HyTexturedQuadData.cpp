/**************************************************************************
*	HyTexturedQuadData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyTexturedQuadData.h"
#include "Renderer/IHyRenderer.h"

HyTexturedQuadData::HyTexturedQuadData(uint32 uiChecksum, uint32 uiBankId, HyAssets &assetsRef) :
	IHyNodeData(true),
	m_pAtlas(nullptr),
	m_eExtrinsicFileHandle(HY_UNUSED_HANDLE)
{
	m_pAtlas = assetsRef.GetAtlas(uiChecksum, uiBankId, m_UvCoords);
}

HyTexturedQuadData::HyTexturedQuadData(HyExtrinsicFileHandle hFileHandle, std::string sFilePath, HyTextureInfo textureInfo, HyAssets &assetsRef) :
	IHyNodeData(true),
	m_pAtlas(HY_NEW HyFileAtlas(hFileHandle, sFilePath, textureInfo)),
	m_UvCoords(0.0f, 1.0f, 1.0f, 0.0f),
	m_eExtrinsicFileHandle(hFileHandle)
{
	assetsRef.SetExtrinsicFile(hFileHandle, m_pAtlas);
}

HyTexturedQuadData::~HyTexturedQuadData()
{
}

/*virtual*/ IHyFile *HyTexturedQuadData::GetExtrinsicFile() const /*override*/
{
	HyAssert(IsExtrinsic(), "HyTexturedQuadData::GetExtrinsicFile() was called on an non-extrinsic object");
	return m_pAtlas;
}

HyFileAtlas *HyTexturedQuadData::GetAtlas() const
{
	return m_pAtlas;
}

const HyRectangle<float> &HyTexturedQuadData::GetUvCoords() const
{
	return m_UvCoords;
}
