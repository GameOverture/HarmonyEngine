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


template <typename T>
ReleaseManager<T>::ReleaseManager(void (*releaseFunction)(T*)):
    _releaseFunction(releaseFunction) {}

template <typename T>
ReleaseManager<T>::~ReleaseManager() {

    for(unsigned i=0; i < (unsigned) _releaseQueue.size(); ++i)

        if(*_releaseQueue.at(i)) {

            if(_releaseFunction)
                _releaseFunction(*_releaseQueue.at(i));
            else
                delete *_releaseQueue.at(i);
        }

    for(unsigned i=0; i < (unsigned) _releaseAddressQueue.size(); ++i)

        if(_releaseAddressQueue.at(i)) {

            if(_releaseFunction)
                _releaseFunction(_releaseAddressQueue.at(i));
            else
                delete _releaseAddressQueue.at(i);

        }
}

template <typename T>
void ReleaseManager<T>::add(T** var) {

    _releaseQueue.push_back(var);
}

template <typename T>
void ReleaseManager<T>::add(T* address) {

    _releaseAddressQueue.push_back(address);
}

