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
	IHyAudio() = default;
	virtual ~IHyAudio(void) = default;

	virtual void OnUpdate() = 0;
};

class HyAudio_Null : public IHyAudio
{
public:
	HyAudio_Null() = default;
	virtual ~HyAudio_Null() = default;

	virtual void OnUpdate() override
	{ }
};

#endif /* IHyAudio_h__ */
