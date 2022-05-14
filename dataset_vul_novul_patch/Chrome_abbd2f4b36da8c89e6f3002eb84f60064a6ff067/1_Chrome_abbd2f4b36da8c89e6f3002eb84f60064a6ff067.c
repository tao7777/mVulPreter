 void FaviconSource::SendDefaultResponse(int request_id) {
  if (!default_favicon_.get()) {
    default_favicon_ =
        ResourceBundle::GetSharedInstance().LoadDataResourceBytes(
            IDR_DEFAULT_FAVICON);
   }
 
  SendResponse(request_id, default_favicon_);
 }
