void IndexedDBTransaction::Abort(const IndexedDBDatabaseError& error) {
  IDB_TRACE1("IndexedDBTransaction::Abort", "txn.id", id());
  DCHECK(!processing_event_queue_);
  if (state_ == FINISHED)
    return;

  UMA_HISTOGRAM_ENUMERATION("WebCore.IndexedDB.TransactionAbortReason",
                            ExceptionCodeToUmaEnum(error.code()),
                            UmaIDBExceptionExclusiveMaxValue);

  timeout_timer_.Stop();

  state_ = FINISHED;
  should_process_queue_ = false;

  if (backing_store_transaction_begun_)
    transaction_->Rollback();

  while (!abort_task_stack_.empty())
    abort_task_stack_.pop().Run();

  preemptive_task_queue_.clear();
  pending_preemptive_events_ = 0;
  task_queue_.clear();

  CloseOpenCursors();
  transaction_->Reset();

  database_->transaction_coordinator().DidFinishTransaction(this);
#ifndef NDEBUG
  DCHECK(!database_->transaction_coordinator().IsActive(this));
#endif

  if (callbacks_.get())
    callbacks_->OnAbort(*this, error);

   database_->TransactionFinished(this, false);
 
  // Note: During force-close situations, the connection can be destroyed during
  // the |IndexedDBDatabase::TransactionFinished| call
  if (connection_)
    connection_->RemoveTransaction(id_);
 }
