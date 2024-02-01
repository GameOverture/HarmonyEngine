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
#include "Assets/Nodes/Objects/HyTexturedQuadData.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Renderer/IHyRenderer.h"

HyTexturedQuadData::HyTexturedQuadData(uint32 uiChecksum, uint32 uiBankId, HyAssets &assetsRef) :
	IHyNodeData(HyNodePath(uiChecksum, uiBankId)),
	m_pAtlas(nullptr),
	m_eAuxiliaryFileHandle(HY_UNUSED_HANDLE)
{
	m_pAtlas = assetsRef.GetAtlas(uiChecksum, uiBankId, m_UvCoords, m_uiCropMask);
}

HyTexturedQuadData::HyTexturedQuadData(HyAuxiliaryFileHandle hFileHandle, std::string sFilePath, HyTextureInfo textureInfo, HyAssets &assetsRef) :
	IHyNodeData(HyNodePath(0, hFileHandle)),
	m_pAtlas(HY_NEW HyFileAtlas(hFileHandle, sFilePath, textureInfo)),
	m_UvCoords(0.0f, 1.0f, 1.0f, 0.0f),
	m_uiCropMask(0),
	m_eAuxiliaryFileHandle(hFileHandle)
{
	assetsRef.SetAuxiliaryFile(hFileHandle, m_pAtlas);
}

HyTexturedQuadData::~HyTexturedQuadData()
{
}

/*virtual*/ IHyFile *HyTexturedQuadData::GetAuxiliaryFile() const /*override*/
{
	HyAssert(m_PATH.IsAuxiliary(), "HyTexturedQuadData::GetAuxiliaryFile() was called on an non-auxiliary object");
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

uint64 HyTexturedQuadData::GetCropMask() const
{
	return m_uiCropMask;
}
