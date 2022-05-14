void WebViewPlugin::destroy() {
  if (delegate_) {
     delegate_->WillDestroyPlugin();
     delegate_ = NULL;
   }
   container_ = NULL;
   MessageLoop::current()->DeleteSoon(FROM_HERE, this);
 }
