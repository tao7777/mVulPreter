void WebViewPlugin::destroy() {
  if (delegate_) {
     delegate_->WillDestroyPlugin();
     delegate_ = NULL;
   }
  if (container_)
    container_->element().setAttribute("title", old_title_);
   container_ = NULL;
   MessageLoop::current()->DeleteSoon(FROM_HERE, this);
 }
