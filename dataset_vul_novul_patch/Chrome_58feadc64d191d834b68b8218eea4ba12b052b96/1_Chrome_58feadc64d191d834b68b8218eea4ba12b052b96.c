 void QuotaTask::DeleteSoon() {
   MessageLoop::current()->DeleteSoon(FROM_HERE, this);
 }
