//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// inserter_proxy.cpp
//
// Identification: src/execution/proxy/inserter_proxy.cpp
//
// Copyright (c) 2015-2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/proxy/inserter_proxy.h"

#include "execution/proxy/data_table_proxy.h"
#include "execution/proxy/executor_context_proxy.h"
#include "execution/proxy/pool_proxy.h"
#include "execution/proxy/transaction_context_proxy.h"
#include "execution/proxy/tuple_proxy.h"

namespace terrier::execution {

DEFINE_TYPE(Inserter, "Inserter", opaque);

DEFINE_METHOD(peloton::codegen, Inserter, Init);
DEFINE_METHOD(peloton::codegen, Inserter, AllocateTupleStorage);
DEFINE_METHOD(peloton::codegen, Inserter, GetPool);
DEFINE_METHOD(peloton::codegen, Inserter, Insert);
DEFINE_METHOD(peloton::codegen, Inserter, TearDown);

}  // namespace terrier::execution
