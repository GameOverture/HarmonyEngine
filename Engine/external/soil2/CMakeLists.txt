set(SOIL2_SOURCE_FILES
	src/etc1_utils.c
	src/image_DXT.c
	src/image_helper.c
	src/SOIL2.c)

set(SOIL2_HEADER_FILES
	include/soil2/etc1_utils.h
	include/soil2/image_DXT.h
	include/soil2/image_helper.h
	include/soil2/jo_jpeg.h
	include/soil2/pkm_helper.h
	include/soil2/pvr_helper.h
	include/soil2/SOIL2.h
	include/soil2/stbi_DDS.h
	include/soil2/stbi_DDS_c.h
	include/soil2/stbi_ext.h
	include/soil2/stbi_ext_c.h
	include/soil2/stbi_pkm.h
	include/soil2/stbi_pkm_c.h
	include/soil2/stbi_pvr.h
	include/soil2/stbi_pvr_c.h
	include/soil2/stb_image.h
	include/soil2/stb_image_write.h)

add_library(soil2 STATIC ${SOIL2_SOURCE_FILES} ${SOIL2_HEADER_FILES})
target_include_directories(soil2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(soil2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/soil2")
target_include_directories(soil2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../glad/include")
set_target_properties(soil2 PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)

set_target_properties(soil2 PROPERTIES FOLDER "Engine Libs")
