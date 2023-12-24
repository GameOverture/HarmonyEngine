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
	HyFilesManifest					m_RequiredFiles[HYNUM_FILETYPES];

public:
	IHyNodeData(bool bIsAuxiliary); // For auxiliary nodes // TODO: remove 'bIsAuxiliary' (just using it to test against compiler errors)
	IHyNodeData(const std::string &sPath);
	virtual ~IHyNodeData(void);

	bool IsAuxiliary() const;

	uint32 GetNumStates() const;
	
	const std::string &GetPath() const;
	const HyFilesManifest *GetManifestFiles(HyFileType eFileType) const;
	virtual IHyFile *GetAuxiliaryFile() const;
};

#endif /* IHyNodeData_h__ */
