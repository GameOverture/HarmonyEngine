/**************************************************************************
 *	HyText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/LICENSE
 *************************************************************************/
#ifndef HyText2d_h__
#define HyText2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

class HyText2d : public IHyText<IHyDrawable2d, HyEntity2d>
{
#ifdef HY_USE_TEXT_DEBUG_BOXES
	HyPrimitive2d	m_DebugBox;
#endif

public:
	HyText2d(HyEntity2d *pParent = nullptr);
	HyText2d(const HyNodePath &nodePath, HyEntity2d *pParent = nullptr);
	HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	HyText2d(const HyText2d &copyRef);
	virtual ~HyText2d(void);

	const HyText2d &operator=(const HyText2d &rhs);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;

#ifdef HY_USE_TEXT_DEBUG_BOXES
	virtual void ShowDebugBox(bool bShow) override;
#endif

protected:
#ifdef HY_USE_TEXT_DEBUG_BOXES
	virtual void OnSetDebugBox() override;
#endif
	virtual void OnLoadedUpdate() override;
	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
};
typedef HyText2d HyText;

#endif /* HyText2d_h__ */
