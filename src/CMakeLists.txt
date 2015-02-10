FILE(GLOB SOURCE_FILES "*.cpp")
include_directories(../include)
add_library(UbexCpp_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
