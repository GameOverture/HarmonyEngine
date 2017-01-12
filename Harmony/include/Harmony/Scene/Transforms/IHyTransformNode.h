/**************************************************************************
*	IHyTransformNode.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransformNode_h__
#define __IHyTransformNode_h__

#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"

class IHyTransformNode
{
	friend class HyScene;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bDirty;
	bool							m_bIsInst2d;
	bool							m_bEnabled;

	IHyTransformNode *				m_pParent;
	std::vector<IHyTransformNode *>	m_ChildList;


	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	std::vector<HyTweenFloat *>		m_ActiveAnimFloatsList;

public:
	IHyTransformNode(HyType eInstType);
	virtual ~IHyTransformNode();

	HyType GetType();
	bool IsInst2d();

	bool IsEnabled();
	void SetEnabled(bool bEnabled);

	int64 GetTag();
	void SetTag(int64 iTag);

	void AddChild(IHyTransformNode &childInst);
	void Detach();

protected:
	void Update();
	virtual void InstUpdate() { }

	void SetDirty();
	void InsertActiveAnimFloat(HyTweenFloat *pAnimFloat);
};

#endif /* __IHyTransformNode_h__ */
