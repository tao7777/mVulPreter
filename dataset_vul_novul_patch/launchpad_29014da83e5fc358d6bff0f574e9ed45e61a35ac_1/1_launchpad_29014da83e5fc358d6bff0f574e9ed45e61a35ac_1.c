scoped_refptr<BrowserContext> BrowserContextImpl::GetOffTheRecordContext() {
   if (!otr_context_) {
    OTRBrowserContextImpl* context = new OTRBrowserContextImpl(
        this,
        static_cast<BrowserContextIODataImpl *>(io_data()));
    otr_context_ = context->GetWeakPtr();
   }
 
  return make_scoped_refptr(otr_context_.get());
 }
