//===-- Utility class to test different flavors of fma --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_MATH_FMATEST_H
#define LLVM_LIBC_TEST_SRC_MATH_FMATEST_H

#include "src/__support/FPUtil/FPBits.h"
#include "test/UnitTest/FPMatcher.h"
#include "test/UnitTest/Test.h"

template <typename T>
class FmaTestTemplate : public LIBC_NAMESPACE::testing::Test {
private:
  using Func = T (*)(T, T, T);
  using FPBits = LIBC_NAMESPACE::fputil::FPBits<T>;
  using StorageType = typename FPBits::StorageType;
  const T nan = T(FPBits::build_quiet_nan(1));
  const T inf = T(FPBits::inf());
  const T neg_inf = T(FPBits::neg_inf());
  const T zero = T(FPBits::zero());
  const T neg_zero = T(FPBits::neg_zero());

public:
  void test_special_numbers(Func func) {
    EXPECT_FP_EQ(func(zero, zero, zero), zero);
    EXPECT_FP_EQ(func(zero, neg_zero, neg_zero), neg_zero);
    EXPECT_FP_EQ(func(inf, inf, zero), inf);
    EXPECT_FP_EQ(func(neg_inf, inf, neg_inf), neg_inf);
    EXPECT_FP_EQ(func(inf, zero, zero), nan);
    EXPECT_FP_EQ(func(inf, neg_inf, inf), nan);
    EXPECT_FP_EQ(func(nan, zero, inf), nan);
    EXPECT_FP_EQ(func(inf, neg_inf, nan), nan);

    // Test underflow rounding up.
    EXPECT_FP_EQ(func(T(0.5), T(FPBits(FPBits::MIN_SUBNORMAL)),
                      T(FPBits(FPBits::MIN_SUBNORMAL))),
                 T(FPBits(StorageType(2))));
    // Test underflow rounding down.
    T v = T(FPBits(FPBits::MIN_NORMAL + StorageType(1)));
    EXPECT_FP_EQ(func(T(1) / T(FPBits::MIN_NORMAL << 1), v,
                      T(FPBits(FPBits::MIN_NORMAL))),
                 v);
    // Test overflow.
    T z = T(FPBits(FPBits::MAX_NORMAL));
    EXPECT_FP_EQ(func(T(1.75), z, -z), T(0.75) * z);
    // Exact cancellation.
    EXPECT_FP_EQ(func(T(3.0), T(5.0), -T(15.0)), T(0.0));
    EXPECT_FP_EQ(func(T(-3.0), T(5.0), T(15.0)), T(0.0));
  }
};

#endif // LLVM_LIBC_TEST_SRC_MATH_FMATEST_H
