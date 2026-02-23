/**************************************************************************
*	HyTranslateTable.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Localization/HyTranslateTable.h"
#include "HyEngine.h"

/*static*/ std::unordered_map<std::string, std::string> HyTranslateTable::sm_table;
/*static*/ std::string HyTranslateTable::sm_language("default");
/*static*/ bool HyTranslateTable::sm_testMode(false);

/*static*/ void HyTranslateTable::Load(std::string sIso639Code, std::string sIso3166Code)
{
	sm_language = sIso639Code;
	if (!sIso3166Code.empty())
	{
		sm_language += "_" + sIso3166Code;
	}

	std::string sFilePath = HyEngine::DataDir();
	sFilePath += HYASSETS_TranslationsFile;

	std::vector<char> sFileContents;
	HyIO::ReadTextFile(sFilePath.c_str(), sFileContents);

	HyJsonDoc itemsDoc;
	
	if (itemsDoc.ParseInsitu(sFileContents.data()).HasParseError())
	{
		HyError("HyTranslateTable::Load - Items had JSON parsing error: " << rapidjson::GetParseErrorFunc(itemsDoc.GetParseError()));
		return;
	}
	HyAssert(itemsDoc.IsObject(), "HyTranslateTable::Load - Items json file wasn't an object");

	for (auto it = itemsDoc.MemberBegin(); it != itemsDoc.MemberEnd(); ++it)
	{
		HyJsonObj keyObj = itemsDoc[it->name].GetObject();

		if (keyObj.HasMember(sm_language.c_str()))
		{
			rapidjson::Value::ConstMemberIterator languageMember = keyObj.FindMember(sm_language.c_str());

			HyTranslateTable::sm_table[it->name.GetString()] = languageMember->value.GetString();
		}
	}
}

/*static*/ const std::string HyTranslateTable::Get(const std::string id)
{
	if (sm_testMode)
		return "TEST";

	if (HyTranslateTable::sm_table.find(id) == HyTranslateTable::sm_table.end())
		return id;

	return HyTranslateTable::sm_table[id];
}

/*static*/ void HyTranslateTable::SetTestMode(bool state)
{
	sm_testMode = state;
}