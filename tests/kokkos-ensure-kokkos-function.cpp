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

// RUN: %check_clang_tidy %s kokkos-ensure-kokkos-function %t -- -header-filter=.* -system-headers -- -isystem %S/Inputs/kokkos/

#include "Kokkos_Core_mock.h"

KOKKOS_FUNCTION void legal(){}

void foo(){}
KOKKOS_FUNCTION void f(){foo();}
// CHECK-MESSAGES: :[[@LINE-1]]:26: warning: function 'foo' called in 'f' is missing a KOKKOS_X_FUNCTION annotation [kokkos-ensure-kokkos-function]


KOKKOS_FUNCTION void awesome_f2(){legal();}
