/**************************************************************************
 *	IHyTimeInst.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyTimeInst_h__
#define IHyTimeInst_h__

class IHyTimeInst
{
	friend class HyTime;

protected:
	bool			m_bIsRunning;
	double			m_dElapsedTime;

public:
	IHyTimeInst(void);
	virtual ~IHyTimeInst(void);

	bool IsRunning() const;
	double TimeElapsed() const;

	void SetElapsedTime(double dSeconds);
	void OffsetElapsedTime(double dSeconds);
	
	virtual std::string ToString() const = 0;

protected:
	void Update(double dDelta);
	virtual void OnUpdate() { };

	std::string FormatString(double dSeconds) const;
};

#endif /* IHyTimeInst_h__ */
