bool WebMediaPlayerImpl::DidGetOpaqueResponseFromServiceWorker() const {
  if (data_source_)
    return data_source_->DidGetOpaqueResponseViaServiceWorker();
  return false;
}
