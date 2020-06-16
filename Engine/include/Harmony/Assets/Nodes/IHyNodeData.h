/**************************************************************************
 *	IHyNodeData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyNodeData_h__
#define IHyNodeData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyAtlas.h"
#include "Assets/Files/HyFilesManifest.h"

class IHyRenderer;
class HyGLTF;
class HyAudioBank;

class IHyNodeData
{
	const std::string				m_sPATH;

protected:
	HyFileIndices					m_RequiredAtlasIndices;
	HyGLTF *						m_pGltf;
	HyAudioBank *					m_pAudioBank;

public:
	IHyNodeData(const std::string &sPath);
	virtual ~IHyNodeData(void);
	
	const std::string &GetPath() const;
	const HyFileIndices &GetRequiredAtlasIndices() const;
	HyGLTF *GetGltf() const;
	HyAudioBank *GetAudioBank() const;

};

#endif /* IHyNodeData_h__ */
