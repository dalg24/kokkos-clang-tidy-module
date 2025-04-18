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

#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include "ReplaceViewStrideUnderscoreCheck.h"
#include "EnsureKokkosFunctionCheck.h"
#include "ImplicitThisCaptureCheck.h"

namespace clang {
namespace tidy {
namespace kokkos {

class KokkosModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<ReplaceViewStrideUnderscoreCheck>(
        "kokkos-replace-view-stride-underscore");
    CheckFactories.registerCheck<EnsureKokkosFunctionCheck>(
        "kokkos-ensure-kokkos-function");
    CheckFactories.registerCheck<ImplicitThisCaptureCheck>(
        "kokkos-implicit-this-capture");
  }
};

} // namespace kokkos

// Register the DarwinTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<kokkos::KokkosModule>
    X("kokkos-module", "Adds Kokkos specific linting checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the KokkosModule.
volatile int KokkosModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
