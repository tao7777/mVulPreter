void LogoService::GetLogo(LogoCallbacks callbacks) {
  if (!template_url_service_) {
    RunCallbacksWithDisabled(std::move(callbacks));
    return;
  }
  const TemplateURL* template_url =
      template_url_service_->GetDefaultSearchProvider();
  if (!template_url) {
    RunCallbacksWithDisabled(std::move(callbacks));
    return;
  }
  const base::CommandLine* command_line =
      base::CommandLine::ForCurrentProcess();
  GURL logo_url;
  if (command_line->HasSwitch(switches::kSearchProviderLogoURL)) {
    logo_url = GURL(
        command_line->GetSwitchValueASCII(switches::kSearchProviderLogoURL));
  } else {
#if defined(OS_ANDROID)
    logo_url = template_url->logo_url();
#endif
  }
  GURL base_url;
  GURL doodle_url;
  const bool is_google = template_url->url_ref().HasGoogleBaseURLs(
      template_url_service_->search_terms_data());
  if (is_google) {
    base_url =
        GURL(template_url_service_->search_terms_data().GoogleBaseURLValue());
    doodle_url = search_provider_logos::GetGoogleDoodleURL(base_url);
  } else if (base::FeatureList::IsEnabled(features::kThirdPartyDoodles)) {
    if (command_line->HasSwitch(switches::kThirdPartyDoodleURL)) {
      doodle_url = GURL(
          command_line->GetSwitchValueASCII(switches::kThirdPartyDoodleURL));
    } else {
      std::string override_url = base::GetFieldTrialParamValueByFeature(
          features::kThirdPartyDoodles,
          features::kThirdPartyDoodlesOverrideUrlParam);
      if (!override_url.empty()) {
        doodle_url = GURL(override_url);
      } else {
        doodle_url = template_url->doodle_url();
      }
    }
    base_url = doodle_url.GetOrigin();
  }
  if (!logo_url.is_valid() && !doodle_url.is_valid()) {
    RunCallbacksWithDisabled(std::move(callbacks));
    return;
  }
  const bool use_fixed_logo = !doodle_url.is_valid();
  if (!logo_tracker_) {
    std::unique_ptr<LogoCache> logo_cache = std::move(logo_cache_for_test_);
    if (!logo_cache) {
      logo_cache = std::make_unique<LogoCache>(cache_directory_);
    }
    std::unique_ptr<base::Clock> clock = std::move(clock_for_test_);
    if (!clock) {
      clock = std::make_unique<base::DefaultClock>();
    }
    logo_tracker_ = std::make_unique<LogoTracker>(
        request_context_getter_,
        std::make_unique<LogoDelegateImpl>(std::move(image_decoder_)),
        std::move(logo_cache), std::move(clock));
  }
  if (use_fixed_logo) {
    logo_tracker_->SetServerAPI(
        logo_url, base::Bind(&search_provider_logos::ParseFixedLogoResponse),
        base::Bind(&search_provider_logos::UseFixedLogoUrl));
  } else if (is_google) {
    logo_tracker_->SetServerAPI(
        doodle_url,
        search_provider_logos::GetGoogleParseLogoResponseCallback(base_url),
        search_provider_logos::GetGoogleAppendQueryparamsCallback(
            use_gray_background_));
  } else {
    logo_tracker_->SetServerAPI(
        doodle_url,
        base::Bind(&search_provider_logos::GoogleNewParseLogoResponse,
                   base_url),
        base::Bind(&search_provider_logos::GoogleNewAppendQueryparamsToLogoURL,
                   use_gray_background_));
  }
  logo_tracker_->GetLogo(std::move(callbacks));
}
