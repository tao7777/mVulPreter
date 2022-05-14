KeyedService* LogoServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  Profile* profile = static_cast<Profile*>(context);
  DCHECK(!profile->IsOffTheRecord());
#if defined(OS_ANDROID)
  bool use_gray_background = !GetIsChromeHomeEnabled();
 #else
   bool use_gray_background = false;
 #endif
  return new LogoService(profile->GetPath().Append(kCachedLogoDirectory),
                         TemplateURLServiceFactory::GetForProfile(profile),
                         base::MakeUnique<suggestions::ImageDecoderImpl>(),
                         profile->GetRequestContext(), use_gray_background);
 }
