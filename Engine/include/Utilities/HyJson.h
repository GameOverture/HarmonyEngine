/**************************************************************************
*	HyJson.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyJson_h__
#define HyJson_h__

#include "Afx/HyStdAfx.h"

// TODO: Get rid of this #define - Upgrade RapidJson to a newer release version once it becomes available
#ifdef HY_COMPILER_MSVC
	#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif
#include "vendor/rapidjson/document.h"
typedef rapidjson::Document HyJsonDoc;
typedef rapidjson::Value HyJsonValue;
typedef rapidjson::GenericArray<false, rapidjson::Value> HyJsonArray;
typedef rapidjson::GenericObject<false, rapidjson::Value> HyJsonObj;

#endif /* HyJson_h__ */
