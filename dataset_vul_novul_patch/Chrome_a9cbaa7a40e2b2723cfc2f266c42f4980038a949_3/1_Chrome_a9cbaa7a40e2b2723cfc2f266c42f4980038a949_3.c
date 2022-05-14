bool MultibufferDataSource::DidPassCORSAccessCheck() const {
  if (url_data()->cors_mode() == UrlData::CORS_UNSPECIFIED)
    return false;
  if (init_cb_)
    return false;
  if (failed_)
    return false;
  return true;
}
