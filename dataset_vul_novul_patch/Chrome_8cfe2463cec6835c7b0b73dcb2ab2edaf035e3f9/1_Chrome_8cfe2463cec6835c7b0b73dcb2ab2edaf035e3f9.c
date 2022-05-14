AppControllerImpl::AppControllerImpl(Profile* profile)
//// static
     : profile_(profile),
       app_service_proxy_(apps::AppServiceProxy::Get(profile)),
       url_prefix_(base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
           chromeos::switches::kKioskNextHomeUrlPrefix)) {
   app_service_proxy_->AppRegistryCache().AddObserver(this);
 
  if (profile) {
    content::URLDataSource::Add(profile,
                                std::make_unique<apps::AppIconSource>(profile));
  }
 }
