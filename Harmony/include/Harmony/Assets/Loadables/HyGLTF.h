/**************************************************************************
 *	HyGLTF.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyGLTF_h__
#define HyGLTF_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/IHyLoadableData.h"

class HyGLTF : public IHyLoadableData
{
	const std::string		m_sIDENTIFIER;
	tinygltf::Model			m_ModelData;


public:
	HyGLTF(const std::string &sIdentifier);
	~HyGLTF();

	const std::string &GetIdentifier();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyAtlas_h__ */
