set(SDL_NET_SOURCE_FILES
	SDL_net.h
	SDLnet.c
	SDLnetselect.c
	SDLnetsys.h
	SDLnetTCP.c
	SDLnetUDP.c)

add_library(SDL2_net SHARED ${SDL_NET_SOURCE_FILES})
target_include_directories(SDL2_net PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(SDL2_net PUBLIC SDL2)

if(WIN32)
	target_link_libraries(SDL2_net PUBLIC Ws2_32 iphlpapi)
endif()

set_target_properties(SDL2_net PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	FOLDER "Engine Libs")
