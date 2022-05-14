void LogoService::GetLogo(search_provider_logos::LogoObserver* observer) {
  LogoCallbacks callbacks;
  callbacks.on_cached_decoded_logo_available =
      base::BindOnce(ObserverOnLogoAvailable, observer, true);
  callbacks.on_fresh_decoded_logo_available =
      base::BindOnce(ObserverOnLogoAvailable, observer, false);
  GetLogo(std::move(callbacks));
}
