/**************************************************************************
*	HyStorage.h
*
*	Harmony Engine
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyStorage_h__
#define HyStorage_h__

#include "Afx/HyStdAfx.h"

class HyStorage
{
	const int32		m_iSESSION_ONLY;					// Stored as an int for ease of use in Emscripten API

public:
	HyStorage(bool bIsSessionOnly);

	bool Exists(std::string sKey);

	uint32 Length();									// Returns an integer representing the number of data items stored in the Storage object.
	std::string Key(uint32 uiIndex);					// When passed a number n, this method will return the name of the nth key in the storage.
	int32 GetInt(std::string sKey);						// When passed a key name, will return that key's value.
	double GetDouble(std::string sKey);					// When passed a key name, will return that key's value.
	std::string GetString(std::string sKey);			// When passed a key name, will return that key's value.
	void SetItem(std::string sKey, int32 iValue);		// When passed a key name and value, will add that key to the storage, or update that key's value if it already exists.
	void SetItem(std::string sKey, double dValue);		// When passed a key name and value, will add that key to the storage, or update that key's value if it already exists.
	void SetItem(std::string sKey, std::string sValue);	// When passed a key name and value, will add that key to the storage, or update that key's value if it already exists.
	void RemoveItem(std::string sKey);					// When passed a key name, will remove that key from the storage.
	void Clear();										// When invoked, will empty all keys out of the storage.
};

#endif /* HyStorage_h__ */
