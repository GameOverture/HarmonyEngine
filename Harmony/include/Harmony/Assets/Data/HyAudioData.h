/**************************************************************************
 *	HySfxData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HySfxData_h__
#define __HySfxData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/IHyData.h"
#include "Assets/HyFactory.h"

//#include "SFML/Audio.hpp"

class HySound;

class HySfxData : public IHyData
{
	friend class HyFactory<HySfxData>;

	//sf::SoundBuffer			m_SoundBuffer;

	// Only allow HyFactory instantiate
	HySfxData(const std::string &sPath);

public:
	virtual ~HySfxData(void);

	virtual void DoFileLoad() override;
};

#endif /* __HySfxData_h__ */
