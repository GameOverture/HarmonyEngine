#pragma once

#include "Scene\Nodes\Leafs\Draws\HyText2d.h"
#include "HyProfileNode.h"
#include <string>

class HYProfileManager
{
	HyText2d *	m_pStats;
	HyText2d *	m_pTitleStats;
	//LtGPrimitiveSet *	m_pBackground;

	std::string mFrameStats;
	char cBuffer[4096];
	CProfileIterator * pIter;

	char cTitleBuffer[256];
	int m_iNumParents;

	bool m_bDisplay;
	
	static HYProfileManager * m_pInstance;

	HYProfileManager();
	~HYProfileManager();

	void TraverseChild(CProfileIterator * pIter);

	public:

		static HYProfileManager * GetInstance();

		void Update();
		
};

