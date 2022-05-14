 void HistoryModelWorker::DoWorkAndWaitUntilDone(Callback0::Type* work) {
   WaitableEvent done(false, false);
   scoped_refptr<WorkerTask> task(new WorkerTask(work, &done));
  history_service_->ScheduleDBTask(task.get(), this);
   done.Wait();
 }
