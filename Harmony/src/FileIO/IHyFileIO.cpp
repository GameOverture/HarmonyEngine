/**************************************************************************
 *	IHyFileIO.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/IHyFileIO.h"

#include "Scene/Instances/IHyInst2d.h"

#include "FileIO/Data/HySfxData.h"
#include "FileIO/Data/HySpine2dData.h"
#include "FileIO/Data/HySprite2dData.h"
#include "FileIO/Data/HyText2dData.h"
#include "FileIO/Data/HyTexturedQuad2dData.h"

#include "Utilities/jsonxx.h"

#include "stdio.h"
#include <fstream>
#include <algorithm>

IHyFileIO::IHyFileIO(const char *szDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) :	m_GfxCommsRef(gfxCommsRef),
																								m_SceneRef(sceneRef),
																								m_Sfx(HYINST_Sound2d),
																								m_Sprite2d(HYINST_Sprite2d),
																								m_Spine2d(HYINST_Spine2d),
																								m_Txt2d(HYINST_Text2d),
																								m_Mesh3d(HYINST_Mesh3d),
																								m_Quad2d(HYINST_TexturedQuad2d)
{
	m_sDataDir = szDataDirPath;

	std::replace(m_sDataDir.begin(), m_sDataDir.end(), '\\', '/');
	if(m_sDataDir[m_sDataDir.length() - 1] != '/')
		m_sDataDir.append("/");

	
	std::string sAtlasFilePath = m_sDataDir;
	sAtlasFilePath += "Atlas/atlasInfo.json";

	jsonxx::Object atlasObject;
	atlasObject.parse(ReadTextFile(sAtlasFilePath.c_str()));

	uint32 uiNumTextures = static_cast<uint32>(atlasObject.get<jsonxx::Number>("numTextures"));

	jsonxx::Array texturesArray = atlasObject.get<jsonxx::Array>("textures");
	HyAssert(texturesArray.size() == uiNumTextures, "atlasInfo.json reported wrong amount of textures");

	for(uint32 i = 0; i < uiNumTextures; ++i)
	{
		jsonxx::Object texObj = texturesArray.get<jsonxx::Object>(i);
		HyAssert(texObj.get<jsonxx::Number>("id") == i, "atlasInfo.json reported wrong texture Id");

		jsonxx::Array srcFramesArray = texObj.get<jsonxx::Array>("srcFrames");
		uint32 uiNumSrcFrames = srcFramesArray.size();
		for(uint32 j = 0; j < uiNumSrcFrames; ++j)
		{
			jsonxx::Object srcFrameObj = srcFramesArray.get<jsonxx::Object>(j);

			uint32 uiHeight = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("height"));
			uint32 uiWidth = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("width"));
			uint32 uiX = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("x"));
			uint32 uiY = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("y"));
			bool bRotated = srcFrameObj.get<jsonxx::Boolean>("rotated");
		}
	}

	//sm_Atlas.Initialize(sFilePath);


	// Start up Loading thread
	m_LoadingCtrl.m_pLoadQueue_Shared = &m_LoadQueue_Shared;
	m_LoadingCtrl.m_pLoadQueue_Retrieval = &m_LoadQueue_Retrieval;
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);

	IHyInst2d::sm_pCtor = this;
}


IHyFileIO::~IHyFileIO()
{
}


void IHyFileIO::Update()
{
	// Check to see if we have any pending loads to make
	if(m_LoadQueue_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		m_LoadingCtrl.m_csSharedQueue.Lock();
		{
			while(m_LoadQueue_Prepare.empty() == false)
			{
				m_LoadQueue_Shared.push(m_LoadQueue_Prepare.front());
				m_LoadQueue_Prepare.pop();
			}
		}
		m_LoadingCtrl.m_csSharedQueue.Unlock();

		m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
	}

	// Check to see if any loaded data (from the load thread) is ready to go
	m_LoadingCtrl.m_csRetrievalQueue.Lock();
	{
		while(m_LoadQueue_Retrieval.empty() == false)
		{
			IHyData *pData = m_LoadQueue_Retrieval.front();
			m_LoadQueue_Retrieval.pop();

			if(pData->GetType() != HYINST_Sound2d)
				m_GfxCommsRef.Update_SendData(pData);
			else
				OnDataLoaded(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	// Grab and process any returning IData's from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.Update_RetrieveData();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		IHyData *pData = m_pGfxQueue_Retrieval->front();
		m_pGfxQueue_Retrieval->pop();

		if(pData->GetLoadState() == HYLOADSTATE_Queued)
			OnDataLoaded(pData);
		else
			DeleteData(pData);
	}
}

void IHyFileIO::LoadInst2d(IHyInst2d *pInst)
{
	IHyData *pLoadData = NULL;
	switch(pInst->GetInstType())
	{
	case HYINST_Sprite2d:
		pLoadData = m_Sprite2d.GetOrCreateData(pInst->GetPath());
		break;
	case HYINST_Spine2d:
		pLoadData = m_Spine2d.GetOrCreateData(pInst->GetPath());
		break;
	case HYINST_Text2d:
		pLoadData = m_Txt2d.GetOrCreateData(pInst->GetPath());
		break;
	case HYINST_TexturedQuad2d:
		pLoadData = m_Quad2d.GetOrCreateData(pInst->GetPath());
		break;
	}

	pInst->SetData(pLoadData);

	if(pLoadData == NULL || pLoadData->GetLoadState() == HYLOADSTATE_Loaded)
	{
		pInst->SetLoaded();
		if(pLoadData)
			pLoadData->IncRef();

		m_SceneRef.AddInstance(pInst);
	}
	else
	{
		m_vQueuedInst2d.push_back(pInst);

		if(pLoadData->GetLoadState() == HYLOADSTATE_Inactive)
		{
			pLoadData->SetLoadState(HYLOADSTATE_Queued);
			m_LoadQueue_Prepare.push(pLoadData);
		}
	}
}

void IHyFileIO::RemoveInst(IHyInst2d *pInst)
{
	IHyData *pInstData = NULL;

	switch(pInst->GetLoadState())
	{
	case HYLOADSTATE_Loaded:
		m_SceneRef.RemoveInst(pInst);

		pInstData = pInst->GetData();
		if(pInstData && pInstData->DecRef())
			DiscardData(pInstData);
		break;

	case HYLOADSTATE_Queued:
		for(vector<IHyInst2d *>::iterator it = m_vQueuedInst2d.begin(); it != m_vQueuedInst2d.end(); ++it)
		{
			if((*it) == pInst)
			{
				m_vQueuedInst2d.erase(it);
				break;
			}
		}
		break;

	default:
		HyError("IHyFileIO::RemoveInst() passed an invalid HyLoadState");
	}
}

/*static*/ char *IHyFileIO::ReadTextFile(const char *szFilePath, int *iLength)
{
	char *pData;
	FILE *pFile = fopen(szFilePath, "rb");
	if(!pFile)
		return 0;

	fseek(pFile, 0, SEEK_END);
	*iLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pData = new char[*iLength];
	fread(pData, 1, *iLength, pFile);
	fclose(pFile);

	return pData;
}

/*static*/ std::string IHyFileIO::ReadTextFile(const char *szFilePath)
{
	if(szFilePath == NULL)
	{
		//sm_sLogStr = "ReadTextFile - filename is NULL\n";
		return std::string();
	}

	std::ifstream infile(szFilePath, std::ios::binary);
	if(!infile)
	{
		//sm_sLogStr = "ReadTextFile() - invalid filename\n";
		return std::string();
	}

	// TODO: Make this a lot more safer!
	std::istreambuf_iterator<char> begin(infile), end;

	std::string sReadOutput;
	sReadOutput.append(begin, end);

	return sReadOutput;
}

/*static*/ bool IHyFileIO::FileExists(const std::string &sFilePath)
{
	return true;

	//struct stat info;
	//uint32 ret = -1;

	//ret = stat(sFilePath.c_str(), &info);
	//return 0 == ret;
}

void IHyFileIO::OnDataLoaded(IHyData *pData)
{
	bool bDataIsUsed = false;
	for(vector<IHyInst2d *>::iterator iter = m_vQueuedInst2d.begin(); iter != m_vQueuedInst2d.end();)
	{
		if((*iter)->GetData() == pData)
		{
			(*iter)->SetLoaded();
			pData->IncRef();
			m_SceneRef.AddInstance(*iter);

			bDataIsUsed = true;

			iter = m_vQueuedInst2d.erase(iter);
		}
		else
			++iter;
	}

	if(bDataIsUsed)
		pData->SetLoadState(HYLOADSTATE_Loaded);
	else
		DiscardData(pData);
}

void IHyFileIO::DiscardData(IHyData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "IHyFileIO::DiscardData() tried to remove an IData with active references");

	// TODO: Log about erasing data
	pData->SetLoadState(HYLOADSTATE_Discarded);

	if(pData->GetType() != HYINST_Sound2d)
		m_GfxCommsRef.Update_SendData(pData);
	else
		DeleteData(pData);
}

void IHyFileIO::DeleteData(IHyData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "IHyFileIO::DeleteData() tried to delete an IData with active references");

	switch(pData->GetType())
	{
	case HYINST_Sound2d:		m_Sfx.DeleteData(static_cast<HySfxData *>(pData));				break;
	case HYINST_Sprite2d:		m_Sprite2d.DeleteData(static_cast<HySprite2dData *>(pData));	break;
	case HYINST_Spine2d:		m_Spine2d.DeleteData(static_cast<HySpine2dData *>(pData));		break;
	case HYINST_Text2d:			m_Txt2d.DeleteData(static_cast<HyText2dData *>(pData));			break;
	case HYINST_TexturedQuad2d:	m_Quad2d.DeleteData(static_cast<HyTexturedQuad2dData *>(pData)); break;
	}
}

/*static*/ void IHyFileIO::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	vector<IHyData *>	vCurLoadData;

	while(true)
	{
		// Wait idle indefinitely until there is new data to be grabbed
		pLoadingCtrl->m_WaitEvent_HasNewData.Wait();

		// Reset the event so we wait the next time we loop
		pLoadingCtrl->m_WaitEvent_HasNewData.Reset();

		// Copy all the IData ptrs into the 'vCurLoadData' to be processed, while emptying the shared queue
		pLoadingCtrl->m_csSharedQueue.Lock();
		{
			while(pLoadingCtrl->m_pLoadQueue_Shared->empty() == false)
			{
				vCurLoadData.push_back(pLoadingCtrl->m_pLoadQueue_Shared->front());
				pLoadingCtrl->m_pLoadQueue_Shared->pop();
			}
		}
		pLoadingCtrl->m_csSharedQueue.Unlock();

		// Load everything that is enqueued (outside of any critical section)
		for(uint32 i = 0; i < vCurLoadData.size(); ++i)
			vCurLoadData[i]->DoFileLoad();

		// Copy all the (loaded) IData ptrs to the retrieval vector
		pLoadingCtrl->m_csRetrievalQueue.Lock();
		{
			for(uint32 i = 0; i < vCurLoadData.size(); ++i)
				pLoadingCtrl->m_pLoadQueue_Retrieval->push(vCurLoadData[i]);
		}
		pLoadingCtrl->m_csRetrievalQueue.Unlock();

		vCurLoadData.clear();
	}
}
