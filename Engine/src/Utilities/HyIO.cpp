/**************************************************************************
*	HyFileIO.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyIO.h"
#include "Diagnostics/Console/HyConsole.h"

#include "soil2/SOIL2.h"

/*static*/ void HyIO::ReadTextFile(const char *szFilePath, std::string &sContentsOut)
{
	//SDL_RWops *pBankFile = SDL_RWFromFile(sFilePath.c_str(), "rb");
	//if(pBankFile == nullptr)
	//	return false;

	//Sint64 iBankSize = SDL_RWsize(pBankFile);
	//m_pBankData = HY_NEW unsigned char[iBankSize + 1];

	//Sint64 iTotalReadObjs = 0, iCurReadObjs = 1;
	//unsigned char *pFilePtr = m_pBankData;
	//while(iTotalReadObjs < iBankSize && iCurReadObjs != 0)
	//{
	//	iCurReadObjs = SDL_RWread(pBankFile, pFilePtr, 1, (iBankSize - iTotalReadObjs));
	//	iTotalReadObjs += iCurReadObjs;
	//	pFilePtr += iCurReadObjs;
	//}
	//SDL_RWclose(pBankFile);

	//if(iTotalReadObjs != iBankSize)
	//{
	//	HyLogError("Failed to read " << sFilePath);
	//	delete[] m_pBankData;
	//	return false;
	//}

	//m_pBankData[iTotalReadObjs] = '\0';
	//return true;

	sContentsOut.clear();
	if(szFilePath == nullptr)
		return;

	std::ifstream infile(szFilePath, std::ios::binary);
	HyAssert(infile, "HyReadTextFile invalid file: " << szFilePath);

	// TODO: Make this a lot more safer!
	std::istreambuf_iterator<char> begin(infile), end;
	sContentsOut.append(begin, end);
	infile.close();
}

/*static*/ void HyIO::WriteTextFile(const char *szFilePath, const char *szContentBuffer)
{
	std::ofstream outfile(szFilePath);
	outfile.write(szContentBuffer, strlen(szContentBuffer));
	outfile.close();
}

/*static*/ bool HyIO::FileExists(const std::string &sFilePath)
{
	std::ifstream infile(sFilePath.c_str());
	return infile.good();
}

/*static*/ std::string HyIO::CleanPath(const char *szPath, const char *szExtension, bool bMakeLowercase)
{
	std::string sPath(szPath ? szPath : "");
	std::replace(sPath.begin(), sPath.end(), '\\', '/');

	if(szExtension)
	{
		std::string sExtension(szExtension);
		//if(sExtension[0] != '.')
		//	sExtension = "." + sExtension;	// <-- Don't prepend '.' to szExtension, because GUI tool ItemDir's use just '/' as an extension

		if(sPath.empty() || 0 != strcmp(&sPath[sPath.length() - sExtension.size()], sExtension.c_str()))
			sPath += sExtension;
	}

	// Get rid of any double slashes
	size_t uiIndex = 0;
	while(true)
	{
		uiIndex = sPath.find("//", uiIndex);
		if(uiIndex == std::string::npos)
			break;

		sPath.replace(uiIndex, 2, "/");
	}

	// Resolve any "."
	// If path begins with "./", get rid of it
	if(0 == sPath.compare(0, 2, "./"))
		sPath.replace(0, 2, "");

	// Get rid of any other "/./"
	while(true)
	{
		uiIndex = 0;
		uiIndex = sPath.find("/./", uiIndex);
		if(uiIndex == std::string::npos)
			break;

		sPath.replace(uiIndex, 3, "/");
	}

	// Resolve any ".." (as far as possible)
	while(true)
	{
		uiIndex = 0;
		uiIndex = sPath.find("/..", uiIndex);
		if(uiIndex == std::string::npos || uiIndex == 0)
			break;

		size_t uiStartIndex = sPath.rfind("/", uiIndex-1);
		if(uiStartIndex == std::string::npos)
			break;

		sPath.erase(uiStartIndex, (uiIndex+3) - uiStartIndex);
	}


	if(bMakeLowercase)
		transform(sPath.begin(), sPath.end(), sPath.begin(), ::tolower);

	return sPath;
}

/*static*/ bool HyIO::SaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 4, pUncompressedPixelData, 0);
}

/*static*/ bool HyIO::SaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 3, pUncompressedPixelData, 0);
}
