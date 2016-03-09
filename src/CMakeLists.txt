FILE(GLOB SOURCE_FILES_LIB "*.cpp")
include_directories(../include)
add_library(UbjsonCpp SHARED ${SOURCE_FILES_LIB} ${HEADER_FILES_LIB})
