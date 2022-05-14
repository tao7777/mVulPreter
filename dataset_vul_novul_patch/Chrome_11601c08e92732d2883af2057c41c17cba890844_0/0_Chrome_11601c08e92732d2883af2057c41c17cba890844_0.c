IndexedDBTransaction* IndexedDBConnection::CreateTransaction(
    int64_t id,
     const std::set<int64_t>& scope,
     blink::WebIDBTransactionMode mode,
     IndexedDBBackingStore::Transaction* backing_store_transaction) {
  CHECK_EQ(GetTransaction(id), nullptr) << "Duplicate transaction id." << id;
   std::unique_ptr<IndexedDBTransaction> transaction =
       IndexedDBClassFactory::Get()->CreateIndexedDBTransaction(
           id, this, scope, mode, backing_store_transaction);
  IndexedDBTransaction* transaction_ptr = transaction.get();
  transactions_[id] = std::move(transaction);
  return transaction_ptr;
}
