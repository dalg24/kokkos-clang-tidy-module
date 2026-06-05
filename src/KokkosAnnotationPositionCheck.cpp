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

#include "KokkosAnnotationPositionCheck.h"
#include "KokkosMatchers.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang::tidy::kokkos {

namespace {

bool isKokkosAnnotation(StringRef Annotation) {
  return Annotation == "KOKKOS_FUNCTION" ||
         Annotation == "KOKKOS_INLINE_FUNCTION" ||
         Annotation == "KOKKOS_FORCEINLINE_FUNCTION";
}

bool isDeclSpecifierKeywordText(StringRef Text) {
  return Text == "static" || Text == "inline" || Text == "virtual" ||
         Text == "explicit" || Text == "extern" || Text == "constexpr" ||
         Text == "consteval" || Text == "friend";
}

} // namespace

void KokkosAnnotationPositionCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      functionDecl(matchesAttr("KOKKOS_FUNCTION|KOKKOS_INLINE_FUNCTION|"
                               "KOKKOS_FORCEINLINE_FUNCTION"))
          .bind("func"),
      this);
}

void KokkosAnnotationPositionCheck::check(
    const MatchFinder::MatchResult &Result) {
  auto const *FD = Result.Nodes.getNodeAs<FunctionDecl>("func");
  auto &SM = *Result.SourceManager;
  auto const &LO = Result.Context->getLangOpts();

  // Get the source range to scan: from the beginning of the declaration to the
  // function name.  Both are resolved to file (non-macro) locations so we can
  // lex the raw source text reliably.
  auto DeclBeginLoc = SM.getFileLoc(FD->getBeginLoc());
  auto FuncNameLoc = SM.getFileLoc(FD->getLocation());

  if (DeclBeginLoc.isInvalid() || FuncNameLoc.isInvalid())
    return;
  if (!SM.isBeforeInTranslationUnit(DeclBeginLoc, FuncNameLoc))
    return;

  // Scan raw tokens in [DeclBeginLoc, FuncNameLoc).  We are looking for:
  //   • A Kokkos annotation macro name (e.g. KOKKOS_FUNCTION).
  //   • A decl-specifier keyword (static, inline, …).
  // A violation occurs when a specifier keyword appears before the Kokkos macro.
  SourceLocation SpecifierLoc;
  SourceLocation KokkosAttrLoc;

  SourceLocation ScanLoc = DeclBeginLoc;
  while (SM.isBeforeInTranslationUnit(ScanLoc, FuncNameLoc)) {
    Token Tok;
    if (Lexer::getRawToken(ScanLoc, Tok, SM, LO, /*IgnoreWhitespace=*/true))
      break;

    auto TokLoc = Tok.getLocation();
    if (!SM.isBeforeInTranslationUnit(TokLoc, FuncNameLoc))
      break;

    if (Tok.is(tok::raw_identifier)) {
      auto Text = Lexer::getSourceText(CharSourceRange::getTokenRange(TokLoc),
                                       SM, LO);
      if (isKokkosAnnotation(Text)) {
        KokkosAttrLoc = TokLoc;
        break; // No need to scan further; we found the annotation.
      }
      if (isDeclSpecifierKeywordText(Text) && KokkosAttrLoc.isInvalid()) {
        // Record the first specifier keyword seen before the annotation.
        if (SpecifierLoc.isInvalid())
          SpecifierLoc = TokLoc;
      }
    }

    auto NextLoc = Tok.getEndLoc();
    if (NextLoc == ScanLoc)
      break; // Safety: prevent infinite loop on zero-length tokens.
    ScanLoc = NextLoc;
  }

  // A violation requires: found a specifier before the annotation, and the
  // annotation was found too (otherwise the function might not use the macro).
  if (SpecifierLoc.isInvalid() || KokkosAttrLoc.isInvalid())
    return;

  auto MacroName = Lexer::getSourceText(
      CharSourceRange::getTokenRange(KokkosAttrLoc), SM, LO);
  auto SpecifierText = Lexer::getSourceText(
      CharSourceRange::getTokenRange(SpecifierLoc), SM, LO);

  auto Diag = diag(KokkosAttrLoc, "'%0' should appear before '%1'")
              << MacroName << SpecifierText;

  // Only emit fix-it when both locations are in the same file.
  if (SM.isWrittenInSameFile(KokkosAttrLoc, SpecifierLoc)) {
    // Remove the annotation token and reinsert it before the specifier.
    Diag << FixItHint::CreateRemoval(
                CharSourceRange::getTokenRange(KokkosAttrLoc))
         << FixItHint::CreateInsertion(SpecifierLoc,
                                       (MacroName + " ").str());
  }
}

} // namespace clang::tidy::kokkos
