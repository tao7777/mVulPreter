 void RegisterDomDistillerViewerSource(Profile* profile) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kEnableDomDistiller)) {
    dom_distiller::DomDistillerServiceFactory* dom_distiller_service_factory =
        dom_distiller::DomDistillerServiceFactory::GetInstance();
     dom_distiller::LazyDomDistillerService* lazy_service =
         new dom_distiller::LazyDomDistillerService(
             profile, dom_distiller_service_factory);
    scoped_ptr<dom_distiller::ExternalFeedbackReporter> reporter;
 
 #if defined(OS_ANDROID)
    reporter.reset(
        new dom_distiller::android::ExternalFeedbackReporterAndroid());
 #endif  // defined(OS_ANDROID)
 
    if (!dom_distiller::DistillerJavaScriptWorldIdIsSet()) {
      dom_distiller::SetDistillerJavaScriptWorldId(
          chrome::ISOLATED_WORLD_ID_CHROME_INTERNAL);
    }

     content::URLDataSource::Add(
         profile,
         new dom_distiller::DomDistillerViewerSource(
            lazy_service, dom_distiller::kDomDistillerScheme, reporter.Pass()));
   }
 }
