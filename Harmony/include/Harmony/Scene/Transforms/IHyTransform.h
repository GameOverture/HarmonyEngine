/**************************************************************************
 *	IHyTransform.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyTransform_h__
#define __IHyTransform_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Transforms/IHyTransformNode.h"

#include "IHyApplication.h"
#include "Scene/Transforms/Tweens/HyTweenVec3.h"

template<typename tVec>
class IHyTransform : public IHyTransformNode
{
public:
	tVec					pos;
	tVec					rot_pivot;
	tVec					scale;
	tVec					scale_pivot;

	IHyTransform(HyType eInstType);
	virtual ~IHyTransform(void);
	
	// Returns the converted pixel position to the specified HyCoordinateType
	virtual void GetLocalTransform(glm::mat4 &outMtx) const = 0;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const = 0;
};

template<typename tVec>
IHyTransform<tVec>::IHyTransform(HyType eInstType) :	IHyTransformNode(eInstType),
														pos(*this),
														rot_pivot(*this),
														scale(*this),
														scale_pivot(*this)
{
	scale.Set(1.0f);
}

template<typename tVec>
/*virtual*/ IHyTransform<tVec>::~IHyTransform(void)
{ }


#endif /* __ITransform_h__ */
