/**************************************************************************
*	HyFileIO.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Utilities/HyFileIO.h"

char *HyReadTextFile(const char *szFilePath, int *pLengthOut)
{
	char *pData;
	FILE *pFile = fopen(szFilePath, "r");
	if(!pFile)
		return 0;

	int iLength = 0;

	fseek(pFile, 0, SEEK_END);
	iLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	pData = HY_NEW char[iLength];
	fread(pData, 1, iLength, pFile);
	fclose(pFile);

	if(pLengthOut)
		*pLengthOut = iLength;

	return pData;
}

std::string HyReadTextFile(const char *szFilePath)
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

void WriteTextFile(const char *szFilePath, const char *szContentBuffer)
{
	std::ofstream outfile(szFilePath);
	outfile.write(szContentBuffer, strlen(szContentBuffer));
	outfile.close();
}

bool HyFileExists(const std::string &sFilePath)
{
	// TODO: implement
	return true;
}
