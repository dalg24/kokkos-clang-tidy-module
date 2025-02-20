cmake_minimum_required(VERSION 3.16)
project(CMakeClangTidyModule C CXX)

find_package(Clang REQUIRED)

add_library(KokkosClangTidyModule MODULE
  KokkosTidyModule.cpp

  ImplicitThisCaptureCheck.cpp
  ImplicitThisCaptureCheck.h
  KokkosMatchers.cpp
  KokkosMatchers.h
  )
target_include_directories(KokkosClangTidyModule PRIVATE ${CLANG_INCLUDE_DIRS})
target_link_libraries(KokkosClangTidyModule PRIVATE clang-tidy)
target_compile_features(KokkosClangTidyModule PRIVATE cxx_std_17)
