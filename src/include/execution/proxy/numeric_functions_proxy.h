//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// numeric_functions_proxy.h
//
// Identification: src/include/execution/proxy/numeric_functions_proxy.h
//
// Copyright (c) 2015-2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/proxy/proxy.h"

namespace terrier::execution {

PROXY(NumericFunctions) {
  // Utility functions
  DECLARE_METHOD(Abs);
  DECLARE_METHOD(Floor);
  DECLARE_METHOD(Round);
  DECLARE_METHOD(Ceil);

  // Input functions
  DECLARE_METHOD(InputBoolean);
  DECLARE_METHOD(InputTinyInt);
  DECLARE_METHOD(InputSmallInt);
  DECLARE_METHOD(InputInteger);
  DECLARE_METHOD(InputBigInt);
  DECLARE_METHOD(InputDecimal);
};

}  // namespace terrier::execution
