//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
// See https://kokkos.org/LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER

#pragma once

#include "clang-tidy/ClangTidyCheck.h"

namespace clang::tidy::kokkos {

/// Check that KOKKOS_FUNCTION, KOKKOS_INLINE_FUNCTION, and
/// KOKKOS_FORCEINLINE_FUNCTION annotations appear before declaration
/// specifiers (static, inline, virtual, etc.), like C++11 attributes.
///
/// For example, `static KOKKOS_FUNCTION void foo()` must be written as
/// `KOKKOS_FUNCTION static void foo()`.
class KokkosAnnotationPositionCheck : public ClangTidyCheck {
public:
  KokkosAnnotationPositionCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace clang::tidy::kokkos
