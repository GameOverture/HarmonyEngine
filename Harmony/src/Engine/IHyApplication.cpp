/**************************************************************************
 *	IHyApplication.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "IHyApplication.h"
#include "Renderer/Components/HyWindow.h"
#include "Utilities/HyStrManip.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IHyApplication::IHyApplication(HarmonyInit &initStruct) :
{

}

IHyApplication::~IHyApplication()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowList.size()); ++i)
		delete m_WindowList[i];
}

std::string IHyApplication::GameName() const
{
	return m_Init.sGameName;
}

std::string IHyApplication::DataDir() const
{
	return m_Init.sDataDir;
}

bool IHyApplication::IsShowCursor() const
{
	return m_Init.bShowCursor;
}

HyWindow &IHyApplication::Window(uint32 uiIndex /*= 0*/)
{
	HyAssert(uiIndex < m_Init.uiNumWindows, "IApplication::Viewport() took an invalid index: " << uiIndex);
	return *m_WindowList[uiIndex];
}

uint32 IHyApplication::GetNumWindows()
{
	return m_Init.uiNumWindows;
}

HyInputMap &IHyApplication::Input(uint32 uiIndex /*= 0*/)
{
	HyAssert(m_pInputMaps && uiIndex < m_Init.uiNumInputMappings, "IApplication::Input() took an invalid index: " << uiIndex);
	return static_cast<HyInputMap &>(m_pInputMaps[uiIndex]);
}

void IHyApplication::SetInputMapPtr(HyInputMap *pInputMaps)
{
	m_pInputMaps = pInputMaps;
}
