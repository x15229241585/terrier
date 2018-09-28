//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// tuple_value_translator.cpp
//
// Identification: src/execution/expression/tuple_value_translator.cpp
//
// Copyright (c) 2015-2017, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/expression/tuple_value_translator.h"

#include "expression/tuple_value_expression.h"

namespace terrier::execution {

// Constructor
TupleValueTranslator::TupleValueTranslator(const expression::TupleValueExpression &tve_expr,
                                           CompilationContext &context)
    : ExpressionTranslator(tve_expr, context) {
  PELOTON_ASSERT(tve_expr.GetAttributeRef() != nullptr);
}

// Produce the value that is the result of codegening the expression
Value TupleValueTranslator::DeriveValue(CodeGen &codegen, RowBatch::Row &row) const {
  const auto &tve_expr = GetExpressionAs<expression::TupleValueExpression>();
  return row.DeriveValue(codegen, tve_expr.GetAttributeRef());
}

}  // namespace terrier::execution