void FaviconSource::StartDataRequest(const std::string& path,
                                     bool is_incognito,
                                     int request_id) {
   FaviconService* favicon_service =
       profile_->GetFaviconService(Profile::EXPLICIT_ACCESS);
   if (favicon_service) {
     if (path.empty()) {
       SendDefaultResponse(request_id);
       return;
     }
 
    FaviconService::Handle handle;
     if (path.size() > 8 && path.substr(0, 8) == "iconurl/") {
       handle = favicon_service->GetFavicon(
          GURL(path.substr(8)),
          history::FAVICON,
           &cancelable_consumer_,
           NewCallback(this, &FaviconSource::OnFaviconDataAvailable));
     } else {
      GURL url;

      if (path.size() > 5 && path.substr(0, 5) == "size/") {
        size_t slash = path.find("/", 5);
        std::string size = path.substr(5, slash - 5);
        int pixel_size = atoi(size.c_str());
        CHECK(pixel_size == 32 || pixel_size == 16) <<
            "only 32x32 and 16x16 icons are supported";
        request_size_map_[request_id] = pixel_size;
        url = GURL(path.substr(slash + 1));
      } else {
        request_size_map_[request_id] = 16;
        url = GURL(path);
      }

      // TODO(estade): fetch the requested size.
       handle = favicon_service->GetFaviconForURL(
          url,
           icon_types_,
           &cancelable_consumer_,
           NewCallback(this, &FaviconSource::OnFaviconDataAvailable));
    }
    cancelable_consumer_.SetClientData(favicon_service, handle, request_id);
  } else {
    SendResponse(request_id, NULL);
  }
}
