/**************************************************************************
 *	IPolygon2dView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IPolygon2dView_H
#define IPolygon2dView_H

#include "Global.h"
#include "Polygon2dModel.h"

class IPolygon2dView
{
protected:
	Polygon2dModel *			m_pModel;

public:
	IPolygon2dView();
	virtual ~IPolygon2dView();

	Polygon2dModel *GetModel() const;
	void SetModel(Polygon2dModel *pModel);

	virtual void RefreshColor() = 0;
	virtual void RefreshView(bool bTransformPreview) = 0;

	virtual void OnHoverClear() = 0;
};

#endif // IPolygon2dView_H
