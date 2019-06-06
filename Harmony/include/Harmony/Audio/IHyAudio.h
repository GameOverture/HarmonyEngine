/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudio_h__
#define IHyAudio_h__

class IHyAudio
{
public:
	IHyAudio() { }
	virtual ~IHyAudio(void) { }

	virtual void OnUpdate() = 0;
};

#endif /* IHyAudio_h__ */
