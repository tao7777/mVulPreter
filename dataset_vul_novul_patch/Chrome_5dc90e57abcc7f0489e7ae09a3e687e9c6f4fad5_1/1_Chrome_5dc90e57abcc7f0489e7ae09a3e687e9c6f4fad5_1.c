void WebSocketJob::Wakeup() {
  if (!waiting_)
    return;
  waiting_ = false;
   DCHECK(callback_);
   MessageLoopForIO::current()->PostTask(
       FROM_HERE,
      NewRunnableMethod(this, &WebSocketJob::RetryPendingIO));
 }
