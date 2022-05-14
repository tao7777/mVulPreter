scoped_refptr<BrowserContext> BrowserContextImpl::GetOffTheRecordContext() {
BrowserContext* BrowserContextImpl::GetOffTheRecordContext() {
   if (!otr_context_) {
    otr_context_ =
        base::MakeUnique<OTRBrowserContextImpl>(
            this,
            static_cast<BrowserContextIODataImpl*>(io_data()));
   }
 
  return otr_context_.get();
}

BrowserContextImpl::~BrowserContextImpl() {
  CHECK(!otr_context_);
 }
