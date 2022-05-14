 KioskNextHomeInterfaceBrokerImpl::KioskNextHomeInterfaceBrokerImpl(
     content::BrowserContext* context)
    : connector_(content::BrowserContext::GetConnectorFor(context)->Clone()),
      app_controller_(std::make_unique<AppControllerImpl>(
          Profile::FromBrowserContext(context))) {}
