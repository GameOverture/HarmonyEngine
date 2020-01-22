#pragma once

class VersionPatcher
{
public:
	static void Run(Project *pProj);

private:
	static int GetFileVersion(QString sFilePath, bool bIsMeta);

	static bool Patch_0to1(Project *pProj);
};
