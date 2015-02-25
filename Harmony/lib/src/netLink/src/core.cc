/*
    NetLink Sockets: Networking C++ library
    Copyright 2012 Pedro Francisco Pareja Ruiz (PedroPareja@Gmail.com)

    This file is part of NetLink Sockets.

    NetLink Sockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NetLink Sockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NetLink Sockets. If not, see <http://www.gnu.org/licenses/>.

*/

#include <netlink/core.h>


NL_NAMESPACE

/**
* \fn void init()
* Library initialization function
*
* @warning Must be called before using the library
*/


void init() {

	#ifdef OS_WIN32

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0)
			throw Exception(Exception::ERROR_INIT, "Library inicialization failed");

	#endif
}

NL_NAMESPACE_END

