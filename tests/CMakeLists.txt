function(add_run_clang_tidy_test check_name)
  add_test(NAME "RunClangTidy.${check_name}" COMMAND ${CMAKE_COMMAND}
    "-DCLANG_TIDY_COMMAND=$<TARGET_FILE:clang-tidy>"
    "-DCLANG_TIDY_MODULE=$<TARGET_FILE:KokkosClangTidyModule>"
    "-DCHECK_NAME=${check_name}"
    "-DRunClangTidy_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/RunClangTidy.cmake"
    )
endfunction()

add_run_clang_tidy_test(kokkos-implicit-this-capture)
add_run_clang_tidy_test(kokkos-replace-view-stride-underscore)
