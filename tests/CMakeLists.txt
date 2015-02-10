include_directories("../include")
FILE(GLOB TEST_INCLUDE_FILES "test_utils/*.hpp" "value_test/*.cpp")
add_library(UbexCpp_test_lib STATIC ${TEST_INCLUDE_FILES})
#MESSAGE( TEST_LIST  " : ${TEST_INCLUDE_FILES}" )
