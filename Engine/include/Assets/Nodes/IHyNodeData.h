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
#include "Assets/Files/HyFileAtlas.h"
#include "Assets/Files/HyFilesManifest.h"

class IHyRenderer;
class HyGLTF;
class HyFileAudio;

class IHyNodeData
{
	const std::string				m_sPATH;

protected:
	uint32							m_uiNumStates;
	HyFilesManifest					m_RequiredAtlases;
	HyFilesManifest					m_RequiredAudio;
	HyGLTF *						m_pGltf;

public:
	IHyNodeData(const std::string &sPath);
	virtual ~IHyNodeData(void);

	uint32 GetNumStates() const;
	
	const std::string &GetPath() const;
	const HyFilesManifest *GetRequiredFiles(HyFileType eFileType) const;
	HyGLTF *GetGltf() const;
};

#endif /* IHyNodeData_h__ */
