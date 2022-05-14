void FaviconSource::StartDataRequest(const std::string& path,
                                     bool is_incognito,
                                     int request_id) {
   FaviconService* favicon_service =
       profile_->GetFaviconService(Profile::EXPLICIT_ACCESS);
   if (favicon_service) {
    FaviconService::Handle handle;
     if (path.empty()) {
       SendDefaultResponse(request_id);
       return;
     }
 
     if (path.size() > 8 && path.substr(0, 8) == "iconurl/") {
       handle = favicon_service->GetFavicon(
          GURL(path.substr(8)),
          history::FAVICON,
           &cancelable_consumer_,
           NewCallback(this, &FaviconSource::OnFaviconDataAvailable));
     } else {
       handle = favicon_service->GetFaviconForURL(
          GURL(path),
           icon_types_,
           &cancelable_consumer_,
           NewCallback(this, &FaviconSource::OnFaviconDataAvailable));
    }
    cancelable_consumer_.SetClientData(favicon_service, handle, request_id);
  } else {
    SendResponse(request_id, NULL);
  }
}
