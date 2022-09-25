/**************************************************************************
*	HyAudioBank.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_h__
#define HyAudioBank_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyJson.h"

class IHyAudioCore;

class IHySoundBuffer
{
protected:
	IHyAudioCore &				m_CoreRef;
	const std::string			m_sFILE_NAME;
	const int32					m_iGROUP_ID;
	const bool					m_bIS_STREAMING;
	const int32					m_iINSTANCE_LIMIT;

public:
	IHySoundBuffer(IHyAudioCore &coreRef, std::string sFileName, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit) :
		m_CoreRef(coreRef),
		m_sFILE_NAME(sFileName),
		m_iGROUP_ID(iGroupId),
		m_bIS_STREAMING(bIsStreaming),
		m_iINSTANCE_LIMIT(iInstanceLimit)
	{ }
	virtual ~IHySoundBuffer() {
	}

	int32 GetGroupId() const {
		return m_iGROUP_ID;
	}

	bool IsStreaming() const {
		return m_bIS_STREAMING;
	}

	int32 GetInstanceLimit() const {
		return m_iINSTANCE_LIMIT;
	}

	virtual bool Load(std::string sFilePath) = 0;
	virtual void Unload() = 0;
};

class HyAudioBank
{
	std::vector<IHySoundBuffer *>			m_SoundBuffers;
	std::map<uint32, IHySoundBuffer *>		m_ChecksumMap;

public:
	HyAudioBank(IHyAudioCore &coreRef, HyJsonObj bankObj);
	virtual ~HyAudioBank(void);

	virtual bool ContainsSound(uint32 uiAssetChecksum);
	IHySoundBuffer *GetSound(uint32 uiChecksum);

	virtual bool Load(std::string sFilePath);
	virtual void Unload();
};

class HySoundBuffer_Null : public IHySoundBuffer
{
public:
	HySoundBuffer_Null(IHyAudioCore &coreRef, std::string sFileName, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit) :
		IHySoundBuffer(coreRef, sFileName, iGroupId, bIsStreaming, iInstanceLimit)
	{ }
	virtual bool Load(std::string sFilePath) override { return true; }
	virtual void Unload() override { }
};

class HyAudioBank_Null : public HyAudioBank
{
public:
	HyAudioBank_Null() = default;
	virtual ~HyAudioBank_Null() = default;

	virtual bool ContainsSound(uint32 uiAssetChecksum) override
	{ return false; }

	virtual bool Load(std::string sFilePath) override
	{ return true; }

	virtual void Unload() override
	{ }
};

#endif /* HyAudioBank_h__ */
