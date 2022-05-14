 QuotaTask::QuotaTask(QuotaTaskObserver* observer)
     : observer_(observer),
      original_task_runner_(base::MessageLoopProxy::current()) {
 }
