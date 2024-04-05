/**************************************************************************
*	HyTexturedQuadData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTexturedQuadData_h__
#define HyTexturedQuadData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HyTexturedQuadData : public IHyNodeData
{
	HyFileAtlas *				m_pAtlas;
	HyMargins<float>			m_UvCoords;
	uint64						m_uiCropMask;

	HyAuxiliaryFileHandle		m_eAuxiliaryFileHandle;

public:
	HyTexturedQuadData(uint32 uiChecksum, uint32 uiBankId, HyAssets &assetsRef);
	HyTexturedQuadData(HyAuxiliaryFileHandle hHandle, std::string sFilePath, HyTextureInfo textureInfo, HyAssets &assetsRef);
	virtual ~HyTexturedQuadData();

	virtual IHyFile *GetAuxiliaryFile() const override;
	HyFileAtlas *GetAtlas() const;
	const HyMargins<float> &GetUvCoords() const;
	uint64 GetCropMask() const;
};

#endif /* HyTexturedQuad2dData_h__ */
