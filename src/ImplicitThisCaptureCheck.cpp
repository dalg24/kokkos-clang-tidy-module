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

#include "ImplicitThisCaptureCheck.h"
#include "KokkosMatchers.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <optional>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace kokkos {

namespace {
std::optional<SourceLocation> capturesThis(CXXRecordDecl const *CRD) {
  if (!CRD->isLambda()) {
    return std::nullopt;
  }

  for (auto const &Capture : CRD->captures()) {
    if (Capture.capturesThis()) {
      return Capture.getLocation();
    }
  }

  return std::nullopt;
}

} // namespace

ImplicitThisCaptureCheck::ImplicitThisCaptureCheck(StringRef Name,
                                                   ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context) {
  AllowIfExplicitHost = std::stoi(Options.get("AllowIfExplicitHost", "0").str());
}

void ImplicitThisCaptureCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "AllowIfExplicitHost",
                std::to_string(AllowIfExplicitHost));
}

void ImplicitThisCaptureCheck::registerMatchers(MatchFinder *Finder) {
  auto isAllowedPolicy = expr(hasType(
      cxxRecordDecl(matchesName("::Impl::ThreadVectorRangeBoundariesStruct.*|"
                                "::Impl::TeamThreadRangeBoundariesStruct.*"))));

  auto Lambda = expr(hasType(cxxRecordDecl(isLambda()).bind("Lambda")));

  Finder->addMatcher(callExpr(isKokkosParallelCall(), hasAnyArgument(Lambda),
                              unless(hasAnyArgument(isAllowedPolicy)))
                         .bind("x"),
                     this);
}

void ImplicitThisCaptureCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *CE = Result.Nodes.getNodeAs<CallExpr>("x");

  AllowIfExplicitHost = std::stoi(Options.get("AllowIfExplicitHost", "0").str());

  auto const *Lambda = Result.Nodes.getNodeAs<CXXRecordDecl>("Lambda");
  auto CaptureLocation = capturesThis(Lambda);
  if (CaptureLocation) {
    diag(Lambda->getBeginLoc(), "Lambda passed to %0 implicitly captures this.")
        << CE->getDirectCallee()->getName();
    diag(CaptureLocation.value(), "the captured variable is used here.",
         DiagnosticIDs::Note);
  }
}

} // namespace kokkos
} // namespace tidy
} // namespace clang
