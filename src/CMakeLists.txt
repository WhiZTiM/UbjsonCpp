FILE(GLOB SOURCE_FILES "*.cpp")
include_directories(../include)
add_library(UbjsonCpp SHARED ${SOURCE_FILES} ${HEADER_FILES})
