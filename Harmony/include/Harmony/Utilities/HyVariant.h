/**************************************************************************
*	HyVariant.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyVariant_h__
#define __HyVariant_h__

union HyVariant
{
private:
	char asChar;
	unsigned char asUChar;
	short asShort;
	unsigned short asUShort;
	int asInt;
	unsigned int asUInt;
	long asLong;
	unsigned long asULong;
	float asFloat;
	double asDouble;
	char* asStr;
	
public:
	HyVariant() { asULong = 0; }
	HyVariant(char in) { asChar = in; }
	HyVariant(unsigned char in) { asUChar = in; }
	HyVariant(short in) { asShort = in; }
	HyVariant(unsigned short in) { asUShort = in; }
	HyVariant(int in) { asInt = in; }
	HyVariant(unsigned int in) { asUInt = in; }
	HyVariant(long in) { asLong = in; }
	HyVariant(unsigned long in) { asULong = in; }
	HyVariant(float in) { asFloat = in; }
	HyVariant(double in) { asDouble = in; }
	HyVariant(char* in) { asStr = in; }
	
	operator char() { return asChar; }
	operator unsigned char() { return asUChar; }
	operator short() { return asShort; }
	operator unsigned short() { return asUShort; }
	operator int() { return asInt; }
	operator unsigned int() { return asUInt; }
	operator long() { return asLong; }
	operator unsigned long() { return asULong; }
	operator float() { return asFloat; }
	operator double() { return asDouble; }
	operator char*() { return asStr; }

	~HyVariant();
};

#endif /* __HyVariant_h__ */
