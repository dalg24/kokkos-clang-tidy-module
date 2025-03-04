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

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_KOKKOS_KOKKOSMATCHERS_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_KOKKOS_KOKKOSMATCHERS_H

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <string>

namespace clang {
namespace tidy {
namespace kokkos {

AST_MATCHER_FUNCTION(ast_matchers::internal::Matcher<FunctionDecl>,
                     kokkosParallelXFunctionDecl) {
  using namespace clang::ast_matchers;
  return functionDecl(matchesName("::Kokkos::parallel_.*"));
}

AST_MATCHER(CallExpr, isKokkosParallelCall) {
  using namespace clang::ast_matchers;
  if (auto const *FD = Node.getDirectCallee()) {
    std::string Name = FD->getQualifiedNameAsString();
    StringRef SR(Name);
    if (SR.startswith("::Kokkos::parallel_") ||
        SR.startswith("Kokkos::parallel_")) {
      return true;
    }
  }

  return false;
}

bool matchesAnnotation(Decl const* D, std::string const& RegExp);

AST_MATCHER_P(Decl, matchesAttr, std::string, RegExp) {
  return matchesAnnotation(&Node, RegExp);
}

bool explicitlyDefaultHostExecutionSpace(CallExpr const *CE);

} // namespace kokkos
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_KOKKOS_KOKKOSMATCHERS_H
