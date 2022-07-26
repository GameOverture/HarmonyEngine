set(GLAD_SOURCE_FILES
	src/glad.cpp
	include/glad/glad.h
	include/KHR/khrplatform.h)

add_library(glad STATIC ${GLAD_SOURCE_FILES})
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(glad PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO)

set_target_properties(glad PROPERTIES FOLDER "Engine Libs")
