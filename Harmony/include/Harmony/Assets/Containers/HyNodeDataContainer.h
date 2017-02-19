/**************************************************************************
 *	HyNodeDataContainer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyNodeDataContainer_h__
#define __HyNodeDataContainer_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyStrManip.h"
#include "Assets/Containers/HyAtlasContainer.h"

#include <vector>

template<typename tData>
class HyNodeDataContainer
{
	std::map<std::string, uint32>	m_LookupIndexMap;
	std::vector<tData>				m_DataList;
	
public:
	HyNodeDataContainer()
	{ }

	~HyNodeDataContainer()
	{ }

	void Init(jsonxx::Object &subDirObjRef, HyAtlasContainer &atlasContainerRef)
	{
		uint32 i = 0;
		for(auto iter = subDirObjRef.kv_map().begin(); iter != subDirObjRef.kv_map().end(); ++iter, ++i)
		{
			std::string sPath = MakeStringProperPath(iter->first.c_str(), nullptr, true);

			m_LookupIndexMap.insert(std::make_pair(sPath, i));
			m_DataList.emplace_back(iter->first, iter->second, atlasContainerRef);
		}
	}

	tData *GetData(const std::string &sPrefix, const std::string &sName)
	{
		std::string sPath;

		if(sPrefix.empty() == false)
			sPath += MakeStringProperPath(sPrefix.c_str(), "/", true);

		sPath += sName;
		sPath = MakeStringProperPath(sPath.c_str(), nullptr, true);

		auto iter = m_LookupIndexMap.find(sPath);
		HyAssert(iter != m_LookupIndexMap.end(), "Could not find data: " << sPath.c_str());

		return &m_DataList[iter->second];
	}
};

#endif /* __HyNodeDataContainer_h__ */
