 bool WebMediaPlayerImpl::HasSingleSecurityOrigin() const {
  if (demuxer_found_hls_) {
    // HLS manifests might pull segments from a different origin. We can't know
    // for sure, so we conservatively say no here.
    return false;
  }

   if (data_source_)
     return data_source_->HasSingleOrigin();
   return true;
}
