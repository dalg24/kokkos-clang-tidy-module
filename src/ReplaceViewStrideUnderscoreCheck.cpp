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

#include "ReplaceViewStrideUnderscoreCheck.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"

clang::tidy::kokkos::ReplaceViewStrideUnderscoreCheck::
    ReplaceViewStrideUnderscoreCheck(clang::StringRef Name,
                                     clang::tidy::ClangTidyContext *Context)
    : clang::tidy::ClangTidyCheck(Name, Context) {}

void clang::tidy::kokkos::ReplaceViewStrideUnderscoreCheck::registerMatchers(
    clang::ast_matchers::MatchFinder *Finder) {
  using namespace clang::ast_matchers;
  auto ViewType = hasType(cxxRecordDecl(hasName("View")));
  auto StrideUnderscroreMethodDecl = cxxMethodDecl(matchesName("stride_[0-7]"));
  Finder->addMatcher(cxxMemberCallExpr(onImplicitObjectArgument(ViewType),
                                       callee(StrideUnderscroreMethodDecl))
                         .bind("X"),
                     this);
}

void clang::tidy::kokkos::ReplaceViewStrideUnderscoreCheck::check(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  auto const *CE = Result.Nodes.getNodeAs<CXXMemberCallExpr>("X");

  auto StrideI = CE->getMethodDecl()->getName();
  auto StrideParensI =
      (StrideI.substr(0, 6) + "(" + StrideI.substr(7, 8) + ")").str();

  diag(CE->getExprLoc(),
       "Kokkos::View::%0() is deprecated, use Kokkos::View::%1 instead")
      << StrideI << StrideParensI
      << FixItHint::CreateReplacement(
             SourceRange(CE->getExprLoc(), CE->getRParenLoc()), StrideParensI);
}
