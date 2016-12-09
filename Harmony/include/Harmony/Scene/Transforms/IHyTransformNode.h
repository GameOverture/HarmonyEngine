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
	friend class HyAnimFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bEnabled;

	IHyTransformNode *				m_pParent;
	std::vector<IHyTransformNode *>	m_ChildList;

	bool							m_bDirty;

	std::vector<HyAnimFloat *>		m_ActiveAnimFloatsList;

public:
	IHyTransformNode(HyType eInstType);
	virtual ~IHyTransformNode();

	HyType GetType();

	bool IsEnabled();
	void SetEnabled(bool bEnabled);

	void AddChild(IHyTransformNode &childInst);
	void Detach();

	void SetDirty();

	void Update();
	virtual void OnTransformUpdate() = 0;

private:
	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);
	void ProcessActiveFloats();
};

#endif /* __IHyTransformNode_h__ */
