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
#include "Assets/Files/HyFileIO.h"

//void HyFindFilesRecursively(const char *szDirPath, const std::string &sExtension, std::vector<std::string> &filesFoundOut)
//{
//	transform(sExtension.begin(), sExtension.end(), sExtension.begin(), ::tolower);
//
//	QFileInfoList list = dirEntry.entryInfoList();
//	QStack<QFileInfoList> dirStack;
//	dirStack.push(list);
//
//	while(dirStack.isEmpty() == false)
//	{
//		list = dirStack.pop();
//		for(int i = 0; i < list.count(); i++)
//		{
//			QFileInfo info = list[i];
//
//			if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
//			{
//				QDir subDir(info.filePath());
//				QFileInfoList subList = subDir.entryInfoList();
//
//				dirStack.push(subList);
//			}
//			else if(info.suffix().toLower() == sExt)
//			{
//				appendList.push_back(info.filePath());
//			}
//		}
//	}
//}

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

void HyReadTextFile(const char *szFilePath, std::string &sContentsOut)
{
	sContentsOut.clear();

	if(szFilePath == NULL)
	{
		//HyLogError("ReadTextFile - filename is NULL");
		return;
	}

	std::ifstream infile(szFilePath, std::ios::binary);
	HyAssert(infile, "HyReadTextFile invalid file: " << szFilePath);

	// TODO: Make this a lot more safer!
	std::istreambuf_iterator<char> begin(infile), end;
	sContentsOut.append(begin, end);
	infile.close();
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
