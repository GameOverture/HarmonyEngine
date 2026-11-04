/**************************************************************************
*	AudioDraw.cpp
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2020 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#include "Global.h"
#include "AudioDraw.h"

AudioDraw::AudioDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
}

/*virtual*/ void AudioDraw::OnResizeRenderer() /*override*/
{
}
