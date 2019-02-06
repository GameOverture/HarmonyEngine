#include "Afx/HyStdAfx.h"
#include "Utilities/HyRand.h"

template <typename TYPE>
/*static*/ TYPE HyRand::Get()
{
	return Range(std::numeric_limits<TYPE>::min(), std::numeric_limits<TYPE>::max());
}

//template <typename TYPE>
///*static*/ TYPE HyRand::Range(TYPE min, TYPE max)
//{
//	if(max < min)
//		return 
//}
