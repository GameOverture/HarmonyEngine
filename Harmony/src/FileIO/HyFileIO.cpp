/**************************************************************************
 *	HyFileIO.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/HyFileIO.h"

#include "stdio.h"
#include <fstream>
#include <algorithm>

std::string	HyFileIO::sm_sDataDir = "";
std::string HyFileIO::sm_sLogStr = "";

/*static*/ void HyFileIO::SetDataDir(const char *szPath)
{
	sm_sDataDir = szPath;

	if(sm_sDataDir[sm_sDataDir.length()-1] == '\\')
		sm_sDataDir[sm_sDataDir.length()-1] = '/';
	else if(sm_sDataDir[sm_sDataDir.length()-1] != '/')
		sm_sDataDir.append("/");

	// TODO: Error check whether pData dir and all the sub dirs exists
}

/*static*/ std::string HyFileIO::GetFilePath(HyInstanceType eType, const char *szPrefix, const char *szName)
{
	if(szPrefix == NULL && szName == NULL)
		return std::string();

	std::string sFilePath = sm_sDataDir;
	switch(eType)
	{
	case HYINST_Sound2d:		sFilePath += "audio/";		break;
	case HYINST_Sprite2d:		sFilePath += "sprites/";	break;
	case HYINST_Spine2d:		sFilePath += "spine/";		break;
	case HYINST_Text2d:			sFilePath += "fonts/";		break;
	case HYINST_Shader:			sFilePath += "shaders/";	break;
	case HYINST_Primitive2d:
	case HYINST_Mesh3d:
		break;
	}

	sFilePath.append(szPrefix ? szPrefix : "");
	if(sFilePath[sFilePath.size() - 1] == '\\')
		sFilePath[sFilePath.size() - 1] = '/';
	else if(sFilePath[sFilePath.size() - 1] != '/')
		sFilePath.append("/");

	sFilePath.append(szName ? szName : "");

	switch(eType)
	{
	case HYINST_Sound2d:	sFilePath += ".hyaud";	break;
	case HYINST_Text2d:		sFilePath += ".hyfnt";	break;
	case HYINST_Sprite2d:	sFilePath += ".hyspr";	break;
	}

	std::transform(sFilePath.begin(), sFilePath.end(), sFilePath.begin(), ::tolower);
	return sFilePath;
}

/*static*/ bool HyFileIO::FileExists(const std::string &sFilePath)
{
	return true;

	//struct stat info;
	//uint32 ret = -1;

	//ret = stat(sFilePath.c_str(), &info);
	//return 0 == ret;
}

/*static*/ char *HyFileIO::ReadTextFile(const char *szFilePath, int *iLength)
{
	char *pData;
	FILE *pFile = fopen(szFilePath, "rb");
	if (!pFile) 
		return 0;

	fseek(pFile, 0, SEEK_END);
	*iLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pData = new char[*iLength];
	fread(pData, 1, *iLength, pFile);
	fclose(pFile);

	return pData;
}

/*static*/ std::string HyFileIO::ReadTextFile(const char *szFilePath)
{
	if (szFilePath == NULL)
	{
		sm_sLogStr = "ReadTextFile - filename is NULL\n";
		return std::string();
	}

	std::ifstream infile(szFilePath, std::ios::binary);
	if(!infile)
	{
		sm_sLogStr = "ReadTextFile() - invalid filename\n";
		return std::string();
	}

	// TODO: Make this a lot more safer!
	std::istreambuf_iterator<char> begin(infile), end;

	std::string sReadOutput;
	sReadOutput.append(begin, end);

	return sReadOutput;
}

/*static*/ jsonxx::Object HyFileIO::ReadJsonFile(HyInstanceType eDataDir, const char *szPrefix, const char *szName)
{
	

	//switch(eDataDir)
	//{
	//case DIR_Audio:	sFilePath.append(".";	break;
	//	DIR_Fonts,
	//	DIR_Spine,
	//	DIR_Sprites,
	//	DIR_Shaders,
	//}

	jsonxx::Object o;
	return o;
	//assert(o.parse(teststr));
}
