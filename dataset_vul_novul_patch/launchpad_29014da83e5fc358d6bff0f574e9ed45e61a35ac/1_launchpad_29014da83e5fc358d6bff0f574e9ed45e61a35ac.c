WebContext* WebContext::FromBrowserContext(oxide::BrowserContext* context) {
   BrowserContextDelegate* delegate =
       static_cast<BrowserContextDelegate*>(context->GetDelegate());
   if (!delegate) {
    return nullptr;
  }

   return delegate->context();
 }
