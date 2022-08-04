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

#include "vendor/rapidjson/document.h"
typedef rapidjson::Document HyJsonDoc;
typedef rapidjson::Value HyJsonValue;
typedef rapidjson::GenericArray<false, rapidjson::Value> HyJsonArray;
typedef rapidjson::GenericObject<false, rapidjson::Value> HyJsonObj;


//#ifdef HY_USE_RAPIDJSON
//	#ifdef TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR
//		// This uses the RapidJSON CRTAllocator.  It is thread safe and multiple
//		// documents may be active at once.
//		using json =
//			rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
//		using json_const_iterator = json::ConstMemberIterator;
//		using json_const_array_iterator = json const *;
//		using JsonDocument =
//			rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator>;
//		rapidjson::CrtAllocator s_CrtAllocator;  // stateless and thread safe
//		rapidjson::CrtAllocator &GetAllocator() { return s_CrtAllocator; }
//	#else
//		// This uses the default RapidJSON MemoryPoolAllocator.  It is very fast, but
//		// not thread safe. Only a single JsonDocument may be active at any one time,
//		// meaning only a single gltf load/save can be active any one time.
//		using json = rapidjson::Value;
//		using json_const_iterator = json::ConstMemberIterator;
//		using json_const_array_iterator = json const *;
//		rapidjson::Document *s_pActiveDocument = nullptr;
//		rapidjson::Document::AllocatorType &GetAllocator() {
//			assert(s_pActiveDocument);  // Root json node must be JsonDocument type
//			return s_pActiveDocument->GetAllocator();
//		}
//
//
//		struct HyJsonDoc : public rapidjson::Document {
//			HyJsonDoc() {
//				assert(s_pActiveDocument ==
//					nullptr);  // When using default allocator, only one document can be
//							   // active at a time, if you need multiple active at once,
//							   // define TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR
//				s_pActiveDocument = this;
//			}
//			HyJsonDoc(const HyJsonDoc &) = delete;
//			HyJsonDoc(HyJsonDoc &&rhs) noexcept
//				: rapidjson::Document(std::move(rhs)) {
//				s_pActiveDocument = this;
//				rhs.isNil = true;
//			}
//			~HyJsonDoc() {
//				if(!isNil) {
//					s_pActiveDocument = nullptr;
//				}
//			}
//
//		private:
//			bool isNil = false;
//		};
//	#endif  // TINYGLTF_USE_RAPIDJSON_CRTALLOCATOR
//
//#else
//	#include <nlohmann/json.hpp>
//	typedef nlohmann::json HyJsonDoc;
//	typedef nlohmann::json::value_type HyJsonValue;
//	typedef nlohmann::json::array_t HyJsonArray;
//	typedef nlohmann::json::object_t HyJsonObj;
//
//	//using json_const_iterator = nlohmann::json::const_iterator;
//	//using json_const_array_iterator = json_const_iterator;
//	////using JsonDocument = nlohmann::json;
//	//using HyJsonDoc = nlohmann::json;
//#endif

#endif /* HyJson_h__ */
