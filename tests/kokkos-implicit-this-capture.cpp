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

// RUN: %check_clang_tidy %s kokkos-implicit-this-capture %t -- -header-filter=.* -system-headers -- -isystem %S/Inputs/kokkos/

#include "Kokkos_Core_mock.h"

KOKKOS_FUNCTION void do_something(int x) { printf("Got int %d\n", x); }
using TeamMember = Kokkos::TeamPolicy<>::member_type;
class BadClass {
  int my_int;

public:
  void capturesThis() {
    Kokkos::parallel_for(
        10, KOKKOS_LAMBDA(int x) { my_int = x; });
    // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: Lambda passed to parallel_for implicitly captures this. [kokkos-implicit-this-capture]
  }
};

// Should not warn
struct GoodClass {
  int foo;
  TeamMember member;

  KOKKOS_INLINE_FUNCTION void method() {
    auto TTR = Kokkos::TeamThreadRange(member, 15);
    Kokkos::parallel_for(TTR, [&](int i) {
      auto TVR = Kokkos::ThreadVectorRange(member, 3);
      auto func = [&](int j) {
        do_something(foo);
        do_something(i + j);
      };
      Kokkos::parallel_for(TVR, func);
    });
  }
};

namespace ConsiderDroppingThis {

KOKKOS_FUNCTION void do_something(int x) { printf("Got int %d\n", x); }

class BadClass {
  int MyInt = 0;

public:
  void capturesThis() {
    Kokkos::parallel_for(
        Kokkos::RangePolicy<Kokkos::DefaultExecutionSpace>(0,10), 
        [=](int) { do_something(MyInt); });
    // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: Lambda passed to parallel_for implicitly captures this. [kokkos-implicit-this-capture]
  }
};

class GoodClass {
  int MyInt = 0;

public:
  void capturesThis() {
    Kokkos::parallel_for(
        Kokkos::RangePolicy<Kokkos::DefaultHostExecutionSpace>(0,10), 
        KOKKOS_LAMBDA(int) { do_something(MyInt); });
  }
};

}
