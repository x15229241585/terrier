//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// hash_group_by_translator.h
//
// Identification: src/include/execution/operator/hash_group_by_translator.h
//
// Copyright (c) 2015-2017, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/aggregation.h"
#include "execution/oa_hash_table.h"
#include "execution/operator/operator_translator.h"
#include "execution/updateable_storage.h"

namespace terrier::execution {

namespace planner {
class AggregatePlan;
}  // namespace planner

//===----------------------------------------------------------------------===//
// The translator for a hash-based group-by operator.
//===----------------------------------------------------------------------===//
class HashGroupByTranslator : public OperatorTranslator {
 public:
  // Global/configurable variable controlling whether hash aggregations prefetch
  static std::atomic<bool> kUsePrefetch;

  // Constructor
  HashGroupByTranslator(const planner::AggregatePlan &group_by, CompilationContext &context, Pipeline &pipeline);

  // Codegen any initialization work for this operator
  void InitializeQueryState() override;

  // Define any helper functions this translator needs
  void DefineAuxiliaryFunctions() override {}

  // The method that produces new tuples
  void Produce() const override;

  // The method that consumes tuples from child operators
  void Consume(ConsumerContext &context, RowBatch::Row &row) const override;
  void Consume(ConsumerContext &context, RowBatch &batch) const override;

  // Codegen any cleanup work for this translator
  void TearDownQueryState() override;

 private:
  //===--------------------------------------------------------------------===//
  // The callback the group-by uses when iterating the results of the hash table
  //===--------------------------------------------------------------------===//
  class ProduceResults : public HashTable::VectorizedIterateCallback {
   public:
    // Constructor
    ProduceResults(ConsumerContext &ctx, const planner::AggregatePlan &plan, const Aggregation &aggregation);

    // The callback
    void ProcessEntries(CodeGen &codegen, llvm::Value *start, llvm::Value *end, Vector &selection_vector,
                        HashTable::HashTableAccess &access) const override;

   private:
    ConsumerContext &ctx_;
    const planner::AggregatePlan &plan_;
    const Aggregation &aggregation_;
  };

  //===--------------------------------------------------------------------===//
  // The callback used when we probe the hash table when aggregating, but find
  // an existing value associated with a given key. We, therefore, perform the
  // actual aggregation ... duh
  //===--------------------------------------------------------------------===//
  class ConsumerProbe : public HashTable::ProbeCallback {
   public:
    // Constructor
    ConsumerProbe(CompilationContext &context, const Aggregation &aggregation,
                  const std::vector<Value> &next_vals, const std::vector<Value> &grouping_keys);

    // The callback
    void ProcessEntry(CodeGen &codegen, llvm::Value *data_area) const override;

   private:
    // Compilation Context need for code generation and runtime state
    CompilationContext &context_;
    // The guy that handles the computation of the aggregates
    const Aggregation &aggregation_;
    // The next value to merge into the existing aggregates
    const std::vector<Value> &next_vals_;
    // The key used for the Group By, will be needed for distinct aggregations
    const std::vector<Value> grouping_keys_;
  };

  //===--------------------------------------------------------------------===//
  // The callback used when we probe the hash table when aggregating, but do
  // not find an existing entry. At this point, we insert the values as initial
  // aggregates.
  //===--------------------------------------------------------------------===//
  class ConsumerInsert : public HashTable::InsertCallback {
   public:
    // Constructor
    ConsumerInsert(const Aggregation &aggregation, const std::vector<Value> &initial_vals,
                   const std::vector<Value> &grouping_keys);

    // StoreValue the initial values of the aggregates into the provided storage
    void StoreValue(CodeGen &codegen, llvm::Value *data_space) const override;

    llvm::Value *GetValueSize(CodeGen &codegen) const override;

   private:
    // The guy that handles the computation of the aggregates
    const Aggregation &aggregation_;
    // The list of initial values to use as aggregates
    const std::vector<Value> &initial_vals_;
    // The key used for the Group By, will be needed for distinct aggregations
    const std::vector<Value> grouping_keys_;
  };

  //===--------------------------------------------------------------------===//
  // An aggregate finalizer allows aggregations to delay the finalization of an
  // aggregate in the hash-table to a later time. This is needed when we do
  // vectorized scans of the hash-table.
  //===--------------------------------------------------------------------===//
  class AggregateFinalizer {
   public:
    AggregateFinalizer(const Aggregation &aggregation, HashTable::HashTableAccess &hash_table_access);
    // Get the finalized aggregates at the given position in the results
    const std::vector<Value> &GetAggregates(CodeGen &codegen, llvm::Value *index);

   private:
    // The aggregator
    Aggregation aggregation_;
    // The hash-table accessor
    HashTable::HashTableAccess &hash_table_access_;
    // Whether the aggregate has been finalized and the results
    bool finalized_;
    std::vector<Value> final_aggregates_;
  };

  //===--------------------------------------------------------------------===//
  // This class provides (delayed) access to individual attributes of tuples in
  // the result of the aggregation.
  //===--------------------------------------------------------------------===//
  class AggregateAccess : public RowBatch::AttributeAccess {
   public:
    AggregateAccess(AggregateFinalizer &finalizer, uint32_t agg_index);

    Value Access(CodeGen &codegen, RowBatch::Row &row) override;

   private:
    // The associate finalizer
    AggregateFinalizer &finalizer_;
    // The index in the tuple's attributes
    uint32_t agg_index_;
  };

  void CollectHashKeys(RowBatch::Row &row, std::vector<Value> &key) const;

  // Estimate the size of the constructed hash table
  uint64_t EstimateHashTableSize() const;

  // Should this operator employ prefetching?
  bool UsePrefetching() const;

 private:
  // The pipeline forming all child operators of this aggregation
  Pipeline child_pipeline_;

  // The ID of the hash-table in the runtime state
  QueryState::Id hash_table_id_;

  // The hash table
  OAHashTable hash_table_;

  // The aggregation handler
  Aggregation aggregation_;
};

}  // namespace terrier::execution