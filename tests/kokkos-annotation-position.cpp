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

#include "Kokkos_Core_mock.h"

// Annotations before specifiers — these should NOT warn.
KOKKOS_FUNCTION static void ok_static() {}
KOKKOS_INLINE_FUNCTION inline int ok_inline() { return 0; }
KOKKOS_FORCEINLINE_FUNCTION static double ok_forceinline() { return 0.0; }

// Specifiers before annotations — these SHOULD warn.
static KOKKOS_FUNCTION void bad_static() {}
inline KOKKOS_INLINE_FUNCTION int bad_inline() { return 0; }
static KOKKOS_FORCEINLINE_FUNCTION double bad_forceinline() { return 0.0; }

struct MyStruct {
  // Annotation before specifier — should NOT warn.
  KOKKOS_INLINE_FUNCTION static void ok_method() {}

  // Specifier before annotation — SHOULD warn.
  static KOKKOS_INLINE_FUNCTION void bad_method() {}
  static KOKKOS_FUNCTION int bad_static_method() { return 0; }
};

// Multiple specifiers before annotation — should warn on the first.
static inline KOKKOS_FUNCTION void bad_multi_specifier() {}
