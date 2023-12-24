#pragma once

class VersionPatcher
{
public:
	static bool Run(Project *pProj);

private:
	static int GetFileVersion(QString sFilePath, QJsonDocument &fileDocOut, bool bIsMeta);

	static void Patch_0to1(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_1to2(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_2to3(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_3to4(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_4to5(QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef);
	static void Patch_5to6(QJsonDocument &projDocRef);
	static void Patch_6to7(Project *pProj, QJsonDocument &projDocRef);
	static void Patch_7to8(QJsonDocument &projDocRef);
	static void Patch_8to9(QJsonDocument &metaAtlasDocRef, QJsonDocument &dataAtlasDocRef); // Changing to use "textureInfo"
	static void Patch_9to10(QJsonDocument &metaAudioDocRef, QJsonDocument &dataAudioDocRef);
	static void Patch_10to11(QJsonDocument &metaSourceDocRef);
	static void Patch_11to12(QJsonDocument &metaAtlasDocRef, QJsonDocument &metaAudioDocRef);
	static void Patch_12to13(const QJsonDocument &metaItemsDocRef, QJsonDocument &dataItemsDocRef, const QJsonDocument &metaAtlasDocRef); // Adding 'bankId' wherever only a checksum was used
	static void Patch_13to14(QJsonDocument &metaAtlasDocRef, const QJsonDocument &dataAtlasDocRef); // Adding 'textureSizes' array to each meta atlas bank. Also adding 'cropUnusedSpace', 'squareTexturesOnly', 'aggressiveResizing', and 'minimumFillRate' packing settings
	static void Patch_14to15(QJsonDocument &dataItemsDocRef, const QJsonDocument &metaItemsDocRef, QJsonDocument &metaAudioDocRef, QJsonDocument &dataAudioDocRef); // Adding 'bankId' wherever only an audio checksum was used; Renaming 'group' -> 'category'; Renaming "playList" -> "playlist"

	static void RewriteFile(QString sFilePath, QJsonDocument &fileDocRef, bool bIsMeta);
};
