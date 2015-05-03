FILE(GLOB SOURCE_FILES "*.cpp")
include_directories(../include)
add_library(UbexCpp_lib SHARED ${SOURCE_FILES} ${HEADER_FILES})
