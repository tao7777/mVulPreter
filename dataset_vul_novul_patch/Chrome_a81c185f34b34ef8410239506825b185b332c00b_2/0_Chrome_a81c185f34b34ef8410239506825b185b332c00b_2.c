void WebResourceService::StartFetch() {
  ScheduleFetch(cache_update_delay_ms_);

  prefs_->SetString(last_update_time_pref_name_,
                    base::DoubleToString(base::Time::Now().ToDoubleT()));

  if (in_fetch_)
    return;
  in_fetch_ = true;

  GURL web_resource_server =
      application_locale_.empty()
          ? web_resource_server_
          : google_util::AppendGoogleLocaleParam(web_resource_server_,
                                                 application_locale_);

   DVLOG(1) << "WebResourceService StartFetch " << web_resource_server;
   url_fetcher_ =
       net::URLFetcher::Create(web_resource_server, net::URLFetcher::GET, this);
  data_use_measurement::DataUseUserData::AttachToFetcher(
      url_fetcher_.get(),
      data_use_measurement::DataUseUserData::WEB_RESOURCE_SERVICE);
   url_fetcher_->SetLoadFlags(net::LOAD_DISABLE_CACHE |
                             net::LOAD_DO_NOT_SEND_COOKIES |
                             net::LOAD_DO_NOT_SAVE_COOKIES);
  url_fetcher_->SetRequestContext(request_context_.get());
  url_fetcher_->Start();
}
