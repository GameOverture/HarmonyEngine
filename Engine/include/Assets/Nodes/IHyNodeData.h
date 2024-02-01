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
#include "Assets/Nodes/HyNodePath.h"
#include "Assets/Files/HyFilesManifest.h"

class IHyNodeData
{
protected:
	const HyNodePath				m_PATH;

	uint32							m_uiNumStates;
	HyFilesManifest					m_RequiredFiles[HYNUM_FILETYPES];

public:
	IHyNodeData(const HyNodePath &nodePath);
	virtual ~IHyNodeData(void);

	const HyNodePath &GetPath() const;
	uint32 GetNumStates() const;
	
	const HyFilesManifest *GetManifestFiles(HyFileType eFileType) const;
	virtual IHyFile *GetAuxiliaryFile() const;
};

#endif /* IHyNodeData_h__ */
