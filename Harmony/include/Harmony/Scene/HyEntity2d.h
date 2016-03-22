/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEntity2d_h__
#define __HyEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/ITransform.h"
#include "Utilities/Animation/HyAnimVec2.h"

#include "Utilities/HyMath.h"

#include <vector>
using std::vector;

// Forward declarations
class IHyInst2d;
class HySprite2d;
class HySpine2d;
class HyText2d;
class HyPrimitive2d;

class HyEntity2d : public ITransform<HyAnimVec2>
{
	friend class HyScene;

	static HyScene *				sm_pCtor;
	HyEntity2d * const				m_kpParent;

	bool							m_bDirty;
	glm::mat4						m_mtxCached;

	vector<IHyInst2d *>			m_vInstList;
	vector<HyEntity2d *>			m_vChildNodes;

	// Hidden ctor used within AddChild()
	HyEntity2d(HyEntity2d *pParent);

public:
	HyEntity2d(void);
	virtual ~HyEntity2d(void);

	void CtorInit();

	IHyInst2d *Set(HyInstanceType eType, const char *szPrefix, const char *szName);
	HySprite2d *SetSprite(const char *szPrefix, const char *szName);
	HySpine2d *SetSpine(const char *szPrefix, const char *szName);
	HyText2d *SetText(const char *szPrefix, const char *szName);
	HyPrimitive2d *SetPrimitive();

	HySprite2d *GetSprite();
	HySpine2d *GetSpine();
	HyText2d *GetText();
	HyPrimitive2d *GetPrimitive();

	bool Remove(IHyInst2d *pInst);
	void Clear(bool bClearChildren);

	HyEntity2d *GetParent()						{ return m_kpParent; }
	HyEntity2d *AddChild();
	void RemoveChild(HyEntity2d *pEnt2d);


	void SetInstOrderingDirty();
	void GetWorldTransform(glm::mat4 &outMtx);

private:
	virtual void Update();

	void LinkInst(IHyInst2d *pInst);

	void Erase(vector<IHyInst2d *>::iterator &iterRef);
	void SetDirty();
	static void OnDirty(void *);
};

#endif /* __HyEntity2d_h__ */
