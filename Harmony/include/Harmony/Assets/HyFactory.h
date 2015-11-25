/**************************************************************************
 *	HyFactory.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyFactory_h__
#define __HyFactory_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyStrManip.h"

#include <vector>
using std::vector;

template<typename tData>
class HyFactory
{
	const HyInstanceType					m_eTYPE;
	const std::string						m_sDATADIR;

	vector<tData *>							m_vData;
	
public:
	HyFactory(HyInstanceType eType, std::string sDataDir) : m_eTYPE(eType),
															m_sDATADIR(sDataDir)
	{ }

	HyInstanceType GetType()						{ return m_eType; }

	tData *GetOrCreateData(const std::string &sPrefix, const std::string &sName)
	{
		std::string sPath = m_sDATADIR;
		sPath += MakeStringProperPath(sPrefix.c_str(), "/");

		switch(m_eTYPE)
		{
		case HYINST_Sound2d:		sPath += MakeStringProperPath(sName.c_str(), ".hyaud");	break;
		case HYINST_Particles2d:	sPath += MakeStringProperPath(sName.c_str(), ".hypfx");	break;
		case HYINST_Text2d:			sPath += MakeStringProperPath(sName.c_str(), ".hyfnt");	break;
		case HYINST_Spine2d:		sPath += MakeStringProperPath(sName.c_str(), ".hyspi");	break;
		case HYINST_Sprite2d:		sPath += MakeStringProperPath(sName.c_str(), ".hyspr");	break;
		}

		// Check to determine this data with these parameters doesn't already exist, if it does return the 'tData' associated with it.
		size_t uiNumDatas = m_vData.size();
		for(size_t i = 0; i < uiNumDatas; ++i)
		{
			if(m_vData[i]->GetPath() == sPath)
				return m_vData[i];
		}
		
		tData *pOutData = new tData(sPath);
		m_vData.push_back(pOutData);

		return pOutData;
	}

	void DeleteData(tData *pData)
	{
		for(vector<tData *>::iterator it = m_vData.begin(); it != m_vData.end(); ++it)
		{
			if((*it) == pData)
			{
				m_vData.erase(it);
				delete pData;

				return;
			}
		}
	}

	bool IsEmpty()
	{
		return m_vData.empty();
	}
};

#endif /* __HyFactory_h__ */
