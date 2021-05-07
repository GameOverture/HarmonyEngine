/**************************************************************************
*	AudioDraw.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef AUDIODRAW_H
#define AUDIODRAW_H

#include "IDraw.h"

class AudioDraw : public IDraw
{
public:
	AudioDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);

	virtual void OnShow() override;
	virtual void OnHide() override;
	virtual void OnResizeRenderer() override;
};

#endif // AUDIODRAW_H
