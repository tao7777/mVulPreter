 OTRBrowserContextImpl::OTRBrowserContextImpl(
     BrowserContextImpl* original,
     BrowserContextIODataImpl* original_io_data)
     : BrowserContext(new OTRBrowserContextIODataImpl(original_io_data)),
      original_context_(original) {
   BrowserContextDependencyManager::GetInstance()
       ->CreateBrowserContextServices(this);
 }
