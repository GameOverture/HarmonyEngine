/**************************************************************************
*	HyRand.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRand_h__
#define HyRand_h__

class HyRand
{
public:
	template <typename TYPE>
	static TYPE Get();

	template <typename TYPE>
	static typename std::enable_if<hy_is_type_integer<TYPE>::value, TYPE>::type	Range(TYPE min, TYPE max);
};

#endif /* HyRand_h__ */
