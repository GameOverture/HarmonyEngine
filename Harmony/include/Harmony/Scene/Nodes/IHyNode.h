/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyNode_h__
#define __IHyNode_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenFloat.h"

class IHyNode
{
	friend class HyScene;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	std::vector<HyTweenFloat *>		m_ActiveAnimFloatsList;

	bool							m_bEnabled;
	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyNode(HyType eNodeType);
	virtual ~IHyNode();

	bool IsEnabled();

	int64 GetTag();
	void SetTag(int64 iTag);

protected:
	virtual void InstUpdate() = 0;
	virtual void SetDirty() = 0;

private:
	void InsertActiveAnimFloat(HyTweenFloat *pAnimFloat);
	void Update();	// Only Scene will have access to this
};

#endif /* __IHyNode_h__ */
