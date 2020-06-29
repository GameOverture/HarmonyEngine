/**************************************************************************
 *	IHyAudioCore.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudioCore_h__
#define IHyAudioCore_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"

class IHyAudioCore
{
public:
	enum CueType
	{
		CUETYPE_PlayOneShotDefault = 0,
		CUETYPE_PlayOneShot,
		CUETYPE_Start,
		CUETYPE_Stop,
		CUETYPE_Pause,
		CUETYPE_Unpause,
		CUETYPE_Attributes,

		NUM_CUETYPES
	};

protected:
	struct Cue
	{
		IHyNode *						m_pNode;
		CueType							m_eCueType;

		Cue(IHyNode *pNode, CueType eCueType) :
			m_pNode(pNode),
			m_eCueType(eCueType)
		{ }
	};
	std::vector<Cue>					m_CueList;

public:
	IHyAudioCore()
	{
	}
	virtual ~IHyAudioCore(void)
	{
	}

	void AppendCue(IHyNode *pNode, CueType eCueType) {
		m_CueList.emplace_back(pNode, eCueType);
	}

	virtual void OnUpdate() = 0;
};

class HyAudioCore_Null : public IHyAudioCore
{
public:
	HyAudioCore_Null() = default;
	virtual ~HyAudioCore_Null() = default;

	virtual void OnUpdate() override
	{ }
};

#endif /* IHyAudioCore_h__ */
