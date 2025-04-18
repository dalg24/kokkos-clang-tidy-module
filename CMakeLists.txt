cmake_minimum_required(VERSION 3.16)
project(CMakeClangTidyModule C CXX)

find_package(Clang REQUIRED)

add_library(KokkosClangTidyModule MODULE
  src/KokkosTidyModule.cpp

  src/ReplaceViewStrideUnderscoreCheck.cpp
  src/ReplaceViewStrideUnderscoreCheck.h
  src/EnsureKokkosFunctionCheck.cpp
  src/EnsureKokkosFunctionCheck.h
  src/ImplicitThisCaptureCheck.cpp
  src/ImplicitThisCaptureCheck.h
  src/KokkosMatchers.cpp
  src/KokkosMatchers.h
  )
target_include_directories(KokkosClangTidyModule PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(KokkosClangTidyModule PRIVATE clang-tidy)
target_compile_features(KokkosClangTidyModule PRIVATE cxx_std_17)

option(RUN_TESTS "Run the tests for the clang-tidy module" OFF)
if(RUN_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
