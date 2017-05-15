#include "Profiler/HyProfileManager.h"
#include "HyEngine.h"
#include <iostream> 
#include <sstream> 

HYProfileManager * HYProfileManager::m_pInstance = NULL;

HYProfileManager * HYProfileManager::GetInstance()
{
	if(!m_pInstance)
		m_pInstance = new HYProfileManager();

	return m_pInstance;
}

HYProfileManager::HYProfileManager()
{
	m_bDisplay = false;
	m_iNumParents = 0;

	//m_pBackground = LtGPrimitiveSetManager::GetInstance()->Create(0);
	//m_pBackground->AddRectangle(250, 250, 800, 300);
	//m_pBackground->SetTint(255, 255, 255);
	//m_pBackground->SetVisible(false);
	//m_pBackground->SetDisplayOrder(100.0f);

	m_pStats = new HyText2d("Lg", "Default");//LtGFontManager::GetInstance()->LoadCreate(0, "Arial", 15, 200, 0);
	m_pStats->Load();
	m_pStats->pos.Set(-425, 230);
	m_pStats->SetEnabled(false);
	m_pStats->TextSetAlignment(HYALIGN_Left);
	m_pStats->SetDisplayOrder(1000);

	
	m_pTitleStats = new HyText2d("Lg", "Default");//LtGFontManager::GetInstance()->LoadCreate(0, "Arial", 15, 200, 0);
	sprintf_s(cTitleBuffer, "Num     Name     Parent    Total    Ms/Frame    Ms/Call    Calls/Frame     \n");
	m_pTitleStats->Load();
	m_pTitleStats->TextSet(cTitleBuffer);
	m_pTitleStats->pos.Set(-425, 270);
	m_pTitleStats->SetDisplayOrder(1000);
	m_pTitleStats->TextSetAlignment(HYALIGN_Left);
//	m_pTitleStats->SetAsScaleBox(700, 200);
	m_pTitleStats->SetEnabled(false);

	pIter = CProfileManager::Get_Iterator();
	
}

HYProfileManager::~HYProfileManager()
{
	//LtGPrimitiveSetManager::GetInstance()->Remove(m_pBackground);
	delete m_pStats;
	delete  m_pTitleStats;
//	LtGFontManager::GetInstance()->Remove(m_pStats);
//	LtGFontManager::GetInstance()->Remove(m_pTitleStats);
}

void HYProfileManager::Update()
{
//	if(Input().IsBtnDownBuffered(HYKEY_P))
//	{
//
//	}

	if(GetAsyncKeyState(HYKEY_P)/*LtGInputManager::GetInstance()->KeyDownBuff('P')*/)
	{
		m_bDisplay = !m_bDisplay;
		m_pStats->SetEnabled(m_bDisplay);
		//m_pBackground->SetVisible(m_bDisplay);
		m_pTitleStats->SetEnabled(m_bDisplay);
	
	}

	if(!m_bDisplay)
		return;

	pIter->First();

	// There is no child
	if(	pIter->Is_Done())
		pIter->Enter_Parent();

	int iEnterState = -1;
	sprintf_s(cBuffer, "");
	
	if(GetAsyncKeyState(HYKEY_0)/*LtGInputManager::GetInstance()->KeyDownBuff('0')*/) iEnterState = 0;
	else if(GetAsyncKeyState(HYKEY_0)) iEnterState = 1;
	else if(GetAsyncKeyState(HYKEY_1)) iEnterState = 2;
	else if(GetAsyncKeyState(HYKEY_2)) iEnterState = 3;
	else if(GetAsyncKeyState(HYKEY_3)) iEnterState = 4;
	else if(GetAsyncKeyState(HYKEY_4)) iEnterState = 5;
	else if(GetAsyncKeyState(HYKEY_5)) iEnterState = 6;
	else if(GetAsyncKeyState(HYKEY_6)) iEnterState = 7;
	else if(GetAsyncKeyState(HYKEY_7)) iEnterState = 8;
	else if(GetAsyncKeyState(HYKEY_8)) iEnterState = 9;

	int iIndex = 0;
	for( ; !pIter->Is_Done(); pIter->Next())
	{
		if(iEnterState == iIndex)
		{
			pIter->Enter_Child(iEnterState);
			break;
		}
	
		//if(m_iNumParents > 8 && CInputManager::GetInstance()->GetBufferedKey(DIK_E))
		//{
		//}

		if(GetAsyncKeyState(HYKEY_B))
		{
				pIter->Enter_Parent();
		}
		
//		float fTest = pIter->Get_Current_Total_Time();
		char iterbuffer[4096];
		sprintf_s(iterbuffer, "%i  -    %s      %s     %.4f     %.6f       %i   \n", iIndex, pIter->Get_Current_Name(), pIter->Get_Current_Parent_Name(), 0.0f,  pIter->Get_Current_Total_Time(), pIter->Get_Current_Total_Calls());
	
		strcat_s(cBuffer, iterbuffer);
		
		iIndex++;
	}

	m_iNumParents = iIndex;

	//((CFontObject *)m_pStats)->UpdateSentence((char *)cBuffer, 250, 260, 1, 1, 1);

	m_pStats->TextSet(cBuffer);

	CProfileManager::Reset();
}

