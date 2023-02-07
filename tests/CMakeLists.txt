include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(phTests
  main.cpp
)
target_link_libraries(phTests
  GTest::gtest_main
  coolingLib
)
target_include_directories(phTests
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../includes/
PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/
)
include(GoogleTest)
gtest_discover_tests(phTests)

#add_test(NAME phTests COMMAND phTests)
enable_testing()
