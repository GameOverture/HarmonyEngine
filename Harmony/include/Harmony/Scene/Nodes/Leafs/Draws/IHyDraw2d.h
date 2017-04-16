/**************************************************************************
 *	IHyDraw2d.h
 *
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyInst2d_h__
#define __IHyInst2d_h__




class IHyDraw2d : public IHyLeaf2d
{


public:
	IHyDraw2d();
	virtual ~IHyDraw2d(void);
	
protected:
	virtual void NodeUpdate() override final;

	
	//void MakeBoundingVolumeDirty();

	virtual void SetNewChildAttributes(IHyNode2d &childInst);

	

	

	

	virtual void _SetScissor(HyScissor &scissorRef, bool bOverrideExplicitChildren);			// Only Entity2d/3d will invoke this
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren);			// Only Entity2d/3d will invoke this
};

#endif /* __IHyInst2d_h__ */
