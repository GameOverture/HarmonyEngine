/**************************************************************************
*	AudioDraw.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2020 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef AUDIODRAW_H
#define AUDIODRAW_H

#include "IDraw.h"

class AudioDraw : public IDraw
{
public:
	AudioDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef);

	virtual void OnResizeRenderer() override;
};

#endif // AUDIODRAW_H
