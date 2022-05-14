 void BrowserContextDestroyer::FinishDestroyContext() {
  DCHECK_EQ(pending_hosts_, 0U);
 
  delete context_;
  context_ = nullptr;
 
   delete this;
 }
