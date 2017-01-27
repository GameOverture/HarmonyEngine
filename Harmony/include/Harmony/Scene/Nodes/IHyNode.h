/**************************************************************************
*	IHyNode.h
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

class IHyNode
{
	friend class HyScene;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bDirty;
	bool							m_bIsDraw2d;
	bool							m_bEnabled;

	IHyNode *						m_pParent;
	std::vector<IHyNode *>			m_ChildList;


	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	std::vector<HyTweenFloat *>		m_ActiveAnimFloatsList;

public:
	IHyNode(HyType eInstType);
	virtual ~IHyNode();
	
	HyType GetType();
	bool IsDraw2d();

	bool IsEnabled();
	void SetEnabled(bool bEnabled);

	int64 GetTag();
	void SetTag(int64 iTag);

	void AddChild(IHyNode &childInst);
	void Detach();

protected:
	void Update();
	virtual void InstUpdate() = 0;

	void SetDirty();
	void InsertActiveAnimFloat(HyTweenFloat *pAnimFloat);
};

#endif /* __IHyTransformNode_h__ */
