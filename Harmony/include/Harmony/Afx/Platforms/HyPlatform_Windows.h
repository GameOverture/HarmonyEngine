/**************************************************************************
 *	HyPlatform_Windows.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyPlatform_Windows_h__
#define __HyPlatform_Windows_h__

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <sstream>
#include <tchar.h>

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"

#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glext.h"

// Data types
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

typedef signed char			char8;
typedef unsigned char		uchar8;
typedef signed char			int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed int			int32;
typedef unsigned int		uint32;
typedef __int64				int64;
typedef unsigned __int64	uint64;

// Support for the endian issues
#define HY_ENDIAN_LITTLE	1
#define HY_ENDIAN_BIG		0

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) \
		do { \
			if(!(condition)) \
			{	std::stringstream ss; \
				ss << "Assertion (" #condition ") failed.\n\n" << __FILE__ \
				<< "\nLine: " << __LINE__ << "\n\n" << message << std::endl; \
				MessageBoxA(NULL, ss.str().c_str(), "Harmony Engine Assert!", MB_ICONERROR); \
				std::exit(EXIT_FAILURE); \
			} \
		} while (false)

	#define HyError(message) \
		do { \
			std::stringstream ss; \
			ss << "Harmony Error has occured.\n\n" << __FILE__ \
			<< "\nLine: " << __LINE__ << "\n\n" << message << std::endl; \
			MessageBoxA(NULL, ss.str().c_str(), "Harmony Engine Error!", MB_ICONERROR); \
			std::exit(EXIT_FAILURE); \
		} while (false)
#elif defined(HY_RELEASE)
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif

#endif /* __HyPlatform_Windows_h__ */