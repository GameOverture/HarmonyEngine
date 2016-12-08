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
	const HyType					m_eTYPE;
	const std::string				m_sDATADIR;

	vector<tData *>					m_DataList;
	
public:
	HyFactory(HyType eType, std::string sDataDir) :	m_eTYPE(eType),
													m_sDATADIR(sDataDir)
	{ }
	~HyFactory()
	{
		for(uint32 i = 0; i < static_cast<uint32>(m_DataList.size()); ++i)
			delete m_DataList[i];
	}

	HyType GetType()				{ return m_eType; }

	tData *GetOrCreateData2d(const std::string &sPrefix, const std::string &sName, int32 iShaderId)
	{
		std::string sPath = m_sDATADIR;

		if(sPrefix.empty() == false)
			sPath += MakeStringProperPath(sPrefix.c_str(), "/", true);

		sPath += sName;

		switch(m_eTYPE)
		{
		case HYTYPE_Sound2d:		sPath = MakeStringProperPath(sPath.c_str(), ".hyaud", true);	break;
		case HYTYPE_Particles2d:	sPath = MakeStringProperPath(sPath.c_str(), ".hypfx", true);	break;
		case HYTYPE_Text2d:			sPath = MakeStringProperPath(sPath.c_str(), ".hyfnt", true);	break;
		case HYTYPE_Spine2d:		sPath = MakeStringProperPath(sPath.c_str(), ".hyspi", true);	break;
		case HYTYPE_Sprite2d:		sPath = MakeStringProperPath(sPath.c_str(), ".hyspr", true);	break;
		case HYTYPE_TexturedQuad2d:	sPath = sName;													break;
		case HYTYPE_Primitive2d:	sPath = "";														break;
		}

		// Check to determine this data with these parameters doesn't already exist, if it does return the 'tData' associated with it.
		size_t uiNumDatas = m_DataList.size();
		for(size_t i = 0; i < uiNumDatas; ++i)
		{
			if(m_DataList[i]->GetPath() == sPath && m_DataList[i]->GetShaderId() == iShaderId)
				return m_DataList[i];
		}
		
		tData *pOutData = HY_NEW tData(sPath, iShaderId);
		m_DataList.push_back(pOutData);

		return pOutData;
	}

	void DeleteData(tData *pData)
	{
		for(vector<tData *>::iterator it = m_DataList.begin(); it != m_DataList.end(); ++it)
		{
			if((*it) == pData)
			{
				m_DataList.erase(it);
				delete pData;

				return;
			}
		}
	}

	bool IsEmpty()
	{
		return m_DataList.empty();
	}
};

#endif /* __HyFactory_h__ */
