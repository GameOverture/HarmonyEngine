set(SDL_MIXER_SOURCE_FILES
	effect_position.c
	effect_stereoreverse.c
	effects_internal.c
	effects_internal.h
	load_aiff.c
	load_aiff.h
	load_voc.c
	load_voc.h
	mixer.c
	mixer.h
	music.c
	music.h
	music_cmd.c
	music_cmd.h
	music_flac.c
	music_flac.h
	music_fluidsynth.c
	music_fluidsynth.h
	music_mad.c
	music_mad.h
	music_mikmod.c
	music_mikmod.h
	music_modplug.c
	music_modplug.h
	music_mpg123.c
	music_mpg123.h
	music_nativemidi.c
	music_nativemidi.h
	music_ogg.c
	music_ogg.h
	music_opus.c
	music_opus.h
	music_timidity.c
	music_timidity.h
	music_wav.c
	music_wav.h)

add_library(SDL2_mixer SHARED ${SDL_MIXER_SOURCE_FILES})
target_include_directories(SDL2_mixer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(SDL2_mixer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/timidity")
target_include_directories(SDL2_mixer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/native_midi")
target_include_directories(SDL2_mixer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/VisualC/external/include")

target_link_libraries(SDL2_mixer PUBLIC SDL2)

target_compile_definitions(SDL2_mixer PRIVATE MUSIC_WAV)
target_compile_definitions(SDL2_mixer PRIVATE MUSIC_OGG)
target_compile_definitions(SDL2_mixer PRIVATE OGG_DYNAMIC="libvorbisfile-3.dll")

set_target_properties(SDL2_mixer PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO
	FOLDER "Engine Libs")
