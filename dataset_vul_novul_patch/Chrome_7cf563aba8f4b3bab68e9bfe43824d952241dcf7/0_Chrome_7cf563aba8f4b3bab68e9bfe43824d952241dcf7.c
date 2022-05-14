   Core(const OAuthProviderInfo& info,
        net::URLRequestContextGetter* request_context_getter)
       : provider_info_(info),
         request_context_getter_(request_context_getter),
        delegate_(NULL),
        url_fetcher_type_(URL_FETCHER_NONE) {
   }
