 void QuotaTask::DeleteSoon() {
  DCHECK(original_task_runner_->BelongsToCurrentThread());
  if (delete_scheduled_)
    return;
  delete_scheduled_ = true;
   MessageLoop::current()->DeleteSoon(FROM_HERE, this);
 }
