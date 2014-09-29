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

#include <vector>
using std::vector;

template<typename tData>
class HyFactory
{
	HyInstanceType							m_eType;
	vector<tData *>							m_vData;
	
public:
	HyFactory(HyInstanceType eType)
	{
		m_eType = eType;
	}

	HyInstanceType GetType()						{ return m_eType; }

	tData *GetOrCreateData(const std::string &sPath)
	{
		// Check to determine this data with these parameters doesn't already exist, if it does return the 'tData' associated with it.
		uint32 uiNumDatas = m_vData.size();
		for(uint32 i = 0; i < uiNumDatas; ++i)
		{
			if(m_vData[i]->IsMatch(sPath))
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
};

#endif /* __HyFactory_h__ */
