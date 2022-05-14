 QuotaTask::QuotaTask(QuotaTaskObserver* observer)
     : observer_(observer),
      original_task_runner_(base::MessageLoopProxy::current()),
      delete_scheduled_(false) {
 }
