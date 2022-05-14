 void FaviconSource::SendDefaultResponse(int request_id) {
  RefCountedMemory* bytes = NULL;
  if (request_size_map_[request_id] == 32) {
    if (!default_favicon_large_.get()) {
      default_favicon_large_ =
          ResourceBundle::GetSharedInstance().LoadDataResourceBytes(
              IDR_DEFAULT_LARGE_FAVICON);
    }
    bytes = default_favicon_large_;
  } else {
    if (!default_favicon_.get()) {
      default_favicon_ =
          ResourceBundle::GetSharedInstance().LoadDataResourceBytes(
              IDR_DEFAULT_FAVICON);
    }
    bytes = default_favicon_;
   }
  request_size_map_.erase(request_id);
 
  SendResponse(request_id, bytes);
 }
