void FetchManager::Loader::Start() {





  if (!ContentSecurityPolicy::ShouldBypassMainWorld(execution_context_) &&
      !execution_context_->GetContentSecurityPolicy()->AllowConnectToSource(
          fetch_request_data_->Url())) {
    PerformNetworkError(
        "Refused to connect to '" + fetch_request_data_->Url().ElidedString() +
        "' because it violates the document's Content Security Policy.");
    return;
  }

  if ((SecurityOrigin::Create(fetch_request_data_->Url())
           ->IsSameSchemeHostPort(fetch_request_data_->Origin().get())) ||
      (fetch_request_data_->Url().ProtocolIsData() &&
       fetch_request_data_->SameOriginDataURLFlag()) ||
      (fetch_request_data_->Mode() == FetchRequestMode::kNavigate)) {
    PerformSchemeFetch();
    return;
  }

  if (fetch_request_data_->Mode() == FetchRequestMode::kSameOrigin) {
    PerformNetworkError("Fetch API cannot load " +
                        fetch_request_data_->Url().GetString() +
                        ". Request mode is \"same-origin\" but the URL\'s "
                        "origin is not same as the request origin " +
                        fetch_request_data_->Origin()->ToString() + ".");
    return;
  }
 
   if (fetch_request_data_->Mode() == FetchRequestMode::kNoCORS) {
    // "If |request|'s redirect mode is not |follow|, then return a network
    // error.
    if (fetch_request_data_->Redirect() != FetchRedirectMode::kFollow) {
      PerformNetworkError("Fetch API cannot load " +
                          fetch_request_data_->Url().GetString() +
                          ". Request mode is \"no-cors\" but the redirect mode "
                          " is not \"follow\".");
      return;
    }

     fetch_request_data_->SetResponseTainting(FetchRequestData::kOpaqueTainting);
    PerformSchemeFetch();
    return;
  }

  if (!SchemeRegistry::ShouldTreatURLSchemeAsSupportingFetchAPI(
          fetch_request_data_->Url().Protocol())) {
    PerformNetworkError(
        "Fetch API cannot load " + fetch_request_data_->Url().GetString() +
        ". URL scheme must be \"http\" or \"https\" for CORS request.");
    return;
  }

  fetch_request_data_->SetResponseTainting(FetchRequestData::kCORSTainting);

  PerformHTTPFetch();
}
